#include "stdafx.h"
#include "CStage.h"

CStage::CStage()
	: m_pPlayer(nullptr)
{
}

CStage::~CStage()
{
}

void CStage::Update()
{


}

void CStage::Draw(SCENE_DATA& sceneData)
{
	CStaticMeshObject::Draw(sceneData);
}


