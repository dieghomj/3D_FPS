#include "CBoss.h"

static constexpr float SPAWN_OFFSET = -8.f; // 16ms per frame ~ 60FPS

static constexpr float FLIGHT_HEIGHT = 22.0f;
static constexpr float RECOVER_SPEED = 0.3f;

static constexpr float UPDOWN_AMPLITUDE = 0.25f;
static constexpr float SCALEMORPH_AMPLITUDE = 7.25f;

static constexpr float IDLE_SPEED = 0.91f;
static constexpr float CHASE_SPEED = 0.74f;

static constexpr float SLAM_RANGE = 1.0f;
static constexpr float SHOOT_MINION_RANGE = 100.f;
static constexpr float MORPH_ATTACK_RANGE = 20.f;

static constexpr float SLAM_CD = 1.0f;
static constexpr float SHOOT_MINION_CD = 0.5f;
static constexpr float MORPH_ATTACK_CD = 88.0f;

static constexpr float MORPH_ATTACK_WIDTH = 55.5f;
static constexpr float MORPH_ATTACK_HEIGHT = 15.0f;
static constexpr float MORPH_ATTACK_AREA = 5.f;
static constexpr float MORPH_ATTACK_SPEED = 1.0f;

static constexpr float SLAM_ATTACK_DURATION = 1.5f;

static constexpr float SLAM_SPEED = 0.9f;
static constexpr float DASH_CHARGE_TIME = 0.7f;
static constexpr float DASH_SPEED = 2.8f;
static constexpr float DASH_MAX_TRAVEL = 60.0f;

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
	, m_DashChargeTimer(0.0f)
	, m_DashTravel(0.0f)
	, m_DashDirection(0.f, 0.f, 0.f)
	, m_MorphAttackCD(0.0f)
	, m_SlamCD(SLAM_CD)
	, m_ShootMinionCD(0.0f)
	, m_PhaseHealth{ 300.0f, 200.0f, 100.0f }
	, m_MorphSweepStartPosition(0.0f, 0.0f, 0.0f)
	, m_CurrentAttackState(0)
	, m_CurrentShape(0)
	, m_OriginalScale(4.9f, 4.9f, 4.9f)
	, m_DeathTimer(0.0f)
	, m_DeathDuration(3.0f)
	, m_DeathSinkDepth(6.0f)
	, m_DeathStartY(0.0f)
	, m_MorphMoveTimer(0.0f)
	, m_SlamPatternCount(0)
	, m_SlamPatternTarget(2)
	, m_ShootPatternTimer(0.0f)
	, m_LowHPPatternActive(false)
	, m_LowHPSequenceStep(0)

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
	m_Health = 500.0f;
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
	m_DeathTimer = 0.0f;
	m_DeathStartY = m_vPosition.y;
	m_MorphMoveTimer = 0.0f;
	m_SlamPatternCount = 0;
	m_SlamPatternTarget = 2 + (m_RandomGen() % 2);
	m_ShootPatternTimer = 0.0f;
	m_LowHPPatternActive = false;
	m_LowHPSequenceStep = 0;
	m_DashChargeTimer = 0.0f;
	m_DashTravel = 0.0f;
	m_DashDirection = D3DXVECTOR3(0.f, 0.f, 0.f);
}


void CBoss::Update()
{
	if (IsActive() == false)
	{
		return;
	}

	if (m_FloorY <= -FLT_MAX)
	{
		m_FloorY = 2.0f;
	}

	float dt = FPS / 1000.f;

	if (m_State == Dying)
	{
		m_DeathTimer += dt;
		VibrateAnim(dt, 0.08f, 12.0f);

		float targetY = (m_FloorY <= -FLT_MAX) ? (m_DeathStartY - m_DeathSinkDepth) : (m_FloorY - m_DeathSinkDepth);
		float t = m_DeathTimer / m_DeathDuration;
		if (t > 1.0f)
			t = 1.0f;

		m_vPosition.y = m_DeathStartY + (targetY - m_DeathStartY) * t;

		if (m_DeathTimer >= m_DeathDuration)
		{
			m_State = Dead;
			SetActive(false);
			m_vPosition = D3DXVECTOR3(0.0f, -100.0f, 0.0f);
		}
		return;
	}

	// High-HP shoot phase timer accumulation
	if (m_CurrentAttack == SHOOT && m_MinionShot)
	{
		m_ShootPatternTimer += dt;
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
			if (m_LowHPPatternActive)
			{
				// ensure next attack in low-HP loop runs after shooting
				m_LowHPSequenceStep = 1; // next is morph
			}
		}
	}

	else if (m_Morphed && m_State != Attacking)
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

	// Dash strike cooldown/recovery handling
	if (m_CurrentAttack == DASH_STRIKE && m_State != Attacking)
	{
		m_DashChargeTimer = 0.f;
		m_DashTravel = 0.f;
	}
		

	
	

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
		MorphAttack();
		break;
	case GROUNDSLAM:
		GroundSlam();
		break;
	case SHOOT:
		ShootMinions();
		break;
	case DASH_STRIKE:
		DashStrike();
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


	// Low HP pattern: Morph -> Shoot -> Slam loop
	if (m_Health <= 250.f)
	{
		m_LowHPPatternActive = true;
		if (m_State != Attacking)
		{
			switch (m_LowHPSequenceStep)
			{
			case 0:
				m_CurrentAttack = SHOOT;
				break;
			case 1:
				m_CurrentAttack = DASH_STRIKE;
				m_CurrentAttackState = START;
				break;
			case 2:
				m_CurrentAttack = GROUNDSLAM;
				m_CurrentAttackState = START;
				break;
			default:
				m_CurrentAttack = SHOOT;
				m_LowHPSequenceStep = 0;
				break;
			}
			m_State = Attacking;
		}
		return;
	}

	// High HP pattern: Slam 2-3 times, then shoot for ~10s, repeat
	m_LowHPPatternActive = false;
	if (m_SlamPatternCount < m_SlamPatternTarget)
	{
		m_State = Attacking;
		m_CurrentAttack = GROUNDSLAM;
		return;
	}

	if (m_ShootPatternTimer < 10.0f)
	{
		m_State = Attacking;
		m_CurrentAttack = SHOOT;
		return;
	}

	// Reset pattern after shooting phase
	m_SlamPatternCount = 0;
	m_SlamPatternTarget = 2 + (m_RandomGen() % 2);
	m_ShootPatternTimer = 0.0f;
	m_State = Attacking;
	m_CurrentAttack = GROUNDSLAM;
}


