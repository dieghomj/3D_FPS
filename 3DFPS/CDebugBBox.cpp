#include "stdafx.h"
#include "CDebugBBox.h"

static const TCHAR* DEBUGLINE_HLSL = _T("Data\\Shader\\DebugLine.hlsl");

CDebugBBox::CDebugBBox()
	: m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pLayout(nullptr)
	, m_pCBuffer(nullptr)
	, m_pVertexBuffer(nullptr)
	, m_MaxVBVerts(0)
{
}

CDebugBBox::~CDebugBBox()
{
	Release();
}

HRESULT CDebugBBox::Init(ID3D11Device* device)
{
	if (FAILED(CreateShader(device))) return E_FAIL;
	// 12 edges * 2 vertices each = 24 vertices
	const int maxVerts = 24;
	if (FAILED(CreateBuffers(device, maxVerts))) return E_FAIL;
	return S_OK;
}

void CDebugBBox::Release()
{
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pLayout);
	SAFE_RELEASE(m_pCBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
}

HRESULT CDebugBBox::CreateShader(ID3D11Device* device)
{
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* err = nullptr;
	UINT flags = 0;
#ifdef _DEBUG
	flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// VS
	if (FAILED(D3DX11CompileFromFile(DEBUGLINE_HLSL, nullptr, nullptr, "VS_Main", "vs_5_0",
		flags, 0, nullptr, &vsBlob, &err, nullptr)))
	{
		if (err) { OutputDebugStringA((const char*)err->GetBufferPointer()); err->Release(); }
		return E_FAIL;
	}
	if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_pVertexShader)))
	{
		vsBlob->Release();
		return E_FAIL;
	}

	// Input layout
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                          D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	if (FAILED(device->CreateInputLayout(layout, _countof(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_pLayout)))
	{
		vsBlob->Release();
		return E_FAIL;
	}
	vsBlob->Release();

	// PS
	if (FAILED(D3DX11CompileFromFile(DEBUGLINE_HLSL, nullptr, nullptr, "PS_Main", "ps_5_0",
		flags, 0, nullptr, &psBlob, &err, nullptr)))
	{
		if (err) { OutputDebugStringA((const char*)err->GetBufferPointer()); err->Release(); }
		return E_FAIL;
	}
	if (FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pPixelShader)))
	{
		psBlob->Release();
		return E_FAIL;
	}
	psBlob->Release();

	// Constant buffer (VP)
	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.ByteWidth = sizeof(CBUFFER_VP);
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(device->CreateBuffer(&cbd, nullptr, &m_pCBuffer))) return E_FAIL;

	return S_OK;
}

HRESULT CDebugBBox::CreateBuffers(ID3D11Device* device, int maxVerts)
{
	m_MaxVBVerts = maxVerts;
	m_TmpVerts.reserve(maxVerts);

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(VERTEX) * m_MaxVBVerts;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	return device->CreateBuffer(&bd, nullptr, &m_pVertexBuffer);
}

void CDebugBBox::ApplyVP(ID3D11DeviceContext* ctx, const D3DXMATRIX& mVP)
{
	D3D11_MAPPED_SUBRESOURCE mapped{};
	if (SUCCEEDED(ctx->Map(m_pCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		CBUFFER_VP* cb = reinterpret_cast<CBUFFER_VP*>(mapped.pData);
		D3DXMATRIX t = mVP;
		D3DXMatrixTranspose(&t, &t);
		cb->mWVP = t;
		ctx->Unmap(m_pCBuffer, 0);
	}
	ctx->VSSetConstantBuffers(0, 1, &m_pCBuffer);
}

void CDebugBBox::AppendBox(std::vector<VERTEX>& out, const D3DXVECTOR3& min, const D3DXVECTOR3& max, const D3DXVECTOR4& color)
{
	// 8 corners
	D3DXVECTOR3 p000(min.x, min.y, min.z);
	D3DXVECTOR3 p100(max.x, min.y, min.z);
	D3DXVECTOR3 p110(max.x, max.y, min.z);
	D3DXVECTOR3 p010(min.x, max.y, min.z);

	D3DXVECTOR3 p001(min.x, min.y, max.z);
	D3DXVECTOR3 p101(max.x, min.y, max.z);
	D3DXVECTOR3 p111(max.x, max.y, max.z);
	D3DXVECTOR3 p011(min.x, max.y, max.z);

	// 12 edges as line list (24 vertices)

	// bottom face (y=min)
	out.push_back({ p000, color }); out.push_back({ p100, color });
	out.push_back({ p100, color }); out.push_back({ p101, color });
	out.push_back({ p101, color }); out.push_back({ p001, color });
	out.push_back({ p001, color }); out.push_back({ p000, color });

	// top face (y=max)
	out.push_back({ p010, color }); out.push_back({ p110, color });
	out.push_back({ p110, color }); out.push_back({ p111, color });
	out.push_back({ p111, color }); out.push_back({ p011, color });
	out.push_back({ p011, color }); out.push_back({ p010, color });

	// vertical edges
	out.push_back({ p000, color }); out.push_back({ p010, color });
	out.push_back({ p100, color }); out.push_back({ p110, color });
	out.push_back({ p101, color }); out.push_back({ p111, color });
	out.push_back({ p001, color }); out.push_back({ p011, color });
}

void CDebugBBox::DrawBox(ID3D11DeviceContext* ctx,
	const D3DXMATRIX& mView,
	const D3DXMATRIX& mProj,
	const D3DXVECTOR3& min,
	const D3DXVECTOR3& max,
	const D3DXVECTOR4& color)
{
	m_TmpVerts.clear();
	AppendBox(m_TmpVerts, min, max, color);
	if ((int)m_TmpVerts.size() > m_MaxVBVerts) return;

	// Update VP
	D3DXMATRIX mVP = mView * mProj;
	ApplyVP(ctx, mVP);

	// Update VB
	D3D11_MAPPED_SUBRESOURCE mapped{};
	if (FAILED(ctx->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
	memcpy(mapped.pData, m_TmpVerts.data(), sizeof(VERTEX) * m_TmpVerts.size());
	ctx->Unmap(m_pVertexBuffer, 0);

	// Bind and draw
	UINT stride = sizeof(VERTEX), offset = 0;
	ctx->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	ctx->IASetInputLayout(m_pLayout);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	ctx->VSSetShader(m_pVertexShader, nullptr, 0);
	ctx->PSSetShader(m_pPixelShader, nullptr, 0);

	ctx->Draw(static_cast<UINT>(m_TmpVerts.size()), 0);
}