#pragma once
#include "CStage.h"
#include "CPlayer.h"
#include "CBlockedPath.h"

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

	bool IsCleared() const { return m_IsCleared; }

	void SetStage(CStaticMesh* pStageMesh) { m_pStageMesh = pStageMesh; };
	void SetPlayer(CPlayer* player) { m_pPlayer = player; };
	void SetGoal(GOAL goal) { m_Goal = goal; };
	void SetCollisionTriggers(const std::vector<COLLISION_TRIGGER> triggers) { m_CollisionTriggers = triggers; };
	void SetBlockedPathList(const std::vector<CBlockedPath*> pblockedPaths) { m_pBlockedPathList = pblockedPaths; }
	void SetEnemySpawnPositions(const std::vector<D3DXVECTOR3>& positions) { m_EnemySpawnPosition = positions; }
	void SetStartPosition(const D3DXVECTOR3& pos) { m_StartPosition = pos; }

	CStaticMesh* GetStageMesh() const { return m_pStageMesh; }
	D3DXVECTOR3 GetStartPosition() const { return m_StartPosition; }

private:

	void CheckTriggers();
	void CheckGoal();
	bool IsPlayerInTriggerArea(const CLevel::COLLISION_TRIGGER& trigger);
	bool IsPlayerInTriggerArea(const CLevel::GOAL& trigger);
	void UpdateBlockedPaths();
	void HandleBlockedPath();
private:

	bool m_IsCleared;

	float m_StageScale;

	D3DXVECTOR3 m_StartPosition;


	CStaticMesh* m_pStageMesh;

	CPlayer* m_pPlayer;
	GOAL m_Goal;
	std::vector<COLLISION_TRIGGER> m_CollisionTriggers;
	std::vector<CBlockedPath*> m_pBlockedPathList;
	std::vector<D3DXVECTOR3> m_CheckpointPositions;
	std::vector<D3DXVECTOR3> m_EnemySpawnPosition;


};

