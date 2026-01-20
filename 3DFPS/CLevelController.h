#pragma once
#include "CLevel.h"
#include "CPlayer.h"
constexpr int LEVEL_COUNT = 3;


class CLevelController
{

public:

	CLevelController();
	~CLevelController();

	void Init();
	void Update();
	void Draw(SCENE_DATA& sceneInfo);

	void Restart();

	void SetPlayer(CPlayer* player) {
		m_pPlayer = player;
		m_pStage->SetPlayer(*m_pPlayer);
		for (int i = 0; i < LEVEL_COUNT; ++i) {
			m_pLevels[i]->SetPlayer(m_pPlayer);
		}
	};

	void SetCurrentLevel(int levelIndex) { 
		m_CurrentLevel = levelIndex; 
		m_pStage->AttachMesh(*m_pLevels[m_CurrentLevel]->GetStageMesh());
		m_pPlayer->SetPosition(m_pLevels[m_CurrentLevel]->GetStartPosition());
		Restart();
	};
	void SetStageScale(float scale) {
		m_pStage->SetScale(scale);
	};
	CLevel* GetCurrentLevel() const { return m_pLevels[m_CurrentLevel]; };

private:



private:

	int m_CurrentLevel = 0;
	CLevel* m_pLevels[LEVEL_COUNT];
	CStage* m_pStage;
	CPlayer* m_pPlayer;
};

