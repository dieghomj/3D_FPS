/*********************************************************************
*  MSDF Font Shader
*********************************************************************/

Texture2D g_Texture : register(t0);
SamplerState g_samLinear : register(s0);

cbuffer per_mesh : register(b0)
{
    matrix g_mWorld : packoffset(c0);
    float4 g_Color : packoffset(c4);
    float g_ViewPortW : packoffset(c5);
    float g_ViewPortH : packoffset(c6);
    float g_PxRange : packoffset(c7); // 例: 4
};

struct VS_OUTPUT
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD0;
};

VS_OUTPUT VS_Main(float4 Pos : POSITION, float2 UV : TEXCOORD)
{
    VS_OUTPUT o = (VS_OUTPUT) 0;
    o.Pos = mul(Pos, g_mWorld);
    o.Pos.x = (o.Pos.x / g_ViewPortW) * 2.0f - 1.0f;
    o.Pos.y = 1.0f - (o.Pos.y / g_ViewPortH) * 2.0f;
    o.UV = UV;
    return o;
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float4 PS_Main(VS_OUTPUT i) : SV_Target
{
    float3 msd = g_Texture.Sample(g_samLinear, i.UV).rgb;
    float sd = median(msd.r, msd.g, msd.b); // 0.5が輪郭

    // 標準的なMSDFのAA
    float d = sd - 0.5;
    float w = fwidth(sd); // 画面上の距離勾配
    float alpha = saturate(d / w + 0.5); // スムーズステップ相当

    return float4(g_Color.rgb, g_Color.a * alpha);
}

// 単チャネルSDF用
float4 PS_Main_SDF(VS_OUTPUT i) : SV_Target
{
    float dist = g_Texture.Sample(g_samLinear, i.UV).r;
    float w = fwidth(dist);
    float alpha = saturate((dist - 0.5) / w + 0.5);
    return float4(g_Color.rgb, g_Color.a * alpha);
}