#include "stdafx.h"
#include "CLevelController.h"

CLevelController::CLevelController()
	: m_CurrentLevel	(0)
	, m_pLevels			()
{
}

CLevelController::~CLevelController()
{
}

void CLevelController::Init()
{
	m_pStage = new CStage();
	for (int i = 0; i < LEVEL_COUNT; ++i)
	{
		m_pLevels[i] = new CLevel();
		m_pLevels[i]->Init();
	}
}

void CLevelController::Update()
{
	m_pLevels[m_CurrentLevel]->Update();
	m_pStage->Update();
}

void CLevelController::Draw(SCENE_DATA& sceneInfo)
{
	m_pStage->Draw(sceneInfo);
}

void CLevelController::Restart()
{
	m_pStage->RestartPlayerPosition(m_pLevels[m_CurrentLevel]->GetStartPosition());

	m_pLevels[m_CurrentLevel]->Restart();
	
}

void CLevelController::SetPlayer(CPlayer* player) {
	m_pPlayer = player;
	m_pStage->SetPlayer(*m_pPlayer);
	for (int i = 0; i < LEVEL_COUNT; ++i) {
		m_pLevels[i]->SetPlayer(m_pPlayer);
	}
}

void CLevelController::SetCurrentLevel(int levelIndex) {
	m_CurrentLevel = levelIndex;
	m_pStage->AttachMesh(*m_pLevels[m_CurrentLevel]->GetStageMesh());
	m_pPlayer->SetPosition(m_pLevels[m_CurrentLevel]->GetStartPosition());
	Restart();
}
