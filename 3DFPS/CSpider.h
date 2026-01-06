#pragma once
#include "CAnimEnemy.h"

class CSpider : public CAnimEnemy
{

public:


public:

	CSpider();
	virtual ~CSpider() override;
	
	virtual void InitEnemy() override;
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;
	void ApplyDamage(int damage);
	void Die();

private:

	void SpawnBehavior();
	void Attack();
	void JumpAttack();
	void ChasePlayer();
	void IdleBehavior();
	void DamagedAnim();

	void PushBackEffect();

private:

	float m_PlayerDist;
	float m_DamageAnimCd;

#if _DEBUG
public:

	float debugPlayerDist;
	float debugAttackCD;
	float debugStateTimer;
	float debugAnimSpeed;
#endif

};