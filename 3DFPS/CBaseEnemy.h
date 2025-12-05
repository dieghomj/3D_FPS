#pragma once
#include "CAnimCharacter.h"
#include <vector>

class CSpotLight;

class CBaseEnemy :
	public CAnimCharacter
{

public:

	enum enState {
		Move,
		Attack,
		Retiring,
		Dead
	};

	CBaseEnemy();
	CBaseEnemy(std::vector<Pair> path, int width, int height);

	~CBaseEnemy();

	void Update();

	void Draw(SCENE_DATA& sceneData);

	void Start();
	void SetPath(std::vector<Pair> path);
	void SetWidthHeight(int width, int height) {
		m_MazeWidth = width;
		m_MazeHeight = height;
	}
	void SetRowCol(int row, int col) {
		m_CurrentRow = row;
		m_CurrentCol = col;
	}
	int GetCurrentRow() const { return m_CurrentRow; }
	int GetCurrentCol() const { return m_CurrentCol; }
	
	int GetState() const { return m_State; }

	void SetTargetPlayer(CGameObject* player) { m_pPlayer = player; }

	void ReactToSpotLight(const CSpotLight& spotLight);
	bool IsDead() const { return m_State == Dead; }

private:
	void GetNextStep();
	void AttackPlayer();
	void IdleMove();
	D3DXVECTOR3 SetRotationToTarget(D3DXVECTOR3 target);

	bool IsLitBySpot(const CSpotLight& spotLight) const;
	void ApplyRetreatAndDamage(float damage, float retreatForce);

protected:

	int m_State;

	int m_MazeWidth;
	int m_MazeHeight;
	int m_CurrentRow;
	int m_CurrentCol;
	
	std::vector<Pair> m_StepList;
	int m_pathStep;


	float m_Health = 100.f;
	float m_RetreatTimer = 0.f;
	float m_RetreatDuration = 2.f;

	float moveSpeed;
	float moveTime;
	float m_fDistanceFromPlayer;

	CGameObject* m_pPlayer;

};

