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

	void SetPlayer(CPlayer* player);;

	void SetCurrentLevel(int levelIndex);;
	void SetStageScale(float scale) { m_pStage->SetScale(scale); };
	CLevel* GetCurrentLevel() const { return m_pLevels[m_CurrentLevel]; };
	CStage* GetStage() const { return m_pStage; };

private:



private:

	int m_CurrentLevel = 0;
	CLevel* m_pLevels[LEVEL_COUNT];
	CStage* m_pStage;
	CPlayer* m_pPlayer;
};

