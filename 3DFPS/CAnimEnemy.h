#pragma once

#include "CAnimCharacter.h"
class CAnimEnemy :
	public CAnimCharacter
{
public:

	enum EnemyState {
		Spawning,
		Idle,
		Chasing,
		Attacking,
		Jumping,
		Damaged,
		Dying,
		Dead
	};


public:
	CAnimEnemy();
	virtual ~CAnimEnemy() override;
	virtual void InitEnemy();
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;
	virtual void Die() = 0;
	virtual void ApplyDamage(int damge) = 0;

	bool IsDead() const {
		return !m_IsAlive;
	}

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
		m_State = Spawning;
		m_IsActive = true;
	}

	void Kill() {
		m_IsAlive = false;
	}

	void SetFloorY(float y) { m_FloorY = y; }
	float GetFloorY() const { return m_FloorY; }

	void SetFloorNormal(const D3DXVECTOR3& normal) { m_FloorNormal = normal; }
	D3DXVECTOR3 GetFloorNormal() const { return m_FloorNormal; }

	void SetRotationSpeed(float speed) { m_RotationSpeed = speed; }

	void UpdateCrossRay();
	CROSSRAY GetHeadCrossRay() const { return *m_pHeadCrossRay; }

protected:

	void FacePlayer(float& distance);

protected:
	D3DXVECTOR3 m_PlayerPos;

	int m_State;
	float m_Health;
	bool m_IsAlive;

	float m_RotationSpeed;

	// 衝突検出用のメンバー
	float m_FloorY;
	D3DXVECTOR3 m_FloorNormal;
	CROSSRAY* m_pHeadCrossRay;  // 頭部の衝突レイ
};
