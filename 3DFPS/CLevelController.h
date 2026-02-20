#pragma once
#include "CLevel.h"
#include "CPlayer.h"
#include "CDataReader.h"
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
	void SetStageMesh(int levelIndex, CStaticMesh* pMesh);

	CLevel* GetCurrentLevel() const { return m_pLevels[m_CurrentLevel]; };
	CStage* GetStage() const { return m_pStage; };
	CDataReader* GetDataReader() const { return m_pDataReader; };
	float GetLevelTimer(int levelIndex) const;

private:



private:

	int m_CurrentLevel = 0;
	CLevel* m_pLevels[LEVEL_COUNT];
	CStage* m_pStage;
	CPlayer* m_pPlayer;
	CDataReader* m_pDataReader;
};

