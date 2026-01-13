#pragma once
#include "CAnimEnemy.h"

class CBoss :
	public CAnimEnemy
{
public:

	CBoss();
	virtual ~CBoss() override;
	virtual void InitEnemy() override;
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;
	virtual void Die() override;
	virtual void ApplyDamage(int damage) override;

private:

	enum Phase {
		Phase1,
		Phase2,
		Phase3,
		PhaseCount,
	};
	enum AttackType {
		MORPH,
		GROUNDSLAM,
		SHOOT,
		ATTACK_COUNT,
	};
	enum MorphShape {
		MORPH_HORIZONTAL,
		MORPH_VERTICAL,
	};

	void IdleBehavior();
	void Attack();
	
	void ChasePlayer();
	void MorphAttack();

	void MorphSlam();
	void MorphSweep();
	void MorphRecover();

	void GroundSlam();
	void ShootMinions();

	int NextPhase();

	int GetRandomMorphShape();

private:

	CROSSRAY m_CrossRay;
	RAY m_HeadRay;
	RAY m_FootRay;

	float m_PlayerDist;


	bool m_Attacked;
	bool m_MinionShot;
	bool m_GroundSlammed;
	bool m_Morphed;

	float m_PhaseHealth[PhaseCount];
	float m_MorphAttackHeight;
	float m_GroundSlamHeight;

	int m_CurrentShape;

	int m_MorphAttackCount;
	int m_ShootMinionCount;

	int m_CurrentAttack;
	int m_CurrentPhase;

	float m_SlamAnimTime;

	float m_MorphAttackCD;
	float m_SlamCD;
	float m_ShootMinionCD;

};