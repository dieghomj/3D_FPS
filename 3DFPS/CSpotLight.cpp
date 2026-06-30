#include "stdafx.h"
#include "CSpotLight.h"


CSpotLight::CSpotLight()
	: m_vPosition	()
	, m_vDirection	(0.0f, -1.0f, 0.0f)
	, m_Color		(1.0f, 1.0f, 1.0f, 1.0f)
	, m_Range		(10.0f)
	, m_InnerAngle	(D3DXToRadian(20.0f))
	, m_OuterAngle	(D3DXToRadian(30.0f))
	, m_Intensity	(1.0f)
	, m_Index		(-1)
{
}

CSpotLight::~CSpotLight()
{
}

void CSpotLight::Init()
{
}

void CSpotLight::Release()
{
}

void CSpotLight::Update()
{
}

void CSpotLight::Render(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, D3DXVECTOR3& CameraPos, FOG& Fog)
{

}

