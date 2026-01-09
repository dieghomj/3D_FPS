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
	float GetAttackCD() const
	{
		return m_AttackCD;
	};
	void Die();

private:

	void Attack();
	void ChasePlayer();
	void RunAway();
	void IdleBehavior();

private:
	float m_PlayerDist;

	float m_AttackCD;
	bool m_HasAttacked;

#if _DEBUG
	public:
	float debugPlayerDist;
	float debugAttackCD;
	float debugStateTimer;
	float debugAnimSpeed;
#endif
};