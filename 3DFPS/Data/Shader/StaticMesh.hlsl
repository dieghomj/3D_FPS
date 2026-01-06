/*********************************************************************
*	スタティックメッシュクラス用シェーダファイル.
**/

#define MAX_LIGHT 4

// Textures and Samplers
Texture2D g_Texture : register(t0);
SamplerState g_SamPoint : register(s0); // Use Point Sampler for PSX effect

struct SpotLightData
{
    float4 origin;
    float4 direction;
    float4 color;
    float range;
    float innerCos;
    float outerCos;
    float intensity;
};

// Constant Buffers
cbuffer per_mesh : register(b0)
{
    matrix g_mW; // World Matrix
    matrix g_mWVP; // World-View-Projection Matrix
};

cbuffer per_material : register(b1)
{
    float4 g_Diffuse; // Diffuse color
    float4 g_Ambient; // Ambient color
    float4 g_Specular; // Specular color
};

cbuffer per_frame : register(b2)
{
    float4 g_CameraPos; // Camera Position
    float4 g_LightColor; // Light Color
    float4 g_LightDir; // Light Direction
    float4 g_AmbientColor; // Ambient Color
    float4 g_FogColor; // Fog Color
    float4 g_FogParams; // Fog Parameters (start, end, density, unused)
    float g_LightIntensity; // Light Intensity
    float g_AffineIntensity; // PSX Affine Texture Mapping Intensity
    float g_VertexSnapping; // PSX Vertex Snapping Intensity
};

cbuffer per_spotlight : register(b3)
{
    SpotLightData g_SpotLights[MAX_LIGHT];
    uint g_NumSpotLights;
};

// ヘルパー: ガンマ補正（簡易 sRGB 変換）
static float3 ToSRGB(float3 col)
{
    return pow(saturate(col), 1.0 / 2.2);
}


float3 ApplySpotLights(float3 pos, float3 normal)
{
    float3 accumulatedLight = float3(0, 0, 0);
    
    [loop]
    for (uint i = 0; i < g_NumSpotLights; i++)
    {
        SpotLightData light = g_SpotLights[i];
        
        float3 lightDir = normalize(pos.xyz - light.origin.xyz);
        float NdotL = saturate(dot(normal, -lightDir));
        
        // Spotlight effect
        float spotEffect = dot(light.direction.xyz, lightDir);
        float spotFactor = smoothstep(light.outerCos, light.innerCos, spotEffect);
        
        // Attenuation
        float distance = length(pos.xyz - light.origin.xyz);
        float attenuation = saturate(1.0 - (distance / light.range));

        float3 lightContribution = light.color.rgb  * spotFactor * attenuation * light.intensity;
        accumulatedLight += lightContribution;
    }
    
    return accumulatedLight;
}

// Vertex Shader Input
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
};

// Pixel Shader Input
struct PS_INPUT
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};

//-------------------------------------------------
//	Vertex Shader (With Texture)
//-------------------------------------------------
PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    // Calculate world position
    output.WorldPos = mul(input.Pos, g_mW).xyz;
    
	// Projection
    output.Pos = mul(input.Pos, g_mWVP);

    // --- PSX Effects Start ---
    //output.Pos.xyw *= output.Pos.w;
    output.Pos.w *= g_AffineIntensity;

    if (g_VertexSnapping > 0.0f)
    {
        output.Pos.xy = floor(g_VertexSnapping * output.Pos.xy) / g_VertexSnapping;
    }
    // --- PSX Effects End ---
    
	// Pass data to pixel shader
    output.Normal = mul(input.Normal, (float3x3) g_mW);
    output.UV = input.UV;

    return output;
}


//-------------------------------------------------
//	Vertex Shader (No Texture)
//-------------------------------------------------
PS_INPUT VS_NoTex(float4 Pos : POSITION, float3 Norm : NORMAL)
{
    PS_INPUT output = (PS_INPUT) 0;

    output.Pos = mul(Pos, g_mWVP);

    // --- PSX Effects Start ---
    //output.Pos.xyw *= output.Pos.w;
    output.Pos.w *= g_AffineIntensity;

    if (g_VertexSnapping > 0.0f)
    {
        output.Pos.xy = floor(g_VertexSnapping * output.Pos.xy) / g_VertexSnapping;
    }
    // --- PSX Effects End ---
    
    output.Normal = mul(Norm, (float3x3) g_mW);
    output.UV = float2(0, 0); // No UV

    return output;
}

//-------------------------------------------------
//	Pixel Shader (With Texture)
//-------------------------------------------------
float4 PS_Main(PS_INPUT input) : SV_Target
{
	// Sample texture
    float4 texColor = g_Texture.Sample(g_SamPoint, input.UV);

	// Lighting
    float3 N = normalize(input.Normal);
    float3 L = normalize(g_LightDir.xyz);
    float NdotL = saturate(dot(N, -L));

	// Final Color Calculation
    float3 baseDiffuse = g_Diffuse.rgb * g_LightColor.rgb * NdotL;
    float3 baseAmbient = g_AmbientColor.rgb;
    float3 spotDiffuse = ApplySpotLights(input.WorldPos, N);
    float3 lit = (baseAmbient + baseDiffuse) * g_LightIntensity;
    lit += spotDiffuse; // spotlights usually are not scaled by g_LightIntensity
    float3 color = texColor.rgb * lit;
    
    float alpha = texColor.a;
    
    float dist = length(input.WorldPos.xyz - g_CameraPos.xyz);
    float fogFactor = 1.f;
    
    int mode = (int) g_FogParams.w;
    if (mode == 0) // linear
    {
        float start = g_FogParams.x;
        float end = max(g_FogParams.y, start + 0.001f);
        fogFactor = saturate((end - dist) / (end - start));
    }
    else if (mode == 1) // exp
    {
        float density = g_FogParams.z;
        fogFactor = exp(-density * dist);
    }
    else // exp2
    {
        float density = g_FogParams.z;
        fogFactor = exp(-density * density * dist * dist);
    }
    
    float3 finalColor = lerp(g_FogColor.rgb, color, fogFactor);
    return float4(finalColor, alpha);
}

//-------------------------------------------------
//	Pixel Shader (No Texture)
//-------------------------------------------------
float4 PS_NoTex(PS_INPUT input) : SV_Target
{
    
    
    
    // Lighting
    float3 N = normalize(input.Normal);
    float3 L = normalize(g_LightDir.xyz);
    float NdotL = saturate(dot(N, -L));

    
    
    // Final Color Calculation
    float3 ambient = g_AmbientColor.rgb;
    float3 specular = g_Specular.rgb * pow(saturate(dot(reflect(L, N), normalize(g_CameraPos.xyz - input.WorldPos.xyz))), 16.0f);
    float3 diffuse = g_Diffuse.rgb * g_LightColor.rgb * NdotL;
    float3 finalColor = (ambient + diffuse + specular) * g_LightIntensity;
    
    

    return float4(finalColor, g_Diffuse.a);
}