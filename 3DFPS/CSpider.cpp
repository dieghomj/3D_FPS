#include "CSpider.h"
#include "CRobo.h"


//アニメーション
// 1: 歩行
// 2: ?
// 3: 両腕攻撃
// 4: ジャンプ前
// 5: ?
// 6: ジャンプ
// 7: のけぞり
// 8: 死亡1
// 9 : 死亡2
// 10: 片腕攻撃

static constexpr float ATTACK_CD = 2.0f;
static constexpr float JUMP_ATTACK_CD = 3.0f;
static constexpr float CHASE_SPEED = 0.3f;
static constexpr float IDLE_SPEED = 0.025f;
static constexpr float IDLE_TIMER_MAX = 2.0f;
static constexpr float JUMP_ATTACK_SPEED = 0.85f;
static constexpr float HEALTH_MAX = 8.f;
static constexpr float DAMAGE_TIMER_MAX = 0.5f;

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

void CSpider::InitEnemy()
{
	m_Radius = 1.8f;
	m_Health = HEALTH_MAX;
	m_IsAlive = true;
	CAnimEnemy::InitEnemy();
}

void CSpider::Update()
{

	if (IsActive() == false)
	{
		return;
	}

	if (!m_IsAlive && m_State == Dead)
	{
		return;
	}
	else if (!m_IsAlive)
	{
		m_State = Dying;
	}
	else if (m_State == Dead)
	{
		m_State = Idle;
	}

	FacePlayer(m_PlayerDist);

	switch (m_State)
	{
	case Spawning:
		SpawnBehavior();
		break;

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
	case Damaged:
		DamagedAnim();
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
	if (IsActive() == false)
	{
		return;
	}

	CAnimEnemy::Draw(sceneData);
}

void CSpider::ApplyDamage(int damage)
{
	m_Health -= damage;

	if (m_State != Damaged)
	{
		m_State = Damaged;
	}

	if (m_Health <= 0.0f)
	{
		Kill();
	}
}

void CSpider::SpawnBehavior()
{
	m_State = Idle;
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
	m_vPosition = GetPosition() + m_vForward * -CHASE_SPEED;
	if (m_PlayerDist < 0.5f)
		m_State = Attacking;
	else if (m_PlayerDist < 35.0f)
		m_State = Jumping;
}

void CSpider::IdleBehavior()
{
	SetAnimNo(1, BLEND_CHANGE);

	if (m_PlayerDist < 3.3f)
		m_State = Attacking;
	else if(m_PlayerDist < 500.0f)
		m_State = Chasing;
	 
}

void CSpider::DamagedAnim()
{
	if (GetAnimNo() == 7 && IsAnimOver())
	{
		m_State = Idle;
		return;
	}
	SetAnimNo(7, BLEND_CHANGE);
	PushBackEffect();
}

void CSpider::PushBackEffect()
{
	m_vPosition += m_vForward * 0.3f;
	m_vPosition.y += 0.15f;
}

void CSpider::Die()
{

	if(GetAnimNo() == 8 && IsAnimOver())
	{
		m_State = Dead;
		SetActive(false);
		m_vPosition = D3DXVECTOR3(0.0f, -100.0f, 0.0f);
		return;
	}


	SetAnimNo(8, FORCE_CHANGE);
	PushBackEffect();
}
