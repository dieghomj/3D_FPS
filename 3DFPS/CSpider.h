#pragma once
#include "CAnimEnemy.h"

class CSpider : public CAnimEnemy
{

public:


public:

	CSpider();
	virtual ~CSpider() override;
	
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;

private:

	void Attack();
	void JumpAttack();
	void ChasePlayer();
	void IdleBehavior();
	void Die();

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