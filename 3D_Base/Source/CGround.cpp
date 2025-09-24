#include "CGround.h"

CGround::CGround()
	: m_pPlayer		( nullptr )
{
}

CGround::~CGround()
{
	m_pPlayer = nullptr;
}

void CGround::Update()
{
//	//地面をスクロールさせる
//	m_vPosition.z -= 0.2f;
//	if (m_vPosition.z < -100.f) {
//		m_vPosition.z = 0.f;
//	}

	//-------------------------
	// Y軸のレイ処理
	//-------------------------
	RAY ray = m_pPlayer->GetRayY();	//プレイヤーが持っているレイの情報
	float Distance = 0.f;
	D3DXVECTOR3 Intersect(0.f, 0.f, 0.f);

	//プレイヤーのレイと当たり判定
	if (IsHitForRay(ray, &Distance, &Intersect) == true)
	{
		D3DXVECTOR3 Pos = m_pPlayer->GetPosition();
		Pos.y = Intersect.y + 1.f; //1.fで少し上へ補正
		m_pPlayer->SetPosition(Pos);
	}

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

}
