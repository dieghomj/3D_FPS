/***************************************************************************************************
*	SkinMeshCode Version 2.50
*	LastUpdate	: 2025/06/23.
**/
//------------------------------------------------.
//	定数.
//------------------------------------------------.
static const int MAX_BONE_MATRICES = 255;

//------------------------------------------------.
//	グローバル変数.
//------------------------------------------------.
Texture2D g_Texture : register(t0); //テクスチャーは、レジスターt(n).
SamplerState g_Sampler : register(s0); //サンプラーは、レジスターs(n).

//------------------------------------------------.
//	コンスタントバッファ.
//------------------------------------------------.
//メッシュ.
cbuffer per_mesh : register(b0)
{
    matrix g_mW; //ワールド行列.
    matrix g_mWVP; //ワールド,ビュー,プロジェクションの合成行列.
};
//マテリアル.
cbuffer per_material : register(b1)
{
    float4 g_Ambient; //アンビエント色(環境色).
    float4 g_Diffuse; //ディフューズ色(拡散反射色).
    float4 g_Specular; //スペキュラ色(鏡面反射色).
};
//フレーム.
cbuffer per_frame : register(b2)
{
    float4 g_CameraPos; //カメラ位置(始点位置).
    float4 g_LightColor; // ライトの色.
    float4 g_LightDir; //ライトの方向ベクトル.
    float4 g_AmbientColor; // 環境光の色.
    float4 g_FogColor; //フォグの色.
    float4 g_FogParams; //フォグのパラメータ(x=開始距離,y=終了距離,z=密度,w=モード).
    float g_AffineIntensity; // PSX アフィン テクスチャ マッピング強度.
    float g_VertexSnapping; // PSX 頂点スナッピング強度.
};
//ボーンのポーズ行列が入る.
cbuffer per_bones : register(b3)
{
    float4x4 g_mConstBoneWorld[MAX_BONE_MATRICES];
};

//スキニング後の頂点・法線が入る.
struct Skin
{
    float4 Pos;
    float4 Norm;
};
//バーテックスバッファーの入力.
struct VSSkinIn
{
    float3 Pos : POSITION; //位置.  
    float3 Norm : NORMAL; //頂点法線.
    float2 UV : TEXCOORD; //UV座標（テクスチャー座標）.
    uint4 Bones : BONE_INDEX; //ボーンのインデックス.
    float4 Weights : BONE_WEIGHT; //ボーンの重み.
};

//ピクセルシェーダーの入力（バーテックスバッファーの出力）　
struct PSSkinIn
{
    float4 Pos : SV_Position; //位置.
    float3 Norm : TEXCOORD0; //頂点法線.
    float2 UV : TEXCOORD1; //UV座標（テクスチャー座標）.
    float3 WorldPos : TEXCOORD2; //ワールド座標.
};


//指定した番号のボーンのポーズ行列を返す.
//	サブ関数（バーテックスシェーダーで使用）.
matrix FetchBoneMatrix(uint Bone)
{
    return g_mConstBoneWorld[Bone];
}


//頂点をスキニング（ボーンにより移動）する.
//	サブ関数（バーテックスシェーダーで使用）.
Skin SkinVert(VSSkinIn Input)
{
    Skin Output = (Skin) 0;

    float4 Pos = float4(Input.Pos, 1.f); //座標のwは 1.
    float4 Norm = float4(Input.Norm, 0.f); //ベクトルのwは 0.
	//ボーン0.
    uint Bone = Input.Bones.x;
    float Weight = Input.Weights.x;
    float4x4 m = FetchBoneMatrix(Bone);
    Output.Pos += Weight * mul(Pos, m);
    Output.Norm += Weight * mul(Norm, m);
	
	//ボーン1.
    Bone = Input.Bones.y;
    Weight = Input.Weights.y;
    m = FetchBoneMatrix(Bone);
    Output.Pos += Weight * mul(Pos, m);
    Output.Norm += Weight * mul(Norm, m);
	
	//ボーン2.
    Bone = Input.Bones.z;
    Weight = Input.Weights.z;
    m = FetchBoneMatrix(Bone);
    Output.Pos += Weight * mul(Pos, m);
    Output.Norm += Weight * mul(Norm, m);
	
	//ボーン3.
    Bone = Input.Bones.w;
    Weight = Input.Weights.w;
    m = FetchBoneMatrix(Bone);
    Output.Pos += Weight * mul(Pos, m);
    Output.Norm += Weight * mul(Norm, m);

    return Output;
}

// バーテックスシェーダ.
PSSkinIn VS_Main(VSSkinIn input)
{
    PSSkinIn output = (PSSkinIn) 0;
	
    Skin vSkinned = SkinVert(input);

    // ワールド位置を計算
    output.WorldPos = mul(vSkinned.Pos, g_mW).xyz;

	//プロジェクション変換（ワールド、ビュー、プロジェクション）.
    output.Pos = mul(vSkinned.Pos, g_mWVP);
	
    // --- PSX エフェクト開始 ---
    output.Pos.w *= g_AffineIntensity;

    if (g_VertexSnapping > 0.0f)
    {
        output.Pos.xy = floor(g_VertexSnapping * output.Pos.xy) / g_VertexSnapping;
    }
    // --- PSX エフェクト終了 ---

	//法線をモデルの姿勢に合わせる.
	// (モデルが回転すれば法線も回転させる必要があるため).
    output.Norm = normalize((float3) mul(vSkinned.Norm, g_mW));
	
	//UV座標（テクスチャ座標）.	
    output.UV = input.UV;

    return output;
}

// ピクセルシェーダ.
float4 PS_Main(PSSkinIn input) : SV_Target
{
	//テクスチャからピクセル色を取り出す.
    float4 texColor = g_Texture.Sample(g_Sampler, input.UV);

	// ライティング
    float3 N = normalize(input.Norm);
    float3 L = normalize(g_LightDir.xyz);
    float NdotL = saturate(dot(N, -L));

	// 最終的な色の計算
    float3 diffuse = g_Diffuse.rgb * texColor.rgb * g_LightColor.rgb * NdotL;
    float3 ambient = g_AmbientColor.rgb * texColor.rgb;
    float3 color = ambient + diffuse;
    
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