// Morph attack logic
void CBoss::MorphAttack()
{
	float dt = FPS / 1000.f;

	if (m_CurrentAttackState == START)
	{
		// Step 1: return to start position before morph charge
		if (!MoveToPosition(m_StartPosition, IDLE_SPEED))
		{
			return;
		}

		// Step 2: rotate and enlarge before rushing the player
		m_vRotation.y += D3DXToRadian(45.0f);
		m_CurrentAttackState = SWEEP;
		m_MorphMoveTimer = 0.0f;
		m_MorphAttackCount = 0;
		m_MorphAttackCD = 0.f;
		m_Morphed = true;
		m_vScale = D3DXVECTOR3(MORPH_ATTACK_WIDTH, MORPH_ATTACK_HEIGHT, 4.9f);
		m_CurrentAttack = MORPH;
	}

	// Engage sweep phase logic (spawns minions, sweeps, then slams)
	if (m_CurrentAttackState == SWEEP)
	{
		MorphSweep();
		return;
	}

	m_MorphMoveTimer += dt;

	// Move toward player while enlarged
	D3DXVECTOR3 dirToPlayer = m_PlayerPos - m_vPosition;
	dirToPlayer.y = 0.0f;
	float distToPlayer = D3DXVec3Length(&dirToPlayer);
	if (distToPlayer > 35.f)
	{
		D3DXVec3Normalize(&dirToPlayer, &dirToPlayer);
		m_vPosition += dirToPlayer * MORPH_ATTACK_SPEED;
	}

	// Keep a subtle morph scaling animation while charging
	ScaleMorphAnim(dt, 0.25f, 2.0f);

	if (m_MorphMoveTimer >= 0.5f)
	{
		m_MorphMoveTimer = 0.0f;
		m_CurrentAttackState = RECOVER;
	}

	// Recover phase (fallback): return to start then transition to slam loop
	if (m_CurrentAttackState == RECOVER)
	{
		if (!MoveToPosition(m_StartPosition, IDLE_SPEED ))
		{
			return;
		}

		m_vScale = m_OriginalScale;
		m_CurrentAttackState = START;
		m_Morphed = false;
		m_CurrentAttack = GROUNDSLAM;
		m_Attacked = false;
		m_LowHPSequenceStep = 0; // reset to shoot after slam
		return;
	}

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

	static bool spawnedMinions = false;
	if (m_CurrentAttackState != SWEEP)
	{
		spawnedMinions = false;
	}

	// stay enlarged during sweep
	m_vScale = D3DXVECTOR3(MORPH_ATTACK_WIDTH, MORPH_ATTACK_HEIGHT, 4.9f);
	m_CurrentAttackState = SWEEP;

	if (!spawnedMinions)
	{
		ShootMinions();
		spawnedMinions = true;
	}

	// sweep/charge toward player until within slam distance
	D3DXVECTOR3 dirToPlayer = m_PlayerPos - m_vPosition;
	dirToPlayer.y = 0.f;
	float distToPlayer = D3DXVec3Length(&dirToPlayer);
	if (distToPlayer > 35.f)
	{
		if (distToPlayer > 1e-4f)
		{
			D3DXVec3Normalize(&dirToPlayer, &dirToPlayer);
			m_vPosition += dirToPlayer * MORPH_ATTACK_SPEED;
		}
	}
	else
	{
		// close enough: transition to slam over the player's position
		m_vPosition.x = m_PlayerPos.x;
		m_vPosition.z = m_PlayerPos.z;
		m_CurrentAttack = GROUNDSLAM;
		m_CurrentAttackState = START;
		m_State = Attacking;
		spawnedMinions = false;
	}

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

void CBoss::DashStrike()
{
	// Phase 1: move to offset distance and telegraph
	if (m_CurrentAttackState == START)
	{
		D3DXVECTOR3 dirToPlayer = m_PlayerPos - m_vPosition;
		dirToPlayer.y = 0.f;
		float dist = D3DXVec3Length(&dirToPlayer);
		if (dist > 1e-4f)
		{
			D3DXVec3Normalize(&dirToPlayer, &dirToPlayer);
			D3DXVECTOR3 targetPos = m_PlayerPos - dirToPlayer * 35.f;
			MoveToPosition(targetPos, CHASE_SPEED);
		}
		m_DashChargeTimer += 0.016f;
		VibrateAnim(0.016f, 0.05f, 6.0f);
		if (m_DashChargeTimer >= DASH_CHARGE_TIME)
		{
			m_DashChargeTimer = 0.f;
			m_DashTravel = 0.f;
			m_DashDirection = m_PlayerPos - m_vPosition;
			m_DashDirection.y = 0.f;
			if (D3DXVec3LengthSq(&m_DashDirection) > 1e-4f)
			{
				D3DXVec3Normalize(&m_DashDirection, &m_DashDirection);
			}
			else
			{
				m_DashDirection = D3DXVECTOR3(0.f, 0.f, 1.f);
			}
			m_CurrentAttackState = DASHING;
		}
		return;
	}

	// Phase 2: dash forward
	if (m_CurrentAttackState == DASHING)
	{
		D3DXVECTOR3 delta = m_DashDirection * DASH_SPEED;
		m_vPosition += delta;
		m_DashTravel += D3DXVec3Length(&delta);
		VibrateAnim(0.016f, 0.03f, 8.0f);

		D3DXVECTOR3 toPlayer = m_PlayerPos - m_vPosition;
		toPlayer.y = 0.f;
		float distToPlayer = D3DXVec3Length(&toPlayer);
		if (distToPlayer < SLAM_RANGE + 1.0f || m_DashTravel >= DASH_MAX_TRAVEL)
		{
			m_vPosition.x = m_PlayerPos.x;
			m_vPosition.z = m_PlayerPos.z;
			m_CurrentAttack = GROUNDSLAM;
			m_CurrentAttackState = START;
			m_State = Attacking;
			m_DashChargeTimer = 0.f;
			m_DashTravel = 0.f;
		}
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

	// Chase player horizontally while performing the slam
	D3DXVECTOR3 dirToPlayer = m_PlayerPos - m_vPosition;
	dirToPlayer.y = 0.0f;
	if (D3DXVec3Length(&dirToPlayer) > 1e-4f)
	{
		D3DXVec3Normalize(&dirToPlayer, &dirToPlayer);
		m_vPosition += dirToPlayer * (CHASE_SPEED );
	}

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
		if (!m_LowHPPatternActive)
		{
			m_SlamPatternCount++;
		}
		m_SlamAnimTime = 0.f;
		m_SlamCD = 0.f;
		m_State = Idle;
		if (m_LowHPPatternActive)
		{
			m_LowHPSequenceStep = 0; // reset to shoot after slam
		}
		return;
	}
	VibrateAnim(0.016f, 0.05f, 9.50f);

	m_vPosition.y += 0.9f; // Move up

}

void CBoss::ShootMinions()
{

	if (m_MinionShot)
		return;

	// Return to center/start before shooting and reset scale
	if (!MoveToPosition(m_StartPosition, IDLE_SPEED))
	{
		m_vScale = m_OriginalScale;
		return;
	}

	m_vScale = m_OriginalScale;

	m_Shot = true;
	m_MinionShot = true;
	m_Attacked = true;
	m_ShootMinionCD = 0.f;

	if (m_LowHPPatternActive)
	{
		m_LowHPSequenceStep = 1; // next should be dash strike
	}

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
	if (m_State == Dying || m_State == Dead)
	{
		return;
	}

	m_IsAlive = false;
	m_State = Dying;
	m_DeathTimer = 0.0f;
	m_DeathStartY = m_vPosition.y;
	m_Shot = false;
	m_Attacked = true;
}

void CBoss::ApplyDamage(int damage)
{
	if (m_State == Dying || m_State == Dead)
	{
		return;
	}

	m_Health -= damage;

	// Small knockback opposite current facing
	D3DXVECTOR3 knockDir = m_vForward;
	knockDir.y = 0.0f;
	if (D3DXVec3LengthSq(&knockDir) > 1e-4f)
	{
		D3DXVec3Normalize(&knockDir, &knockDir);
		m_vPosition += knockDir * 0.35f;
	}
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
