#pragma once

//警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)
#include <crtdbg.h>
#include <cstdio>
#include <list>
#include <vector>
#include <map>
#include <cstdlib>
#include <memory>

//DirectX9
#include <d3dx9.h>
//DirectX11
#include <D3DX11.h>
#include <D3D11.h>
//DirectX10
#include <D3DX10.h>	//「D3DX～」の定義使用時に必要.
#include <D3D10.h>

#include "MyMacro.h"
#include "Util.h"

//ライブラリ読み込み.
#pragma comment( lib, "winmm.lib" )
//DirectX9
#pragma comment( lib, "d3dx9.lib" )
#pragma comment( lib, "d3d9.lib" )
//DirectX11
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
//DirectX10
#pragma comment( lib, "d3dx10.lib" )	//「D3DX～」の定義使用時に必要.

//=================================================
//	定数.
//=================================================
//#define WND_W 1280	//←定数宣言で#defineは使わない.
const int WND_W = 1280;	//ウィンドウの幅.
const int WND_H = 720;	//ウィンドウの高さ.
const int FPS = 60;		//フレームレート.
const float AFFINE_INTENSITY = 1.f;	//アフィン変換の強度.
const float VERTEX_SNAPPING = 32.f;		//頂点スナッピングの強度.
const int	MAX_LIGHT = 4;	//最大ライト数.
const double PI = 3.14159265358979323846;

//=================================	================
//	構造体
//=================================================
//カメラ情報構造体.
struct CAMERA
{
	D3DXVECTOR3	vPosition;	//視点.
	D3DXVECTOR3 vLook;		//注視点.
	float yaw;
	float pitch;
};

//ライト情報.
struct LIGHT
{
	D3DXVECTOR3	Position;	//位置.
	D3DXVECTOR3	vDirection;	//方向.
	D3DXMATRIX	mRotation;	//回転行列.
	float		fIntensity;	//強度(明るさ).
};

//フォグ情報.
struct FOG {
	bool		Enable;		//フォグ有効無効.
	D3DXVECTOR4 Color;		//フォグの色.
	float		Start;		//フォグ開始距離.
	float		End;		//フォグ終了距離.
	float		Density;	//フォグ密度(指数関数フォグ用).
	int			Mode;		//フォグの種類(線形,指数関数,指数関数2乗).
};

//スポットライト情報.
struct SPOT_LIGHT
{
	D3DXVECTOR4 LightOrigin;	//ライトの位置.
	D3DXVECTOR4 LightDir;	//ライトの方向.
	D3DXVECTOR4	LightColor;		//ライトの色.
	float		fIntensity;	//ライトの強度.
	float		fRange;		//ライトの届く距離.
	float		fInnerAngle;//内側の角度(完全に光る範囲).
	float		fOuterAngle;//外側の角度(徐々に暗くなる範囲).
};

//シーン情報構造体.
struct SCENE_DATA
{
	const char* name;
	D3DXMATRIX	mView;
	D3DXMATRIX	mProj;
	CAMERA		Camera;
	LIGHT		Light;
	FOG			Fog;
	SPOT_LIGHT*	pSpotLightArray;
	int			SpotLightNum;
};

struct Pair
{
	int x;
	int y;
};

