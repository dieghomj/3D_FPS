#include "CRobo.h"

static constexpr float CHASE_SPEED = 0.1f;
static constexpr float RUN_AWAY_SPEED = 0.2f;
static constexpr float IDLE_SPEED = 0.015f;
static constexpr float IDLE_TIMER_MAX = 2.0f;
static constexpr float HEALTH_MAX = 8.0f;
static constexpr float ATTACK_RANGE = 249.f;
static constexpr float RUN_AWAY_RANGE = 20.f;
static constexpr float ATTACK_CD = 2.0f;

CRobo::CRobo()
	: CAnimEnemy()
	, m_PlayerDist(0.0f)
	, m_AttackCD(ATTACK_CD)
	, m_HasAttacked(false)
	
{
	m_Health = HEALTH_MAX;
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
	FacePlayer(m_PlayerDist, false);

	if (m_HasAttacked)
	{
		m_AttackCD += 0.016f;
		if (m_AttackCD >= ATTACK_CD)
		{
			m_HasAttacked = false;
			m_AttackCD = ATTACK_CD;
			m_State = Idle;
		}
	}
	
	switch (m_State)
	{
	case Spawning:
		m_State = Idle;
		break;
	case Idle:
		IdleBehavior();
		break;
	case Chasing:
		ChasePlayer();
		break;
	case Attacking:
		Attack();
		break;
	case Running:
		RunAway();
		break;
	case Jumping:
	case Damaged:
	case Dying:
	case Dead:
		break;
	default:
		break;
	}

	CAnimEnemy::Update();
}

void CRobo::Draw(SCENE_DATA& sceneData)
{
	CAnimEnemy::Draw(sceneData);
}

void CRobo::ApplyDamage(int damage)
 {

	m_Health-= damage;
	if (m_State != Damaged)
	{
		m_State = Damaged;
	}
	if (m_Health <= 0.0f)
	{
		Die();
	}
	else
	{
	}

}

void CRobo::Die()
{
	m_IsAlive = false;
	m_State = Dead;
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

	if(m_PlayerDist > 2.f * RUN_AWAY_RANGE )
	{
		m_State = Idle;
		return;
	}
	D3DXVECTOR3 dir = m_vPosition - m_PlayerPos;
	D3DXVec3Normalize(&dir, &dir);
	m_vPosition += dir * RUN_AWAY_SPEED;

}

void CRobo::IdleBehavior()
{


	if (m_PlayerDist <= ATTACK_RANGE)
	{
		if (m_PlayerDist <= RUN_AWAY_RANGE)
		{
			m_State = Running;
			return;
		}
		m_State = Attacking;
	}
	else
	{
		m_State = Chasing;
	}

}



