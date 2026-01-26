#include "CBoss.h"

static constexpr float SPAWN_OFFSET = -8.f; // 16ms per frame ~ 60FPS

static constexpr float FLIGHT_HEIGHT = 22.0f;
static constexpr float RECOVER_SPEED = 0.1f;

static constexpr float UPDOWN_AMPLITUDE = 0.25f;
static constexpr float SCALEMORPH_AMPLITUDE = 7.25f;

static constexpr float IDLE_SPEED = 0.01f;
static constexpr float CHASE_SPEED = 0.44f;

static constexpr float SLAM_RANGE = 1.0f;
static constexpr float SHOOT_MINION_RANGE = 100.f;
static constexpr float MORPH_ATTACK_RANGE = 20.f;

static constexpr float SLAM_CD = 4.0f;
static constexpr float SHOOT_MINION_CD = 3.0f;
static constexpr float MORPH_ATTACK_CD = 88.0f;

static constexpr float MORPH_ATTACK_WIDTH = 55.5f;
static constexpr float MORPH_ATTACK_HEIGHT = 15.0f;
static constexpr float MORPH_ATTACK_AREA = 106.f;
static constexpr float MORPH_ATTACK_SPEED = 0.5f;

static constexpr float SLAM_ATTACK_DURATION = 1.5f;

static constexpr float SLAM_SPEED = 0.9f;

CBoss::CBoss()
	: CAnimEnemy()
	, shapeInt(0, 1)
	, m_PlayerDist(0.0f)
	, m_GroundSlammed(false)
	, m_Morphed(false)
	, m_Attacked(false)
	, m_MinionShot(false)
	, m_SpawnFlag(false)
	, m_MorphAttackHeight(0.0f)
	, m_GroundSlamHeight(0.0f)
	, m_MorphAttackCount(0)
	, m_ShootMinionCount(0)
	, m_CurrentAttack(0)
	, m_CurrentPhase(0)
	, m_SlamAnimTime(0.0f)
	, m_MorphAttackCD(0.0f)
	, m_SlamCD(SLAM_CD)
	, m_ShootMinionCD(0.0f)
	, m_PhaseHealth{ 300.0f, 200.0f, 100.0f }
	, m_MorphSweepStartPosition(0.0f, 0.0f, 0.0f)
	, m_CurrentAttackState(0)
	, m_CurrentShape(0)
	, m_OriginalScale(4.9f, 4.9f, 4.9f)

{

	m_Radius = 5.0f;
	m_GravityEnabled = false;
	m_State = Idle;

}

CBoss::~CBoss()
{
}

void CBoss::InitEnemy()
{
	CAnimEnemy::InitEnemy();
	m_Health = 300.0f;
	m_IsAlive = true;
	m_RotationSpeed = 0.03f;
	m_vScale = m_OriginalScale;
	m_Attacked = false;
	m_Morphed = false;
	m_GroundSlammed = false;
	m_CurrentAttack = GROUNDSLAM;
	m_CurrentPhase = Phase1;
	m_CurrentAttackState = START;
	m_IsGrounded = false;
	m_ShootMinionCD = SHOOT_MINION_CD;
	m_MorphAttackCD = MORPH_ATTACK_CD;
	m_SlamAnimTime = 0.f;
	m_SlamCD = SLAM_CD;
	m_SpawnFlag = false;
	m_GroundSlammed = false;
}


