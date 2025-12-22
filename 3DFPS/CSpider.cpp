#include "CSpider.h"


//Anim
// 1: Walk
// 2: ?
// 3: Two arm Attack
// 4: Pre-Jump
// 5: ?
// 6: Jump
// 7: Nod
// 8: Die1
// 9 : Die2
// 10: One arm attack

static constexpr float ATTACK_CD = 2.0f;
static constexpr float JUMP_ATTACK_CD = 3.0f;
static constexpr float CHASE_SPEED = 0.1f;
static constexpr float IDLE_SPEED = 0.015f;
static constexpr float JUMP_ATTACK_SPEED = 0.35f;

CSpider::CSpider()
	: CAnimEnemy()
	, m_PlayerDist(0.0f)

{
	SetAnimSpeed(0.015);
	m_State = Idle;
}

CSpider::~CSpider()
{
}

void CSpider::Update()
{
	
	D3DXVECTOR3 toPlayer = m_PlayerPos - GetPosition();
	m_PlayerDist = D3DXVec3Length(&toPlayer);
	D3DXVECTOR3 prevFwd = m_vForward;
	m_vForward = m_PlayerPos - GetPosition();
	m_vForward.y = 0.0f;

	float angle = atan2f(m_vForward.x, m_vForward.z) - atan2f(prevFwd.x, prevFwd.z);

	SetRotation(0.0f,D3DXToRadian(180.f) + GetRotation().y + angle, 0.0f);

	switch (m_State)
	{
	case Idle:
		IdleBehavior();
		break;
	case Chasing:
		ChasePlayer();
		break;
	case Jumping:
		JumpAttack();
		break;
	case Attacking:
		Attack();
		break;
	case Dying:
		Die();
		break;
	default:
		break;
	}

	CAnimEnemy::Update();
}

void CSpider::Draw(SCENE_DATA& sceneData)
{
	CAnimEnemy::Draw(sceneData);
}

void CSpider::Attack()
{

	if(GetAnimNo() == 3 && IsAnimOver())
	{
		m_State = Idle;
		return;
	}
	SetAnimNo(3, BLEND_CHANGE);

}

void CSpider::JumpAttack()
{

	if (GetAnimNo() == 6 && IsAnimOver())
	{
		m_State = Attacking;
		return;
	}
	SetAnimNo(6, BLEND_CHANGE);
	m_vPosition += m_vForward * -JUMP_ATTACK_SPEED;
	m_vPosition.y += 0.2f;


	
}

void CSpider::ChasePlayer()
{
	SetAnimNo(1, BLEND_CHANGE);
	D3DXVec3Normalize(&m_vForward, &m_vForward);
	SetPosition(GetPosition() + m_vForward * -CHASE_SPEED);
	if (m_PlayerDist < 0.5f)
		m_State = Attacking;
	else if (m_PlayerDist < 15.0f)
		m_State = Jumping;
}

void CSpider::IdleBehavior()
{
	SetAnimNo(1, BLEND_CHANGE);

	if (m_PlayerDist < 2.3f)
		m_State = Attacking;
	else if(m_PlayerDist < 30.0f)
		m_State = Chasing;
	 
}

void CSpider::Die()
{

}
