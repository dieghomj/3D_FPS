#include "stdafx.h"
#include "CStage.h"

CStage::CStage()
{
}

CStage::~CStage()
{
}

void CStage::Update()
{

	//-------------------------
	// Y軸のレイ処理
	//-------------------------
	RAY ray = m_pPlayer->GetRayY();	//プレイヤーが持っているレイの情報
	float Distance = 0.f;
	D3DXVECTOR3 Intersect(0.f, 0.f, 0.f);

	//プレイヤーのレイと当たり判定
	if (IsHitForRay(ray, &Distance, &Intersect) == true)
	{
		m_pPlayer->SetFloorY(Intersect.y);
	}
	else m_pPlayer->SetFloorY(-1000.f); //床が無い場所にいる場合

	//-------------------------
	// 十字のレイ処理
	//-------------------------	
	CROSSRAY CrossRay = m_pPlayer->GetCrossRay();
	
	//壁との当たり判定
	CalculatePositionFromWall(&CrossRay);
	D3DXVECTOR3 Pos = m_pPlayer->GetPosition();
	Pos.x = CrossRay.Ray[CROSSRAY::XL].Position.x;
	Pos.z = CrossRay.Ray[CROSSRAY::XL].Position.z;
	m_pPlayer->SetPosition(Pos);

	CStaticMeshObject::Update();
}

void CStage::Draw(SCENE_DATA& sceneData)
{
	CStaticMeshObject::Draw(sceneData);
}


