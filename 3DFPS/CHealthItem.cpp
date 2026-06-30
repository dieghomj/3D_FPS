#include "CHealthItem.h"

CHealthItem::CHealthItem()
	: CItem()
{
}

CHealthItem::~CHealthItem()
{
}

void CHealthItem::Update()
{
	RotateAnim(1000.f/FPS, D3DXToRadian(0.15f));

}

void CHealthItem::Draw(SCENE_DATA& sceneData)
{
	CStaticMeshObject::Draw(sceneData);
}

void CHealthItem::ActivateEffect()
{
}

void CHealthItem::DeactivateEffect()
{
}

void CHealthItem::OnPickup(CPlayer& player)
{
	// プレイヤーを回復.
}
