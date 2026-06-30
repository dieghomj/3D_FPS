#include "CCharacter.h"

CCharacter::CCharacter()
	: m_Shot		(false)
	, m_pRayY		( nullptr )
	, m_pCrossRay	( nullptr )
	, m_Radius		(0.3f)	//とりあえず0.3にしておく
	, m_Height		(1.8f)	//とりあえず1.8にしておく
	, m_GravityEnabled(true)
	, m_Velocity(0.f, 0.f, 0.f)
{
	m_pRayY = new RAY();
	m_pRayY->Axis = D3DXVECTOR3(0.f, -1.f, 0.f);	//下向きの軸
	m_pRayY->Length = 10.f;		//とりあえず10にしておく
	m_pCrossRay = new CROSSRAY();
}

CCharacter::~CCharacter()
{
	SAFE_DELETE( m_pCrossRay );
	SAFE_DELETE( m_pRayY );
}

void CCharacter::Update()
{
	CStaticMeshObject::Update();
}

void CCharacter::Draw(SCENE_DATA& sceneData)
{
	CStaticMeshObject::Draw(sceneData);
}

void CCharacter::UpdateRayY(float posY)
{
	//レイの位置をプレイヤーの座標にそろえる
	m_pRayY->Position = m_vPosition;
	//地面めり込み回避のためプレイヤーの位置よりも少し上にしておく
	m_pRayY->Position.y = m_vPosition.y - m_Height * 0.001f;
	m_pRayY->RotationY = m_vRotation.y;
	m_pRayY->Length = m_Height + 0.9f;
}

void CCharacter::UpdateCrossRay(CROSSRAY& crossRay, float posY)
{
	// レイの長さを速度に応じて変化させる
	const float RAY_LENGTH = 1.5f;

	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		crossRay.Ray[dir].Position = m_vPosition;
		crossRay.Ray[dir].Position.y = posY;
		crossRay.Ray[dir].RotationY = m_vRotation.y;
		crossRay.Ray[dir].Length = RAY_LENGTH;
	}
}