/*********************************************************************
*	Skybox shader for rendering a cubemap skybox.
**/

// Cubemap texture and sampler
TextureCube g_CubeMap : register(t0);
SamplerState g_Sampler : register(s0);

// Constant buffer
cbuffer cbSkybox : register(b0)
{
    matrix g_mWVP;
};

// Vertex shader input
struct VS_INPUT
{
    float3 Pos : POSITION;
};

// Pixel shader input
struct PS_INPUT
{
    float4 Pos : SV_Position;
    float3 TexCoord : TEXCOORD0;
};

//-------------------------------------------------
//	Vertex Shader
//-------------------------------------------------
PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    // Transform position
    output.Pos = mul(float4(input.Pos, 1.0f), g_mWVP);
    
    // Set z = w so the skybox is always at the far plane
    output.Pos.z = output.Pos.w;
    
    // Use position as texture coordinate for cubemap lookup
    output.TexCoord = input.Pos;
    
    return output;
}

//-------------------------------------------------
//	Pixel Shader
//-------------------------------------------------
float4 PS_Main(PS_INPUT input) : SV_Target
{
    // Sample the cubemap using the interpolated position as direction
    float4 color = g_CubeMap.Sample(g_Sampler, input.TexCoord);
    
    return color;
}
