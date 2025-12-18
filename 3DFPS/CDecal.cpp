#include "stdafx.h"
#include "CDecal.h"
#include "CDirectX11.h"

const TCHAR DECAL_SHADER_NAME[] = _T("Data\\Shader\\Decal.hlsl");

CDecal::CDecal()
	: CSprite3D()
	, m_FadeDistance(5.0f)
{
}

CDecal::~CDecal()
{
}

HRESULT CDecal::Init(CDirectX11& pDx11, LPCTSTR lpFileName, SPRITE_STATE& pSs)
{
	// Store DirectX reference first
	m_pDx11 = &pDx11;
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	m_SpriteState = pSs;

	// Create shader with custom decal shader
	if (FAILED(CreateShader()))
	{
		return E_FAIL;
	}

	// Create model (polygon)
	if (FAILED(CreateModel()))
	{
		return E_FAIL;
	}

	// Create texture
	if (FAILED(CreateTexture(lpFileName)))
	{
		return E_FAIL;
	}

	// Create sampler
	if (FAILED(CreateSampler()))
	{
		return E_FAIL;
	}

	// Set default fade distance
	m_FadeDistance = 5.0f;

	return S_OK;
}

HRESULT CDecal::CreateShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	// Compile vertex shader from HLSL
	if (FAILED(
		D3DX11CompileFromFile(
			DECAL_SHADER_NAME,
			nullptr,
			nullptr,
			"VS_Main",
			"vs_5_0",
			uCompileFlag,
			0,
			nullptr,
			&pCompiledShader,
			&pErrors,
			nullptr)))
	{
		if (pErrors)
		{
			// Output the error message for debugging
			OutputDebugStringA((char*)pErrors->GetBufferPointer());
			SAFE_RELEASE(pErrors);
		}
		_ASSERT_EXPR(false, _T("hlsl読み込み失敗"));
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// Create vertex shader
	if (FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		_ASSERT_EXPR(false, _T("バーテックスシェーダ作成失敗"));
		return E_FAIL;
	}

	// Define vertex input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			12,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Create input layout
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))
	{
		SAFE_RELEASE(pCompiledShader);
		_ASSERT_EXPR(false, _T("Decal input layout creation failed"));
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	// Compile pixel shader from HLSL
	if (FAILED(
		D3DX11CompileFromFile(
			DECAL_SHADER_NAME,
			nullptr,
			nullptr,
			"PS_Main",
			"ps_5_0",
			uCompileFlag,
			0,
			nullptr,
			&pCompiledShader,
			&pErrors,
			nullptr)))
	{
		if (pErrors)
		{
			OutputDebugStringA((char*)pErrors->GetBufferPointer());
			SAFE_RELEASE(pErrors);
		}
		_ASSERT_EXPR(false, _T("Decal pixel shader compilation failed"));
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// Create pixel shader
	if (FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pPixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		_ASSERT_EXPR(false, _T("Decal pixel shader creation failed"));
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	// Create constant buffer
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_CONSTANT_BUFFER);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(
		m_pDevice11->CreateBuffer(
			&cb,
			nullptr,
			&m_pConstantBuffer)))
	{
		_ASSERT_EXPR(false, _T("Decal constant buffer creation failed"));
		return E_FAIL;
	}

	return S_OK;
}

void CDecal::RenderDecal(D3DXMATRIX& mView, D3DXMATRIX& mProj, const D3DXVECTOR3& normal)
{


	CSprite3D::Render(mView, mProj);
}