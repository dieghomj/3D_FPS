#pragma once
#include "CAnimEnemy.h"

class CRobo : public CAnimEnemy
{
public:

	CRobo();
	virtual ~CRobo() override;
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;
	virtual void ApplyDamage(int damage) override;
	void Die();

private:

	void Attack();
	void ChasePlayer();
	void RunAway();
	void IdleBehavior();

private:
	float m_PlayerDist;
#if _DEBUG
	public:
	float debugPlayerDist;
	float debugAttackCD;
	float debugStateTimer;
	float debugAnimSpeed;
#endif
};