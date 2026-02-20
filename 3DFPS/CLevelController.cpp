#include "stdafx.h"
#include "CLevelController.h"

CLevelController::CLevelController()
	: m_CurrentLevel	(0)
	, m_pLevels			()
	, m_pStage			(nullptr)
	, m_pPlayer			(nullptr)
	, m_pDataReader		(nullptr)
{
}

CLevelController::~CLevelController()
{
	for (int i = 0; i < LEVEL_COUNT; ++i)
	{
		if (m_pLevels[i])
		{
			delete m_pLevels[i];
			m_pLevels[i] = nullptr;
		}
	}

	if (m_pStage)
	{
		delete m_pStage;
		m_pStage = nullptr;
	}

	if (m_pDataReader)
	{
		delete m_pDataReader;
		m_pDataReader = nullptr;
	}
}

void CLevelController::Init()
{
	m_pStage = new CStage();
	m_pDataReader = new CDataReader();

	// Load all CSV data
	m_pDataReader->LoadData(L"Data/CSV/");

	// Create and initialize levels using CSV data
	for (int i = 0; i < LEVEL_COUNT; ++i)
	{
		m_pLevels[i] = new CLevel();
		m_pLevels[i]->Init();
		m_pLevels[i]->LoadFromData(*m_pDataReader, i);
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

void CLevelController::SetPlayer(CPlayer* player) 
{
	m_pPlayer = player;
	m_pStage->SetPlayer(*m_pPlayer);
	for (int i = 0; i < LEVEL_COUNT; ++i) 
	{
		m_pLevels[i]->SetPlayer(m_pPlayer);
	}
}

void CLevelController::SetCurrentLevel(int levelIndex) 
{
	if (levelIndex < 0 || levelIndex >= LEVEL_COUNT)
		return;

	m_CurrentLevel = levelIndex;
	
	CStaticMesh* pStageMesh = m_pLevels[m_CurrentLevel]->GetStageMesh();
	if (pStageMesh)
	{
		m_pStage->AttachMesh(*pStageMesh);
	}

	if (m_pPlayer)
	{
		m_pPlayer->SetPosition(m_pLevels[m_CurrentLevel]->GetStartPosition());
	}
	
	Restart();
}

void CLevelController::SetStageMesh(int levelIndex, CStaticMesh* pMesh)
{
	if (levelIndex < 0 || levelIndex >= LEVEL_COUNT)
		return;

	m_pLevels[levelIndex]->SetStage(pMesh);
}

float CLevelController::GetLevelTimer(int levelIndex) const
{
	if (levelIndex < 0 || levelIndex >= LEVEL_COUNT)
		return 180.f;

	return m_pLevels[levelIndex]->GetTimer();
}
