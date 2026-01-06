#include "CRobo.h"

CRobo::CRobo()
	: CAnimEnemy()
	, m_PlayerDist(0.0f)
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
	if(dt > 1.f)
	{
		dt = 0.f;
		i++;
	}
	else
	{
		dt += 0.00016f;
	}
	if(i>19)
		i = 0;
	SetAnimNo(i, BLEND_CHANGE);
	m_State = Idle;
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



