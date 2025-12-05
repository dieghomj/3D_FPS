//ｸﾞﾛｰﾊﾞﾙ変数.
//ﾃｸｽﾁｬは ﾚｼﾞｽﾀ t(n).
Texture2D		g_Texture	: register(t0);
//ｻﾝﾌﾟﾗは ﾚｼﾞｽﾀ s(n).
SamplerState	g_SamLinear	: register(s0);

//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.
//ﾒｯｼｭ単位.
cbuffer per_mesh	: register( b0 )
{
	matrix	g_mW;			//ﾜｰﾙﾄﾞ行列.
	matrix	g_mWVP;			//ﾜｰﾙﾄﾞ,ﾋﾞｭｰ,ﾌﾟﾛｼﾞｪｸｼｮﾝの合成行列.
};
//ﾏﾃﾘｱﾙ単位.
cbuffer per_material: register( b1 )
{
	float4	g_Ambient;		//ｱﾝﾋﾞｴﾝﾄ色(環境色).
	float4	g_Diffuse;		//ﾃﾞｨﾌｭｰｽﾞ色(拡散反射色).
	float4	g_Specular;		//ｽﾍﾟｷｭﾗ色(鏡面反射色).
};
//ﾌﾚｰﾑ単位.
cbuffer per_frame	: register( b2 )
{
	float4	g_CameraPos;	//ｶﾒﾗ位置(視点位置).
	float4	g_LightDir;		//ﾗｲﾄの方向ﾍﾞｸﾄﾙ.
};

//頂点ｼｪｰﾀﾞの出力ﾊﾟﾗﾒｰﾀ.
struct VS_OUTPUT
{
	float4	Pos			: SV_Position;
	float3	Normal		: TEXCOORD0;
	float2	UV			: TEXCOORD1;
	float3	Light		: TEXCOORD2;
	float3	EyeVector	: TEXCOORD3;
	float4	PosWorld	: TEXCOORD4;
	float4	Color		: COLOR;
};

//-------------------------------------------------
//	頂点(ﾊﾞｰﾃｯｸｽ)ｼｪｰﾀﾞ.
//-------------------------------------------------
VS_OUTPUT VS_Main(
	float4 Pos	: POSITION,
	float4 Norm	: NORMAL,
	float2 UV	: TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//ﾌﾟﾛｼﾞｪｸｼｮﾝ変換(ﾜｰﾙﾄﾞ→ﾋﾞｭｰ→ﾌﾟﾛｼﾞｪｸｼｮﾝ).
	output.Pos = mul( Pos, g_mWVP );

	//法線をﾓﾃﾞﾙの姿勢に合わせる.
	// (ﾓﾃﾞﾙが回転すれば法線も回転させる必要があるため).
	output.Normal = mul( Norm, ( float3x3 )g_mW );
	output.Normal = normalize( output.Normal );

	//ﾗｲﾄ方向:
	// ﾃﾞｨﾚｸｼｮﾅﾙﾗｲﾄは、どこでも同じ方向.位置は無関係.
	output.Light = normalize( g_LightDir );

	output.PosWorld = mul( Pos, g_mW );

	//視線ﾍﾞｸﾄﾙ:
	// ﾜｰﾙﾄﾞ空間上での頂点から頂点へ向かうﾍﾞｸﾄﾙ.
	output.EyeVector = normalize( g_CameraPos - output.PosWorld );

	//ﾃｸｽﾁｬ座標.
	output.UV = UV;

	return output;
}

//*************************************************
//	ﾗｽﾀﾗｲｻﾞ(ｱﾌﾟﾘ側で設定する項目。ここでは触れない).
//*************************************************

//-------------------------------------------------
//	ﾋﾟｸｾﾙｼｪｰﾀﾞ.
//-------------------------------------------------
float4 PS_Main( VS_OUTPUT input ) : SV_Target
{
	//ﾃｸｽﾁｬｶﾗｰ.
	float4 texColor = g_Texture.Sample( g_SamLinear, input.UV );

	//環境光　①.
	float4 ambient = texColor * g_Ambient;

	//拡散反射光 ②.
	float NL = saturate( dot( input.Normal, input.Light ) );
	float4 diffuse = ( g_Diffuse / 2 + texColor / 2 )*NL;

	//鏡面反射光 ③.
	float3 reflect = normalize( 2 * NL * input.Normal - input.Light );
	float4 specular =
		pow( saturate( dot( reflect, input.EyeVector ) ), 4 )*g_Specular;

	//最終色　①②③の合計.
	float4 Color = ambient + diffuse + specular;
	return Color;
}

//========= ﾃｸｽﾁｬ無し用 ========================================
//-------------------------------------------------
//	頂点(ﾊﾞｰﾃｯｸｽ)ｼｪｰﾀﾞ.
//-------------------------------------------------
VS_OUTPUT VS_NoTex(
	float4 Pos : POSITION,
	float4 Norm: NORMAL)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//ﾌﾟﾛｼﾞｪｸｼｮﾝ変換(ﾜｰﾙﾄﾞ→ﾋﾞｭｰ→ﾌﾟﾛｼﾞｪｸｼｮﾝ).
	output.Pos = mul( Pos, g_mWVP );

	//法線をﾓﾃﾞﾙの姿勢に合わせる.
	// (ﾓﾃﾞﾙが回転すれば法線も回転させる必要があるため).
	output.Normal = mul( Norm, ( float3x3 )g_mW );
	output.Normal = normalize( output.Normal );

	//ﾗｲﾄ方向:
	// ﾃﾞｨﾚｸｼｮﾅﾙﾗｲﾄは、どこでも同じ方向.位置は無関係.
	output.Light = normalize( g_LightDir );

	output.PosWorld = mul( Pos, g_mW );

	//視線ﾍﾞｸﾄﾙ:
	// ﾜｰﾙﾄﾞ空間上での頂点から頂点へ向かうﾍﾞｸﾄﾙ.
	output.EyeVector = normalize( g_CameraPos - output.PosWorld );


	return output;
}

//-------------------------------------------------
//	ﾋﾟｸｾﾙｼｪｰﾀﾞ.
//-------------------------------------------------
float4 PS_NoTex( VS_OUTPUT input ) : SV_Target
{
	//ﾃｸｽﾁｬｶﾗｰ.
	//環境光　①.
	float4 ambient = g_Ambient;

	//拡散反射光 ②.
	float NL = saturate( dot( input.Normal, input.Light ) );
	float4 diffuse = g_Diffuse * NL;

	//鏡面反射光 ③.
	float3 reflect = normalize( 2 * NL * input.Normal - input.Light );
	float4 specular =
		pow( saturate( dot( reflect, input.EyeVector ) ), 4 )*g_Specular;

	//最終色　①②③の合計.
	float4 Color = ambient + diffuse + specular;
	return Color;
}
