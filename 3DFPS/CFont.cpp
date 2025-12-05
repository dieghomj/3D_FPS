#include "CFont.h"
#include "CDirectX11.h"
#include <stdio.h>

const TCHAR SHADER_NAME[] = _T("Data\\Shader\\Text.hlsl");

CFont::CFont()
	: m_pDx11(nullptr)
	, m_pDevice11(nullptr)
	, m_pContext11(nullptr)
	, m_pVertexShader(nullptr)
	, m_pVertexLayout(nullptr)
	, m_pPixelShader(nullptr)
	, m_pConstantBuffer(nullptr)
	, m_pVertexBuffer()
	, m_pSampleLinear(nullptr)
	, m_pTexture(nullptr)
	, m_Alpha(1.0f)
	, m_Color(1.f, 1.f, 1.f)
	, m_Kerning()
	, m_PxRange(4.0f)
	, m_FontMode(FontMode::MSDF)
	, m_GlyphInfo()
{
}

CFont::~CFont()
{
	Release();

	// These are managed elsewhere, just nullify
	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
	m_pDx11 = nullptr;
}

// 初期化関数.
HRESULT CFont::Init(CDirectX11& pDx11)
{
	m_pDx11 = &pDx11;
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	// Create shaders
	if (FAILED(CreateShader()))
	{
		return E_FAIL;
	}

	// Create geometry
	if (FAILED(CreateModel()))
	{
		return E_FAIL;
	}

	// Load SDF texture atlas
	if (FAILED(CreateTexture(_T("Data\\Font\\ascii_msdf.png"))))
	{
		return E_FAIL;
	}

	// Create sampler
	if (FAILED(CreateSampler()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//解放関数.
void CFont::Release()
{
	SAFE_RELEASE(m_pSampleLinear);
	SAFE_RELEASE(m_pTexture);
	for (int i = SPRITE_MAX - 1; i >= 0; i--) {
		SAFE_RELEASE(m_pVertexBuffer[i]);
	}
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
}

// シェーダ作成.
HRESULT CFont::CreateShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	//シェーダーのコンパイル.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,
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
		_ASSERT_EXPR(false, _T("SDFhlsl読み込み失敗"));
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
		_ASSERT_EXPR(false, _T("SDFバーテックスシェーダ作成失敗"));
		return E_FAIL;
	}

	// Define input layout
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
		_ASSERT_EXPR(false, _T("頂点インプットレイアウト作成失敗"));
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	// Compile pixel shader (SDF-aware)
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,
			nullptr,
			nullptr,
			(m_FontMode == FontMode::SDF ? "PS_Main_SDF" : "PS_Main"),
			"ps_5_0",
			uCompileFlag,
			0,
			nullptr,
			&pCompiledShader,
			&pErrors,
			nullptr)))
	{
		_ASSERT_EXPR(false, _T("SDF hlsl読み込み失敗"));
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
		_ASSERT_EXPR(false, _T("SDFピクセルシェーダ作成失敗"));
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
		_ASSERT_EXPR(false, _T("コンスタントバッファ作成失敗"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFont::CreateTexture(LPCTSTR lpFileName)
{
	if (FAILED(D3DX11CreateShaderResourceViewFromFile(
		m_pDevice11,
		lpFileName,
		nullptr, nullptr,
		&m_pTexture,
		nullptr)))
	{
		_ASSERT_EXPR(false, _T("SDFテクスチャー読み込み失敗"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFont::CreateModel()
{
	constexpr float ATLAS_W = 320.f;   // -dimensions 320 320
	constexpr float ATLAS_H = 320.f;
	constexpr float BASE_SIZE = 32.f;  // -size 32

	if (FAILED(LoadAtlasCSV(_T("Data\\Font\\atlas.csv"))))
	{
		return E_FAIL;
	}

	// 事前クリア
	for (int i = 0; i < SPRITE_MAX; ++i) {
		m_pVertexBuffer[i] = nullptr;
		m_Kerning[i] = 0.0f;
	}

	int count = 0;

	for (int code = 32; code <= 126; ++code)
	{
		auto it = m_GlyphMap.find(code);
		if (it == m_GlyphMap.end()) {
			continue;
		}
		const GlyphInfo& glyph = it->second;

		// UVサイズからテクスチャ上のピクセル寸法を計算
		const float glyphWidthPx = (glyph.uvRight - glyph.uvLeft) * ATLAS_W;
		const float glyphHeightPx = (glyph.uvBottom - glyph.uvTop) * ATLAS_H;

		// 文字送り = advance[em] * BASE_SIZE[px]
		const int bufIndex = code - 32;
		m_Kerning[bufIndex] = glyph.advance * BASE_SIZE;

		// グリフ実寸の矩形で頂点を作成（描画時にFontSize/BASE_SIZEでスケール）
		VERTEX vertices[] =
		{
			{ D3DXVECTOR3(0.0f,         glyphHeightPx, 0.0f), D3DXVECTOR2(glyph.uvLeft,  glyph.uvBottom) }, // BL
			{ D3DXVECTOR3(0.0f,         0.0f,          0.0f), D3DXVECTOR2(glyph.uvLeft,  glyph.uvTop)    }, // TL
			{ D3DXVECTOR3(glyphWidthPx, glyphHeightPx, 0.0f), D3DXVECTOR2(glyph.uvRight, glyph.uvBottom) }, // BR
			{ D3DXVECTOR3(glyphWidthPx, 0.0f,          0.0f), D3DXVECTOR2(glyph.uvRight, glyph.uvTop)    }, // TR
		};
		const UINT vcount = _countof(vertices);

		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX) * vcount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;

		if (FAILED(m_pDevice11->CreateBuffer(&bd, &InitData, &m_pVertexBuffer[count])))
		{
			_ASSERT_EXPR(false, _T("頂点バッファ作成失敗"));
			return E_FAIL;
		}

		count++;
	}

	return S_OK;
}

HRESULT CFont::CreateSampler()
{
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// Linear filtering
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;		// CLAMP to avoid wrapping
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.MipLODBias = 0.0f;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MinLOD = 0;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(m_pDevice11->CreateSamplerState(
		&samDesc, &m_pSampleLinear)))
	{
		_ASSERT_EXPR(false, _T("Sampler creation failed"));
		return E_FAIL;
	}

	return S_OK;
}

void CFont::RenderFont(int FontIndex, float x, float y, float FontSize)
{
	D3DXMATRIX	mWorld;
	D3DXMATRIX	mTrans, mScale;

	constexpr float BASE_SIZE = 32.0f;
	float scale = FontSize / BASE_SIZE;

	// Build world matrix
	D3DXMatrixScaling(&mScale, scale, scale, 1.0);
	D3DXMatrixTranslation(&mTrans, x, y, 0.f);
	mWorld = mScale * mTrans;

	// Update constant buffer
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cb;
	ZeroMemory(&cb, sizeof(cb));  // Initialize to prevent garbage values

	if (SUCCEEDED(
		m_pContext11->Map(m_pConstantBuffer,
			0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		D3DXMATRIX m = mWorld;
		D3DXMatrixTranspose(&m, &m);
		cb.mWorld = m;

		cb.vColor = D3DXVECTOR4(
			m_Color.x, m_Color.y, m_Color.z, m_Alpha);

		cb.fViewPortWidth = static_cast<float>(WND_W);
		cb.fViewPortHeight = static_cast<float>(WND_H);
		cb.fPxRange = m_PxRange;

		memcpy_s(pData.pData, pData.RowPitch,
			(void*)(&cb), sizeof(cb));

		m_pContext11->Unmap(m_pConstantBuffer, 0);
	}

	// Set vertex buffer
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers(0, 1,
		&m_pVertexBuffer[FontIndex], &stride, &offset);

	// Enable alpha blending
	m_pDx11->SetAlphaBlend(true);

	// Draw quad
	m_pContext11->Draw(4, 0);

	// Disable alpha blending
	m_pDx11->SetAlphaBlend(false);
}

// Render text string
void CFont::Render(LPCTSTR text, int x, int y, float FontSize, bool vertical)
{
	// Set shaders
	m_pContext11->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext11->PSSetShader(m_pPixelShader, nullptr, 0);

	// Set constant buffers
	m_pContext11->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext11->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// Set input layout
	m_pContext11->IASetInputLayout(m_pVertexLayout);

	// Set primitive topology
	m_pContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set texture and sampler
	m_pContext11->PSSetSamplers(0, 1, &m_pSampleLinear);
	m_pContext11->PSSetShaderResources(0, 1, &m_pTexture);

	float fx = static_cast<float>(x);
	float fy = static_cast<float>(y);

	constexpr float BASE_SIZE = 32.0f;
	float scale = FontSize / BASE_SIZE;

	// Render each character
	for (int i = 0; i < lstrlen(text); i++)
	{
		TCHAR font = text[i];
		int index = font - 32;	// ASCII offset

		// Bounds check
		if (index < 0 || index >= SPRITE_MAX)
			continue;

		// Render glyph
		RenderFont(index, fx, fy, FontSize);

		// Advance position
		if (vertical)
		{
			fy += scale * m_Kerning[index];
		}
		else
		{
			fx += scale * m_Kerning[index];
		}
	}
}



// In CFont.cpp, add a LoadAtlasCSV() method:
HRESULT CFont::LoadAtlasCSV(LPCTSTR filePath)
{

	const int atlasWidth = 320;  // Texture atlas width in pixels
	const int atlasHeight = 320; // Texture atlas height in pixels

	const int atlasLineCount = 95; // Number of lines in the CSV file

	//
	const char filename[] = "Data\\Font\\atlas.csv";
	FILE* pf;
	errno_t err = fopen_s(&pf, filename, "r");

	if (err != 0) {
		_ASSERT_EXPR(false, _T("CSVファイル読み込み失敗"));
		return E_FAIL;
	}

	char lineBuffer[256] = "";
	char delim[] = ",";
	char* ctx = nullptr;

	int successCount = 0;
	for (int line = 0; line < atlasLineCount; line++)
	{
		if (fgets(lineBuffer, 256, pf) == nullptr)
			break;

		char* pNext = strtok_s(lineBuffer, delim, &ctx);

		int column = 0;
		int unicode = 0;
		float values[10] = { 0 };

		// Parse all columns first
		while (pNext != nullptr && column < 10)
		{
			values[column] = static_cast<float>(atof(pNext));
			pNext = strtok_s(nullptr, delim, &ctx);
			column++;
		}
		// Extract unicode
		unicode = static_cast<int>(values[0]);

		GlyphInfo& glyph = m_GlyphMap[unicode];
		glyph.unicode	= unicode;
		glyph.advance	= values[1];

		float left		= values[6];
		float top		= values[7];
		float right		= values[8];
		float bottom	= values[9];

		glyph.uvLeft	= left / atlasWidth;
		glyph.uvTop		= top / atlasHeight;
		glyph.uvRight	= right / atlasWidth;
		glyph.uvBottom	= bottom / atlasHeight;
	}

	fclose(pf);

	return S_OK;
}