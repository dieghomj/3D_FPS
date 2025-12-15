#include "CItem.h"

CItem::CItem()
	: CStaticMeshObject()
{
}

CItem::~CItem()
{
}

void CItem::Update()
{
}

void CItem::Draw(SCENE_DATA& sceneData)
{
	CStaticMeshObject::Draw(sceneData);
}