void CBoss::Update()
{
	if (m_FloorY <= -FLT_MAX)
	{
		m_FloorY = 2.0f;
	}

	if ((m_CurrentAttack != MORPH))
		FacePlayer(m_PlayerDist);
	m_Shot = false;

	if (m_MinionShot)
	{
		m_ShootMinionCD += 0.016f;
		if (m_ShootMinionCD > SHOOT_MINION_CD)
		{
			m_Attacked = false;
			m_MinionShot = false;
			m_ShootMinionCD = SHOOT_MINION_CD;
			m_State = Idle;
		}
	}

	else if (m_Morphed)
	{
		m_MorphAttackCD += 0.016f;
		ScaleMorphAnim(0.016f, 0.25f, 2.0f);
		if (m_MorphAttackCD > MORPH_ATTACK_CD)
		{
			m_Attacked = false;
			m_Morphed = false;
			m_MorphAttackCD = MORPH_ATTACK_CD;
			m_State = Idle;
			//m_vScale = D3DXVECTOR3(4.9f, 4.9f, 4.9f);
		}
	}

	else if (m_GroundSlammed)
	{
		m_SlamAnimTime = 0.f;
		m_SlamCD += 0.016f;

		RecoverPosition();

		if (m_SlamCD > SLAM_CD)
		{
			m_GroundSlammed = false;
			m_Attacked = false;
			m_SlamCD = SLAM_CD;
		}
		return;
	}
		

	
	
	float dt = FPS/1000.f;
	//UpDownAnim(dt, 0.25f, 1.0f);
	//ScaleMorphAnim(dt, 7.25f, 2.0f);

	//VibrateAnim(0.016f, 0.3f, 1.9f);


	switch (m_State)
	{
	case Spawning:
		if(m_SpawnFlag == false)
			m_vPosition.y += SPAWN_OFFSET;
		m_SpawnFlag = true;
		if (RecoverPosition())
		{
			m_StartPosition = m_vPosition;
			m_State = Idle;
		}
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
	default:
		break;
	}

	CAnimEnemy::Update();

}

bool CBoss::RecoverPosition()
{
	float objectiveHeight = m_FloorY + FLIGHT_HEIGHT;
	if (m_vPosition.y < objectiveHeight)
	{
		m_vPosition.y += RECOVER_SPEED;
		if (m_vPosition.y > objectiveHeight)
			m_vPosition.y = objectiveHeight;
	}
	else if (m_vPosition.y > objectiveHeight)
	{
		m_vPosition.y -= RECOVER_SPEED;
		if (m_vPosition.y < objectiveHeight)
			m_vPosition.y = objectiveHeight;

	}
	else
		return true;
	return false;
}

void CBoss::Draw(SCENE_DATA& sceneData)
{
}

void CBoss::IdleBehavior()
{
	m_State = Chasing;
}

void CBoss::Attack()
{
	switch (m_CurrentAttack)
	{

	case MORPH:
		
		switch (m_CurrentAttackState)
		{
			case START:
				m_MorphSweepStartPosition = m_vPosition;
				MorphAttack();
				break;
			case SLAM:
				MorphSlam();
				break;
			case SWEEP:
				MorphSweep();
				break;
			case RECOVER:
				MorphRecover();
				break;
		default:
			MorphAttack();
			break;
		}

		break;
	case GROUNDSLAM:
		GroundSlam();
		break;
	case SHOOT:
		ShootMinions();
		break;
	default:
		break;
	}


}

void CBoss::ChasePlayer()
{
	float dist = 0;
	D3DXVECTOR3 toPlayer = m_vPosition - m_PlayerPos  ;
	toPlayer.y = 0.0f;
	dist = D3DXVec3Length(&toPlayer);

	if (m_Morphed && m_Health <= 200.f)
	{
		m_State = Attacking;
		m_CurrentAttack = SHOOT;
		return;
	}
	else if(dist <= MORPH_ATTACK_RANGE && m_Health <= 200.f)
	{
		m_State = Attacking;
		m_CurrentAttackState = START;
		m_CurrentAttack = MORPH;
		return;
	}
	else if (dist <= SLAM_RANGE)
	{
		m_State = Attacking;
		m_CurrentAttack = GROUNDSLAM;
		return;
	}

	m_vPosition += -m_vForward * CHASE_SPEED;

}


// Morph attack logic
void CBoss::MorphAttack()
{

	m_CurrentShape = MORPH_HORIZONTAL;
	m_vScale = D3DXVECTOR3(MORPH_ATTACK_WIDTH, MORPH_ATTACK_HEIGHT, 4.9f);
	m_vRotation = D3DXVECTOR3(0.f, 45.f, 0.f);
	MorphSlam();
	m_CurrentAttack = MORPH;

}

// Morph slam logic
// Move up and down quickly in a slam motion
// Stay down and the sweep attack
void CBoss::MorphSlam()
{
	m_CurrentAttackState = SLAM;
	m_SlamAnimTime += 0.016f;
	if (m_SlamAnimTime >= 2.5f && IsGrounded() == false)
	{
		m_vPosition.y -= 1.5f * SLAM_SPEED; // Move down
		return;
	}
	if (IsGrounded() == true)
	{
		MorphSweep();
	}

	m_vPosition.y += 0.9f; // Move up


}

