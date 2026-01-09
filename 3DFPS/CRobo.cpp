#include "CRobo.h"

static constexpr float CHASE_SPEED = 0.1f;
static constexpr float IDLE_SPEED = 0.015f;
static constexpr float IDLE_TIMER_MAX = 2.0f;
static constexpr float HEALTH_MAX = 10.0f;
static constexpr float ATTACK_RANGE = 45.0f;
static constexpr float ATTACK_CD = 3.0f;

CRobo::CRobo()
	: CAnimEnemy()
	, m_PlayerDist(0.0f)
	, m_AttackCD(ATTACK_CD)
{
	m_State = Idle;
}

CRobo::~CRobo()
{
}
int i = 0;
float dt = 0.f;
void CRobo::Update()
{

	m_Shot = false;

	if (m_HasAttacked)
	{
		m_AttackCD += 0.016f;
		if (m_AttackCD >= ATTACK_CD)
		{
			m_HasAttacked = false;
			m_AttackCD = ATTACK_CD;
		}
	}
	
	//SetAnimNo(0, BLEND_CHANGE);
	FacePlayer(m_PlayerDist);
	m_State = Idle;
	Attack();
	CAnimEnemy::Update();
}

void CRobo::Draw(SCENE_DATA& sceneData)
{
	CAnimEnemy::Draw(sceneData);
}

void CRobo::ApplyDamage(int damage)
{
}

void CRobo::Die()
{
	m_IsAlive = false;
	m_State = Dead;
	SetAnimNo(4, FORCE_CHANGE); // Assuming 4 is the death animation
}

void CRobo::Attack()
{
	if(m_HasAttacked)
		return;

	m_Shot = true;
	m_AttackCD = 0.f;
	m_HasAttacked = true;
	//Set attack animation

}

void CRobo::ChasePlayer()
{
}

void CRobo::RunAway()
{
}

void CRobo::IdleBehavior()
{
}



