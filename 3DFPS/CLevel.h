#pragma once
#include "CStage.h"
#include "CPlayer.h"
#include "CBlockedPath.h"
#include "CDataReader.h"

class CGame;

class CLevel
{
public:

	struct GOAL
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 size;
		bool isReached;
	};

	struct COLLISION_TRIGGER
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 size;
		std::vector<int> blockedPathIndices;
		bool isTriggered;
		bool blockBehindPlayer;
	};

	CLevel();
	~CLevel();
	void Init();
	void Update();
	void Draw();
	void Restart();

	bool IsCleared() const { return m_IsCleared; }

	// Load level data from CDataReader
	void LoadFromData(const CDataReader& dataReader, int levelID);

	void SetStage(CStaticMesh* pStageMesh) { m_pStageMesh = pStageMesh; };
	void SetPlayer(CPlayer* player) { m_pPlayer = player; };
	void SetGoal(GOAL goal) { m_Goal = goal; };
	void SetTriggerAreas(const std::vector<COLLISION_TRIGGER> triggers) { m_TriggerAreas = triggers; };
	void SetBlockedPathList(const std::vector<CBlockedPath*> pblockedPaths) { m_pBlockedPathList = pblockedPaths; }
	void SetEnemySpawnPositions(const std::vector<D3DXVECTOR3>& positions) { m_EnemySpawnPosition = positions; }
	void SetStartPosition(const D3DXVECTOR3& pos) { m_StartPosition = pos; }

	bool IsAreaTriggered(int index) const {

		if (index < 0 || index >= m_TriggerAreas.size()) {
			return false;
		}

		return m_TriggerAreas[index].isTriggered;
	}

	int GetTriggerAreaCount() const { return static_cast<int>(m_TriggerAreas.size()); }
	CStaticMesh* GetStageMesh() const { return m_pStageMesh; }
	D3DXVECTOR3 GetStartPosition() const { return m_StartPosition; }
	float GetTimer() const { return m_Timer; }
	GOAL GetGoal() const { return m_Goal; }
	const std::vector<D3DXVECTOR3>& GetEnemySpawnPositions() const { return m_EnemySpawnPosition; }

private:

	void CheckTriggers();
	void CheckGoal();
	bool IsPlayerInTriggerArea(const CLevel::COLLISION_TRIGGER& trigger);
	bool IsPlayerInTriggerArea(const CLevel::GOAL& trigger);
	void UpdateBlockedPaths();
	void HandleBlockedPath();

private:

	bool m_IsCleared;
	int m_LevelID;
	float m_StageScale;
	float m_Timer;

	D3DXVECTOR3 m_StartPosition;

	CStaticMesh* m_pStageMesh;

	CPlayer* m_pPlayer;
	GOAL m_Goal;
	std::vector<COLLISION_TRIGGER> m_TriggerAreas;
	std::vector<CBlockedPath*> m_pBlockedPathList;
	std::vector<D3DXVECTOR3> m_CheckpointPositions;
	std::vector<D3DXVECTOR3> m_EnemySpawnPosition;
	std::vector<bool> m_TriggeredFlags;

};

