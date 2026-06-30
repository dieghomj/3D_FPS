#pragma once

#pragma warning(disable:4005)

#define ALIGN16 _declspec( align(16) )

#include <unordered_map>

// 前方宣言.
class CDirectX11;

/**************************************************
*	SDFフォント描画クラス.
*	符号付き距離フィールド（SDF）技術を用いて高品質でスケーラブルなフォントを描画する.
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

	std::unordered_map<int, GlyphInfo> m_GlyphMap; // ASCIIコード -> グリフ情報.
	
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
		D3DXVECTOR3 Pos;	// 頂点座標.
		D3DXVECTOR2	Tex;	// テクスチャ座標.
	};

public:
	CFont();		// コンストラクタ.
	~CFont();	// デストラクタ.

	// SDFテキスト描画を初期化.
	HRESULT Init(CDirectX11& pDx11);

	// テキスト文字列を描画.
	void Render(LPCTSTR text, int x, int y, float FontSize, bool vertical = false);

	void SetFontMode(FontMode mode){ m_FontMode = mode; }

	// アルファ値を設定 (0.0 = 完全透明, 1.0 = 完全不透明).
	void SetAlpha(float alpha) { m_Alpha = alpha; }

	// テキストカラーを設定 (RGB).
	void SetColor(float r, float g, float b)
	{
		m_Color.x = r;
		m_Color.y = g;
		m_Color.z = b;
	}

	void SetPxRange(float pxRange) { m_PxRange = pxRange; }

private:
	// シェーダ作成.
	HRESULT CreateShader();

	// ジオメトリ作成（グリフごとのクアッド）.
	HRESULT CreateModel();

	// SDFテクスチャアトラス読み込み.
	HRESULT CreateTexture(LPCTSTR lpFileName);

	// サンプラーステート作成（SDF用CLAMPモード）.
	HRESULT CreateSampler();

	// 単一グリフを描画.
	void RenderFont(int FontIndex, float x, float y, float FontSize);

	// リソースを解放.
	void Release();

	// JSONファイルからグリフメトリクスを読み込む.
	HRESULT LoadAtlasCSV(LPCTSTR filePath);

private:
	CDirectX11* m_pDx11;
	ID3D11Device* m_pDevice11;
	ID3D11DeviceContext* m_pContext11;

	ID3D11VertexShader* m_pVertexShader;		// 頂点シェーダ.
	ID3D11InputLayout* m_pVertexLayout;			// 入力レイアウト.
	ID3D11PixelShader* m_pPixelShader;			// ピクセルシェーダ（SDF対応）.
	ID3D11Buffer* m_pConstantBuffer;			// コンスタントバッファ.

	ID3D11Buffer* m_pVertexBuffer[SPRITE_MAX];	// 頂点バッファ（グリフごとに1つ）.

	ID3D11ShaderResourceView* m_pTexture;		// SDFテクスチャアトラス.
	ID3D11SamplerState* m_pSampleLinear;		// サンプラーステート.

	float			m_Alpha;					// アルファ値 (0-1).
	D3DXVECTOR3		m_Color;					// カラー (RGB).
	float			m_Kerning[SPRITE_MAX];		// グリフごとのカーニング値.

	float			m_PxRange;
	FontMode		m_FontMode;
	GlyphInfo		m_GlyphInfo[SPRITE_MAX];

};