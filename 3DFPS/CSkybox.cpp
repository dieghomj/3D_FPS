#include "stdafx.h"
#include "CSkybox.h"
#include <d3dcompiler.h>

static const TCHAR* SKYBOX_HLSL = _T("Data\\Shader\\Skybox.hlsl");

CSkybox::CSkybox()
	: m_pDx11(nullptr)
	, m_pDevice11(nullptr)
	, m_pContext11(nullptr)
	, m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pInputLayout(nullptr)
	, m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
	, m_pConstantBuffer(nullptr)
	, m_pCubemapSRV(nullptr)
	, m_pSamplerState(nullptr)
	, m_pDepthStencilState(nullptr)
	, m_pRasterizerState(nullptr)
{
}

CSkybox::~CSkybox()
{
	Release();
}

HRESULT CSkybox::Init(CDirectX11& dx11, LPCTSTR lpCubemapFile)
{
	m_initialized = false;
	m_pDx11 = &dx11;
	m_pDevice11 = dx11.GetDevice();
	m_pContext11 = dx11.GetContext();

	if (!m_pDevice11 || !m_pContext11)
		return E_FAIL;

	if (FAILED(CreateShader()))
	{
		OutputDebugString(_T("CSkybox: Failed to create shaders\n"));
		return E_FAIL;
	}

	if (FAILED(CreateBuffers()))
	{
		OutputDebugString(_T("CSkybox: Failed to create buffers\n"));
		return E_FAIL;
	}

	if (FAILED(LoadCubemap(lpCubemapFile)))
	{
		OutputDebugString(_T("CSkybox: Cubemap not loaded - skybox will be disabled\n"));
		return S_OK; // 意図的：スカイボックスがなくてもゲームを動作させる
	}

	m_initialized = true;
	return S_OK;
}

void CSkybox::Release()
{
	m_initialized = false;
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pCubemapSRV);
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pDepthStencilState);
	SAFE_RELEASE(m_pRasterizerState);
}

