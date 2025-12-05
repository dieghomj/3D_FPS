cbuffer CBUFFER_VP : register(b0)
{
    float4x4 g_mWVP; // View * Proj (transposed)
};

struct VS_IN
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

struct VS_OUT
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

VS_OUT VS_Main(VS_IN vin)
{
    VS_OUT vout;
    vout.PosH = mul(float4(vin.Pos, 1.0f), g_mWVP);
    vout.Color = vin.Color;
    return vout;
}

float4 PS_Main(VS_OUT pin) : SV_TARGET
{
    return pin.Color;
}