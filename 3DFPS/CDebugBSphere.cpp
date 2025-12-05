#include "stdafx.h"
#include "CDebugBSphere.h"
#include <d3dcompiler.h>

#include <stdlib.h>	//マルチバイト文字→Unicode文字変換で必要.
#include <locale.h>

static const TCHAR* DEBUGLINE_HLSL = _T("Data\\Shader\\DebugLine.hlsl");

CDebugBSphere::CDebugBSphere()
	: m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pLayout(nullptr)
	, m_pCBuffer(nullptr)
	, m_pVertexBuffer(nullptr)
	, m_MaxVBVerts(0)
{
}

CDebugBSphere::~CDebugBSphere()
{
	Release();
}

HRESULT CDebugBSphere::Init(ID3D11Device* device)
{
    if (FAILED(CreateShader(device))) return E_FAIL;
    const int maxVerts = 3 * (128 + 1);
    if (FAILED(CreateBuffers(device, maxVerts))) return E_FAIL;
    return S_OK;
}

void CDebugBSphere::Release()
{
    SAFE_RELEASE(m_pVertexShader);
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pLayout);
    SAFE_RELEASE(m_pCBuffer);
    SAFE_RELEASE(m_pVertexBuffer);
}

HRESULT CDebugBSphere::CreateShader(ID3D11Device* device)
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* err = nullptr;
    UINT flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    if (FAILED(D3DX11CompileFromFile(DEBUGLINE_HLSL, nullptr, nullptr, "VS_Main", "vs_5_0",
        flags, 0, nullptr, &vsBlob, &err, nullptr)))
    {
        if (err) { OutputDebugStringA((char*)err->GetBufferPointer()); err->Release(); } return E_FAIL;
    }

    if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_pVertexShader)))
    {
        vsBlob->Release(); return E_FAIL;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    if (FAILED(device->CreateInputLayout(layout, _countof(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_pLayout)))
    {
        vsBlob->Release(); return E_FAIL;
    }
    vsBlob->Release();

    if (FAILED(D3DX11CompileFromFile(DEBUGLINE_HLSL, nullptr, nullptr, "PS_Main", "ps_5_0",
        flags, 0, nullptr, &psBlob, &err, nullptr)))
    {
        if (err) { OutputDebugStringA((char*)err->GetBufferPointer()); err->Release(); } return E_FAIL;
    }

    if (FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pPixelShader)))
    {
        psBlob->Release(); return E_FAIL;
    }
    psBlob->Release();

    D3D11_BUFFER_DESC cbd{};
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.ByteWidth = sizeof(CBUFFER_VP);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(device->CreateBuffer(&cbd, nullptr, &m_pCBuffer))) return E_FAIL;

    return S_OK;
}

HRESULT CDebugBSphere::CreateBuffers(ID3D11Device* device, int maxVerts)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(VERTEX) * maxVerts;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(device->CreateBuffer(&bd, nullptr, &m_pVertexBuffer))) return E_FAIL;
    m_MaxVBVerts = maxVerts;
    m_TmpVerts.reserve(maxVerts);
    return S_OK;
}

void CDebugBSphere::ApplyVP(ID3D11DeviceContext* ctx, const D3DXMATRIX& mVP)
{
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (SUCCEEDED(ctx->Map(m_pCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        D3DXMATRIX t = mVP;
        D3DXMatrixTranspose(&t, &t);
        CBUFFER_VP* cb = reinterpret_cast<CBUFFER_VP*>(mapped.pData);
        cb->mWVP = t;
        ctx->Unmap(m_pCBuffer, 0);
    }
    ctx->VSSetConstantBuffers(0, 1, &m_pCBuffer);
}

void CDebugBSphere::AppendRing(std::vector<VERTEX>& out, const D3DXVECTOR3& c, float r,
    const D3DXVECTOR4& color, int segments, int axis) // 0:XY,1:XZ,2:YZ
{
    const float deltaTheta = D3DX_PI * 2.0f / segments;
    for (int i = 0; i <= segments; ++i)
    {
        const float theta = i * deltaTheta;
        D3DXVECTOR3 pos;
        switch (axis)
        {
        case 0: // XY
            pos = D3DXVECTOR3(c.x + r * cosf(theta), c.y + r * sinf(theta), c.z);
            break;
        case 1: // XZ
            pos = D3DXVECTOR3(c.x + r * cosf(theta), c.y, c.z + r * sinf(theta));
            break;
        case 2: // YZ
            pos = D3DXVECTOR3(c.x, c.y + r * cosf(theta), c.z + r * sinf(theta));
            break;
        default:
            pos = c;
            break;
        }
        out.push_back({ pos, color });
    }
}

void CDebugBSphere::DrawSphere(ID3D11DeviceContext* ctx,
    const D3DXMATRIX& mView,
    const D3DXMATRIX& mProj,
    const D3DXVECTOR3& position,
    float radius,
    const D3DXVECTOR4& color)
{

	float centerX = position.x;
    float centerY = position.y;
	float centerZ = position.z;
	D3DXVECTOR3 center(centerX, centerY, centerZ);

    m_TmpVerts.clear();
    const int segments = 128;
    AppendRing(m_TmpVerts, center, radius, color, segments, 0); // XY
    AppendRing(m_TmpVerts, center, radius, color, segments, 1); // XZ
    AppendRing(m_TmpVerts, center, radius, color, segments, 2); // YZ

    if ((int)m_TmpVerts.size() > m_MaxVBVerts) return;

    D3DXMATRIX mVP = mView * mProj;
    ApplyVP(ctx, mVP);

    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(ctx->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
    memcpy(mapped.pData, m_TmpVerts.data(), sizeof(VERTEX) * m_TmpVerts.size());
    ctx->Unmap(m_pVertexBuffer, 0);

    UINT stride = sizeof(VERTEX), offset = 0;
    ctx->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
    ctx->IASetInputLayout(m_pLayout);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
    ctx->VSSetShader(m_pVertexShader, nullptr, 0);
    ctx->PSSetShader(m_pPixelShader, nullptr, 0);

    // draw 3 independent rings (no connection lines)
    const UINT vertsPerRing = segments + 1;
    for (UINT r = 0; r < 3; ++r)
        ctx->Draw(vertsPerRing, r * vertsPerRing);
}