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
	// まずDirectXの参照を保持する.
	m_pDx11 = &pDx11;
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	m_SpriteState = pSs;

	// カスタムのデカールシェーダでシェーダを作成.
	if (FAILED(CreateShader()))
	{
		return E_FAIL;
	}

	// モデル（ポリゴン）を作成.
	if (FAILED(CreateModel()))
	{
		return E_FAIL;
	}

	// テクスチャを作成.
	if (FAILED(CreateTexture(lpFileName)))
	{
		return E_FAIL;
	}

	// サンプラを作成.
	if (FAILED(CreateSampler()))
	{
		return E_FAIL;
	}

	// デフォルトのフェード距離を設定.
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

	// HLSLからバーテックスシェーダをコンパイル.
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
			// デバッグ用にエラーメッセージを出力.
			OutputDebugStringA((char*)pErrors->GetBufferPointer());
			SAFE_RELEASE(pErrors);
		}
		_ASSERT_EXPR(false, _T("hlsl読み込み失敗"));
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	// バーテックスシェーダを作成.
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

	// 頂点インプットレイアウトを定義.
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

	// 頂点インプットレイアウトを作成.
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

	// HLSLからピクセルシェーダをコンパイル.
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

	// ピクセルシェーダを作成.
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

	// コンスタントバッファを作成.
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