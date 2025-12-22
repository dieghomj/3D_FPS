#pragma once

#include "CAnimCharacter.h"
class CAnimEnemy :
	public CAnimCharacter
{
public:

	enum EnemyState {
		Idle,
		Chasing,
		Attacking,
		Jumping,
		Dying
	};


public:
	CAnimEnemy();
	virtual ~CAnimEnemy() override;
	virtual void InitEnemy();
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;

	int GetState() const {
		return m_State;
	}

	void SetState(int state) {
		m_State = state;
	}

	void SetPlayerPos(const D3DXVECTOR3& pos) {
		m_PlayerPos = pos;
	}

	void SpawnAt(const D3DXVECTOR3& pos) {
		SetPosition(pos);
		m_IsAlive = true;
	}

	void Kill() {
		m_IsAlive = false;
	}

	void SetFloorY(float y) { m_FloorY = y; }
	float GetFloorY() const { return m_FloorY; }

	void SetFloorNormal(const D3DXVECTOR3& normal) { m_FloorNormal = normal; }
	D3DXVECTOR3 GetFloorNormal() const { return m_FloorNormal; }

	void UpdateCrossRay();
	CROSSRAY GetHeadCrossRay() const { return *m_pHeadCrossRay; }

protected:
	D3DXVECTOR3 m_PlayerPos;

	int m_State;
	bool m_IsAlive;

	// 衝突検出用のメンバー
	float m_FloorY;
	D3DXVECTOR3 m_FloorNormal;
	CROSSRAY* m_pHeadCrossRay;  // 頭部の衝突レイ
};
