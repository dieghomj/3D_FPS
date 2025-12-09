#include "stdafx.h"
#include "CStage.h"

CStage::CStage()
	: m_pPlayer(nullptr)
	, m_prevCrossRay()
{
}

CStage::~CStage()
{
}

void CStage::Update()
{
	float Distance = 0.f;
	D3DXVECTOR3 Intersect(0.f, 0.f, 0.f);
	D3DXVECTOR3 Normal(0.f, 0.f, 0.f);
	CROSSRAY CrossRay = m_pPlayer->GetCrossRay();
	RAY ray = RAY();

	//-------------------------
	// 十字のレイ処理
	//-------------------------	
	//壁との当たり判定
	CalculatePositionFromWall(&CrossRay);
	D3DXVECTOR3 Pos = m_pPlayer->GetPosition();
	Pos.x = CrossRay.Ray[CROSSRAY::XL].Position.x;
	Pos.z = CrossRay.Ray[CROSSRAY::XL].Position.z;
	m_pPlayer->SetPosition(Pos);


		D3DXVECTOR3 diff = Pos - m_prevPlayerPos;
		float len = D3DXVec3Length(&diff);
		D3DXVec3Normalize(&diff, &diff);
		ray.Length = len + 0.1f; //少し長めに設定
		ray.Axis = diff;
		ray.Position = m_prevPlayerPos;
		ray.RotationY = m_pPlayer->GetRotation().y;

		if (IsHitForRay(ray, &Distance, &Intersect, &Normal) == true)
		{
			m_pPlayer->SetPosition(Intersect + Normal);
		}
	
	//-------------------------
	// Y軸のレイ処理
	//-------------------------
	ray = m_pPlayer->GetRayY();	//プレイヤーが持っているレイの情報
	Distance = 0.f;
	Intersect = D3DXVECTOR3(0.f, 0.f, 0.f);

	//プレイヤーのレイと当たり判定
	if (IsHitForRay(ray, &Distance, &Intersect) == true)
	{
		m_pPlayer->SetFloorY(Intersect.y);
	}
	else m_pPlayer->SetFloorY(-1000.f); //床が無い場所にいる場合

	m_prevPlayerPos = m_pPlayer->GetPosition();
	for (int dir = 0; dir < 4; dir++)
	{
		m_prevCrossRay.Ray[dir] = CrossRay.Ray[dir];
	}

}

void CStage::Draw(SCENE_DATA& sceneData)
{
	CStaticMeshObject::Draw(sceneData);
}


