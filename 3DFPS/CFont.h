#pragma once

#pragma warning(disable:4005)

#define ALIGN16 _declspec( align(16) )

#include <unordered_map>

// Forward declaration
class CDirectX11;

/**************************************************
*	SDF Font Renderer Class
*	Renders high-quality scalable fonts using Signed Distance Field technique
**/
class CFont
{
public:

	enum class FontMode { SDF, MSDF };

	struct GlyphInfo {
		int unicode;
		float uvLeft, uvTop, uvRight, uvBottom;
		float advance;
	};

	std::unordered_map<int, GlyphInfo> m_GlyphMap; // ASCII code -> glyph info
	
	// 定数定義.
	static constexpr int SPRITE_MAX = 95;		// 表示可能文字数 (ASCII 32-126).

	// シェーダー用定数バッファ.
	struct SHADER_CONSTANT_BUFFER
	{
		ALIGN16 D3DXMATRIX	mWorld;
		ALIGN16 D3DXVECTOR4	vColor;
		ALIGN16 float fViewPortWidth;
		ALIGN16 float fViewPortHeight;
		ALIGN16 float fPxRange;
		ALIGN16 float fPadding[3];
	};

	// 頂点フォーマット.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	// Vertex position
		D3DXVECTOR2	Tex;	// Texture coordinates
	};

public:
	CFont();		// Constructor
	~CFont();	// Destructor

	// Initialize the SDF text renderer
	HRESULT Init(CDirectX11& pDx11);

	// Render text string
	void Render(LPCTSTR text, int x, int y, float FontSize, bool vertical = false);

	void SetFontMode(FontMode mode){ m_FontMode = mode; }

	// Set alpha value (0.0 = fully transparent, 1.0 = fully opaque)
	void SetAlpha(float alpha) { m_Alpha = alpha; }

	// Set text color (RGB)
	void SetColor(float r, float g, float b)
	{
		m_Color.x = r;
		m_Color.y = g;
		m_Color.z = b;
	}

	void SetPxRange(float pxRange) { m_PxRange = pxRange; }

private:
	// Create shaders
	HRESULT CreateShader();

	// Create geometry (quads for each glyph)
	HRESULT CreateModel();

	// Load SDF texture atlas
	HRESULT CreateTexture(LPCTSTR lpFileName);

	// Create sampler state (CLAMP mode for SDF)
	HRESULT CreateSampler();

	// Render single glyph
	void RenderFont(int FontIndex, float x, float y, float FontSize);

	// Release resources
	void Release();

	// Load glyph metrics from JSON file
	HRESULT LoadAtlasCSV(LPCTSTR filePath);

private:
	CDirectX11* m_pDx11;
	ID3D11Device* m_pDevice11;
	ID3D11DeviceContext* m_pContext11;

	ID3D11VertexShader* m_pVertexShader;		// Vertex shader
	ID3D11InputLayout* m_pVertexLayout;			// Input layout
	ID3D11PixelShader* m_pPixelShader;			// Pixel shader (SDF-aware)
	ID3D11Buffer* m_pConstantBuffer;			// Constant buffer

	ID3D11Buffer* m_pVertexBuffer[SPRITE_MAX];	// Vertex buffers (one per glyph)

	ID3D11ShaderResourceView* m_pTexture;		// SDF texture atlas
	ID3D11SamplerState* m_pSampleLinear;		// Sampler state

	float			m_Alpha;					// Alpha value (0-1)
	D3DXVECTOR3		m_Color;					// Color (RGB)
	float			m_Kerning[SPRITE_MAX];		// Kerning values per glyph

	float			m_PxRange;
	FontMode		m_FontMode;
	GlyphInfo		m_GlyphInfo[SPRITE_MAX];

};