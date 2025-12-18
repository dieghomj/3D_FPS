/*********************************************************************
*	デカール（弾痕など）用シェーダファイル.
**/
//グローバル変数.
//テクスチャは レジスタ t(n).
Texture2D g_Texture : register(t0);
//サンプラは レジスタ s(n).
SamplerState g_samLinear : register(s0);

//コンスタントバッファ.
//アプリ側と同じバッファサイズになっている必要がある.
cbuffer per_mesh : register(b0) //レジスタ番号.
{
    matrix g_mWVP; //ワールド,ビュー,プロジェクションの変換合成行列.
    float4 g_Color; //色（RGBA:xyzw）.
    float4 g_UV; //UV座標（xyしか使わない）.
    float g_FadeDistance; //フェード距離（オプション）.
    float3 g_Padding; //パディング（16バイトアライメント用）.
};

//頂点シェーダの出力パラメータ.
struct VS_OUTPUT
{
    float4 Pos : SV_Position; //座標.
    float2 UV : TEXCOORD0; //UV座標.
    float3 WorldPos : TEXCOORD1; //ワールド座標（オプション：距離フェード用）.
};

//頂点シェーダ.
VS_OUTPUT VS_Main(
	float4 Pos : POSITION,
	float2 UV : TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = mul(Pos, g_mWVP);
    output.UV = UV;
	
	//UVスクロール(UV座標を操作する)
    output.UV.x += g_UV.x;
    output.UV.y += g_UV.y;
	
	// ワールド座標を保存（距離フェード用）
    output.WorldPos = Pos.xyz;

    return output;
}

//ピクセルシェーダ.
float4 PS_Main(VS_OUTPUT input) : SV_Target
{
	// テクスチャからカラーをサンプリング
    float4 color = g_Texture.Sample(g_samLinear, input.UV);
	
	// エッジのソフトフェード処理
	// UV座標が中心から離れるほど透明度を下げる
    float2 centered = input.UV * 2.0 - 1.0; // 0-1 を -1 to 1 に変換
    float edgeDistance = length(centered);
    float edgeFade = saturate(1.0 - edgeDistance);
    edgeFade = pow(edgeFade, 2.0); // より滑らかなフェード
	
	// プログラム制御のα値とエッジフェードを適用
    color.a *= g_Color.a * edgeFade;
	
	// RGB値にもカラーを適用（色調整用）
    color.rgb *= g_Color.rgb;
	
	// 完全に透明なピクセルは破棄（最適化）
    clip(color.a - 0.01);
	
    return color;
}

//======================================================================
// 高品質版ピクセルシェーダ（オプション）
// より滑らかなブレンドと距離フェードをサポート
//======================================================================
float4 PS_HighQuality(VS_OUTPUT input) : SV_Target
{
	// テクスチャからカラーをサンプリング
    float4 color = g_Texture.Sample(g_samLinear, input.UV);
	
	// 円形のソフトフェード
    float2 centered = input.UV * 2.0 - 1.0;
    float edgeDistance = length(centered);
	
	// より複雑なフェードカーブ
    float edgeFade = saturate(1.0 - edgeDistance);
    edgeFade = smoothstep(0.0, 1.0, edgeFade);
    edgeFade = pow(edgeFade, 1.5);
	
	// ディザリング効果（オプション：より自然な見た目）
    float dither = frac(sin(dot(input.Pos.xy, float2(12.9898, 78.233))) * 43758.5453);
    edgeFade = saturate(edgeFade + (dither - 0.5) * 0.1);
	
	// 距離フェード（遠くのデカールを薄くする）
    if (g_FadeDistance > 0.0)
    {
        float distance = length(input.WorldPos);
        float distanceFade = saturate(1.0 - (distance / g_FadeDistance));
        edgeFade *= distanceFade;
    }
	
	// 最終的なアルファ値を適用
    color.a *= g_Color.a * edgeFade;
    color.rgb *= g_Color.rgb;
	
	// 完全に透明なピクセルは破棄
    clip(color.a - 0.01);
	
    return color;
}