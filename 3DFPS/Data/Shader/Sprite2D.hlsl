/*********************************************************************
*	スプライト2Dクラス用シェーダファイル.
**/
//グローバル変数.
//テクスチャは レジスタ t(n).
Texture2D		g_Texture	: register( t0 );
//サンプラは レジスタ s(n).
SamplerState	g_samLinear	: register( s0 );

//コンスタントバッファ.
//アプリ側と同じバッファサイズになっている必要がある.
cbuffer per_mesh : register( b0 )	//レジスタ番号.
{
	matrix	g_mWorld	: packoffset(c0);	//ワールド行列.
	float4	g_Color		: packoffset(c4);	//色（RGBA:xyzw）.
	float4	g_UV		: packoffset(c5);	//UV座標（xyしか使わない）.
	float	g_ViewPortW	: packoffset(c6);	//ビューポート幅
	float	g_ViewPortH	: packoffset(c7);	//ビューポート高さ
};

//頂点シェーダの出力パラメータ.
//頂点シェーダで制御した複数の値をピクセルシェーダ側に渡すために用意している.
struct VS_OUTPUT
{
	float4	Pos		: SV_Position;	//座標（SV_:System-Value Semantics）.
	float2	UV		: TEXCOORD0;	//UV座標.
};

//頂点シェーダ.
//主にモデルの頂点の表示位置を決定する.
VS_OUTPUT VS_Main(
	float4 Pos	: POSITION,
	float2 UV	: TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul( Pos, g_mWorld );
	
	//スクリーン座標に合わせる計算
    output.Pos.x = (output.Pos.x / g_ViewPortW) * 2.f - 1.f;
    output.Pos.y = 1.f - (output.Pos.y / g_ViewPortH) * 2.f;
	
	output.UV = UV;
	
	//UVスクロール(UV座標を操作する)
    output.UV.x += g_UV.x;
    output.UV.y += g_UV.y;

	return output;
}

//ピクセルシェーダ.
//画面上に評されるピクセル（ドット1つ分）の色を決定する.
float4 PS_Main( VS_OUTPUT input ) : SV_Target
{
	float4 color = g_Texture.Sample( g_samLinear, input.UV );//色を返す.

	//プログラム制御のα値をテクスチャが持っているα値にかけ合わせる.
	color.a *= g_Color.a;
	
	// fillPercent behavior:
	// g_UV.z = percent (0..1). g_UV.w >= 0.5 -> horizontal, else vertical.
    if (g_UV.z < 1.0f - 1e-6f) // if less than full
    {
        bool horizontal = (g_UV.w >= 0.5f);
        if (horizontal)
        {
			// if UV.x beyond offset + percent => hide
            if (input.UV.x > g_UV.x + g_UV.z)
            {
                color.a = 0.0f;
            }
        }
        else
        {
			// vertical fill: show from top (lower y) to percent
            if (input.UV.y > g_UV.y + g_UV.z)
            {
                color.a = 0.0f;
            }
        }
    }

	return color;
}