HRESULT CSkybox::CreateShader()
{
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errBlob = nullptr;

	UINT flags = 0;
#ifdef _DEBUG
	flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// 頂点シェーダーをコンパイルする
	if (FAILED(D3DX11CompileFromFile(
		SKYBOX_HLSL, nullptr, nullptr,
		"VS_Main", "vs_5_0",
		flags, 0, nullptr,
		&vsBlob, &errBlob, nullptr)))
	{
		if (errBlob)
		{
			OutputDebugStringA((const char*)errBlob->GetBufferPointer());
			errBlob->Release();
		}
		return E_FAIL;
	}

	if (FAILED(m_pDevice11->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		nullptr, &m_pVertexShader)))
	{
		vsBlob->Release();
		return E_FAIL;
	}

	// 入力レイアウトを作成する
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(m_pDevice11->CreateInputLayout(
		layout, _countof(layout),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&m_pInputLayout)))
	{
		vsBlob->Release();
		return E_FAIL;
	}
	vsBlob->Release();

	// ピクセルシェーダーをコンパイルする
	if (FAILED(D3DX11CompileFromFile(
		SKYBOX_HLSL, nullptr, nullptr,
		"PS_Main", "ps_5_0",
		flags, 0, nullptr,
		&psBlob, &errBlob, nullptr)))
	{
		if (errBlob)
		{
			OutputDebugStringA((const char*)errBlob->GetBufferPointer());
			errBlob->Release();
		}
		return E_FAIL;
	}

	if (FAILED(m_pDevice11->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		nullptr, &m_pPixelShader)))
	{
		psBlob->Release();
		return E_FAIL;
	}
	psBlob->Release();

	// 定数バッファを作成する
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.ByteWidth = sizeof(CBUFFER_SKYBOX);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(m_pDevice11->CreateBuffer(&cbDesc, nullptr, &m_pConstantBuffer)))
		return E_FAIL;

	// サンプラーステートを作成する
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(m_pDevice11->CreateSamplerState(&samplerDesc, &m_pSamplerState)))
		return E_FAIL;

	// 深度ステンシルステートを作成する（深度テスト有効、深度書き込み無効）
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度バッファには書き込まない
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // 以下なら通過（スカイボックスは最大深度に配置）
	dsDesc.StencilEnable = FALSE;

	if (FAILED(m_pDevice11->CreateDepthStencilState(&dsDesc, &m_pDepthStencilState)))
		return E_FAIL;

	// ラスタライザーステートを作成する（立方体の内側にいるため表面をカリングする）
	D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_FRONT; // 表面をカリングする（立方体の内側にいるため）
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;

	if (FAILED(m_pDevice11->CreateRasterizerState(&rsDesc, &m_pRasterizerState)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkybox::CreateBuffers()
{
	// スカイボックスの立方体頂点（原点中心の単位立方体）
	VERTEX vertices[] = {
		// 前面
		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f) },
		{ D3DXVECTOR3(-1.0f,  1.0f, -1.0f) },
		{ D3DXVECTOR3( 1.0f,  1.0f, -1.0f) },
		{ D3DXVECTOR3( 1.0f, -1.0f, -1.0f) },
		// 背面
		{ D3DXVECTOR3(-1.0f, -1.0f,  1.0f) },
		{ D3DXVECTOR3( 1.0f, -1.0f,  1.0f) },
		{ D3DXVECTOR3( 1.0f,  1.0f,  1.0f) },
		{ D3DXVECTOR3(-1.0f,  1.0f,  1.0f) },
	};

	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.ByteWidth = sizeof(vertices);
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;

	if (FAILED(m_pDevice11->CreateBuffer(&vbDesc, &vbData, &m_pVertexBuffer)))
		return E_FAIL;

	// スカイボックスの立方体インデックス
	WORD indices[] = {
		// 前面
		0, 1, 2,  0, 2, 3,
		// 背面
		4, 5, 6,  4, 6, 7,
		// 左面
		4, 7, 1,  4, 1, 0,
		// 右面
		3, 2, 6,  3, 6, 5,
		// 上面
		1, 7, 6,  1, 6, 2,
		// 下面
		4, 0, 3,  4, 3, 5,
	};

	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.ByteWidth = sizeof(indices);
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices;

	if (FAILED(m_pDevice11->CreateBuffer(&ibDesc, &ibData, &m_pIndexBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkybox::LoadCubemap(LPCTSTR lpCubemapFile)
{
	// まずファイルが存在するか確認する
	DWORD fileAttribs = GetFileAttributes(lpCubemapFile);
	if (fileAttribs == INVALID_FILE_ATTRIBUTES)
	{
		OutputDebugString(_T("CSkybox: Cubemap file not found: "));
		OutputDebugString(lpCubemapFile);
		OutputDebugString(_T("\n"));
		return E_FAIL;
	}

	OutputDebugString(_T("CSkybox: Attempting to load: "));
	OutputDebugString(lpCubemapFile);
	OutputDebugString(_T("\n"));

	// 1回目の試行：自動判別で読み込む（D3DX11がDDSヘッダーからキューブマップを自動判別するはず）
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		m_pDevice11,
		lpCubemapFile,
		nullptr,  // D3DX11にフォーマットを自動判別させる
		nullptr,
		&m_pCubemapSRV,
		nullptr);

	if (SUCCEEDED(hr))
	{
		// キューブマップテクスチャが取得できたか検証する
		ID3D11Resource* pResource = nullptr;
		m_pCubemapSRV->GetResource(&pResource);
		if (pResource)
		{
			D3D11_RESOURCE_DIMENSION dim;
			pResource->GetType(&dim);
			
			if (dim == D3D11_RESOURCE_DIMENSION_TEXTURE2D)
			{
				ID3D11Texture2D* pTex2D = nullptr;
				pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTex2D);
				if (pTex2D)
				{
					D3D11_TEXTURE2D_DESC desc;
					pTex2D->GetDesc(&desc);
					
					TCHAR debugMsg[256];
					_stprintf_s(debugMsg, _T("CSkybox: Loaded texture - Width: %d, Height: %d, ArraySize: %d, MiscFlags: 0x%X\n"),
						desc.Width, desc.Height, desc.ArraySize, desc.MiscFlags);
					OutputDebugString(debugMsg);
					
					// キューブマップかどうか確認する（ArraySize == 6 かつ TEXTURECUBE フラグを持つ）
					if (desc.ArraySize == 6 && (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE))
					{
						OutputDebugString(_T("CSkybox: Cubemap loaded successfully!\n"));
					}
					else if (desc.ArraySize == 6)
					{
						OutputDebugString(_T("CSkybox: Loaded as texture array with 6 faces (treating as cubemap)\n"));
					}
					else
					{
						OutputDebugString(_T("CSkybox: Warning - texture is not a cubemap format! Skybox may not render correctly.\n"));
					}
					
					pTex2D->Release();
				}
			}
			pResource->Release();
		}
		
		return S_OK;
	}

	// 2回目の試行：キューブマップとして強制的に読み込む
	D3DX11_IMAGE_LOAD_INFO loadInfo = {};
	loadInfo.Width = D3DX11_DEFAULT;
	loadInfo.Height = D3DX11_DEFAULT;
	loadInfo.Depth = D3DX11_DEFAULT;
	loadInfo.FirstMipLevel = 0;
	loadInfo.MipLevels = D3DX11_DEFAULT;
	loadInfo.Usage = D3D11_USAGE_DEFAULT;
	loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	loadInfo.CpuAccessFlags = 0;
	loadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	loadInfo.Format = DXGI_FORMAT_FROM_FILE;
	loadInfo.Filter = D3DX11_FILTER_LINEAR;
	loadInfo.MipFilter = D3DX11_FILTER_LINEAR;
	loadInfo.pSrcInfo = nullptr;

	hr = D3DX11CreateShaderResourceViewFromFile(
		m_pDevice11,
		lpCubemapFile,
		&loadInfo,
		nullptr,
		&m_pCubemapSRV,
		nullptr);

	if (SUCCEEDED(hr))
	{
		OutputDebugString(_T("CSkybox: Cubemap loaded with forced cubemap flag!\n"));
		return S_OK;
	}

	TCHAR errMsg[256];
	_stprintf_s(errMsg, _T("CSkybox: Failed to load cubemap. HRESULT: 0x%08X\n"), hr);
	OutputDebugString(errMsg);
	return E_FAIL;
}

void CSkybox::Render(const D3DXMATRIX& mView, const D3DXMATRIX& mProj, const D3DXVECTOR3& camPos)
{
	if (!m_initialized ||
		!m_pContext11 || !m_pConstantBuffer || !m_pVertexBuffer || !m_pIndexBuffer ||
		!m_pInputLayout || !m_pSamplerState || !m_pDepthStencilState || !m_pRasterizerState ||
		!m_pVertexShader || !m_pPixelShader || !m_pCubemapSRV)
	{
		return;
	}

	// 現在のステートを保存する
	ID3D11DepthStencilState* prevDSState = nullptr;
	UINT prevStencilRef = 0;
	m_pContext11->OMGetDepthStencilState(&prevDSState, &prevStencilRef);

	ID3D11RasterizerState* prevRSState = nullptr;
	m_pContext11->RSGetState(&prevRSState);

	// 平行移動成分を除いたビュー行列を作成する（スカイボックスはカメラ位置に追従する）
	D3DXMATRIX mViewNoTranslation = mView;
	mViewNoTranslation._41 = 0.0f;
	mViewNoTranslation._42 = 0.0f;
	mViewNoTranslation._43 = 0.0f;

	// スカイボックスを十分大きくスケーリングする
	D3DXMATRIX mWorld;
	D3DXMatrixScaling(&mWorld, 500.0f, 500.0f, 500.0f);

	// WVP行列を計算する
	D3DXMATRIX mWVP = mWorld * mViewNoTranslation * mProj;
	D3DXMatrixTranspose(&mWVP, &mWVP);

	// 定数バッファを更新する
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	if (SUCCEEDED(m_pContext11->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		CBUFFER_SKYBOX* cb = reinterpret_cast<CBUFFER_SKYBOX*>(mapped.pData);
		cb->mWVP = mWVP;
		m_pContext11->Unmap(m_pConstantBuffer, 0);
	}

	// パイプラインステートを設定する
	m_pContext11->OMSetDepthStencilState(m_pDepthStencilState, 0);
	m_pContext11->RSSetState(m_pRasterizerState);

	// シェーダーを設定する
	m_pContext11->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext11->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pContext11->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext11->PSSetShaderResources(0, 1, &m_pCubemapSRV);
	m_pContext11->PSSetSamplers(0, 1, &m_pSamplerState);

	// 頂点／インデックスバッファを設定する
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pContext11->IASetInputLayout(m_pInputLayout);
	m_pContext11->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pContext11->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// スカイボックスを描画する
	m_pContext11->DrawIndexed(36, 0, 0);

	// 直前のステートを復元する
	m_pContext11->OMSetDepthStencilState(prevDSState, prevStencilRef);
	m_pContext11->RSSetState(prevRSState);

	if (prevDSState) prevDSState->Release();
	if (prevRSState) prevRSState->Release();
}