void CBoss::MorphSweep()
{
	if (MoveToPosition(m_StartPosition, CHASE_SPEED) &&
		m_CurrentAttackState == START)
		return;
	m_CurrentAttackState = SWEEP;
	if(m_MorphAttackCount >= 3)
	{
		MorphRecover(); 
		m_Morphed = false;
		m_MorphAttackCount = 0;
		return;
	}
	static float dirSpeed = (rand() % 2) == 0 ? 1 : -1;
	static float distance = 0.f;
	static D3DXVECTOR3 accumVector = D3DXVECTOR3(0.f,0.f,0.f);
	if (distance >= MORPH_ATTACK_AREA)
	{
		dirSpeed = -1;
		m_MorphAttackCount++;
	}
	else if (distance <= -(MORPH_ATTACK_AREA))
	{
		dirSpeed = 1;
		m_MorphAttackCount++;
	}
	accumVector += m_vForward * dirSpeed * MORPH_ATTACK_SPEED;
	distance = D3DXVec3Length(&accumVector);
	m_vPosition += m_vForward * dirSpeed * MORPH_ATTACK_SPEED;

}

void CBoss::MorphRecover()
{
	m_CurrentAttackState = RECOVER;
	
	m_vScale = m_OriginalScale;
	m_Attacked = true;
	m_Morphed = true;
	m_MorphAttackCD = 0.f;
	m_SlamAnimTime = 0.f;
	ScaleMorphAnim(0.016f, 0.25f, 2.0f);

	if (RecoverPosition())
	{
		m_CurrentAttack = GROUNDSLAM;
		m_CurrentAttackState = START;
		m_State = Idle;
	}
}

// Slam attack logic
// Move up for a brief moment and slam down quickly
// Check if player is within SLAM_RANGE
// If yes, apply damage or effects
void CBoss::GroundSlam()
{
	if(m_SlamCD < SLAM_CD)
		return;

	m_SlamAnimTime += 0.016f;
	if (m_SlamAnimTime >= 0.5f && IsGrounded() == false)
	{
		VibrateAnim(0.016f, 0.05f, 9.50f);
		m_vPosition.y -= 1.5f * SLAM_SPEED; // Move down
		return;
	}
	if (IsGrounded() == true)
	{
		m_GroundSlammed = true;
		m_Attacked = true;
		m_SlamAnimTime = 0.f;
		m_SlamCD = 0.f;
		m_State = Idle;
		return;
	}
	VibrateAnim(0.016f, 0.05f, 9.50f);

	m_vPosition.y += 0.9f; // Move up

}

void CBoss::ShootMinions()
{

	if (m_MinionShot)
		return;

	m_Shot = true;
	m_MinionShot = true;
	m_Attacked = true;
	m_ShootMinionCD = 0.f;

}

int CBoss::NextPhase()
{
	if(m_CurrentPhase < PhaseCount - 1)
		m_CurrentPhase = 0;
	m_CurrentPhase++;
	return m_CurrentPhase;
}

int CBoss::GetRandomMorphShape()
{
	int shape = shapeInt(m_RandomGen);
	return shape;
}

void CBoss::Die()
{
}

void CBoss::ApplyDamage(int damage)
{
	m_Health -= damage;
	if (m_Health <= 0.0f)
	{
		m_Health = 0.0f;
		Die();
	}
}

bool CBoss::MoveToPosition(const D3DXVECTOR3& targetPos, float speed)
{
	D3DXVECTOR3 direction = targetPos - m_vPosition;
	float distance = D3DXVec3Length(&direction);

	// –Ú•WˆÊ’u‚É“ž’B‚µ‚½ê‡
	if (distance <= speed)
	{
		m_vPosition = targetPos;
		return true;
	}

	// •ûŒü‚ð³‹K‰»‚µ‚ÄˆÚ“®
	D3DXVec3Normalize(&direction, &direction);
	m_vPosition += direction * speed;

	return false;
}
