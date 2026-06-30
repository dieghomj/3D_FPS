#include "CPlayer.h"

static constexpr float GRAVITY = 0.0198f;
static constexpr float FRICTION = 0.0390f;


static constexpr float HEALTH_MAX = 100.f;

static constexpr float PLAYERSIZE = 6.5;
static constexpr float CROUCHSIZE = 2.8f;
static constexpr float PLAYERRADIUS = 0.3f;

static constexpr float RUN_SPEED = 0.55f;
static constexpr float MAX_RUN_SPEED = 0.8f;

static constexpr float CROUCH_SPEED = 0.9f;

static constexpr float JUMP_STRENGTH = 0.58f;

static constexpr float PISTOL_CD = 0.15f; // 秒
static constexpr float SHOTGUN_CD = 0.15f; // 秒

static constexpr float DASH_SPEED = 2.5f;
static constexpr float DASH_DISTANCE = 5.5f;
static constexpr float DASH_COOLDOWN = 0.05f; // 秒
static constexpr float DASH_MAX = 3.f; // 秒

static constexpr float SLIDE_FRICTION = 0.0003f;
static constexpr float SLIDE_START_SPEED = 0.01f;

static bool debugGodMode = false;

CPlayer::CPlayer()
	: CCharacter()
	, m_pInputHandler			(nullptr)
	
	, m_State					(Idle)
	
	, m_MoveSpeed				(RUN_SPEED)
	, m_JumpStrength			(JUMP_STRENGTH)
	, m_Health					(HEALTH_MAX)
	, m_DashTimer				(DASH_MAX)
	
	, m_currWeapon				(0)
	, m_ShootCooldownTimer		(0.f)
	, m_CanShoot				(true)
	
	, m_FloorY					(0.f)
	, m_Right					(1.f, 0.f, 0.f)
	, m_Forward					(0.f, 0.f, 1.f)
	, m_Acceleration			(0.f, 0.f, 0.f)
	, m_Inertia					(0.f, 0.f, 0.f)
	, m_DashDirection			(0.f, 0.f, 1.f)

	, m_IsOnGround				(true)
	, m_IsJumping				(false)
	, m_IsDashing				(false)
	, m_IsCrouching				(false)
	, m_IsSliding				(false)

	, m_CanDash					(true)
	, m_CanJump					(true)
	, m_CanCrouch				(true)
	, m_CanSlide				(true)
	, m_CanMove					(true)

	, m_IsInertiaEnabled(true)
	, m_InvFrame(false)
	, m_InvFrameTimer(0.f)
	, m_FloorNormal(0.f, 1.f, 0.f)
	, m_GroundedTimer(0.f)
{
	m_pInputHandler = new CInput();
	m_pHeadCrossRay = new CROSSRAY();
	m_Radius = PLAYERRADIUS;
	m_Height = PLAYERSIZE;
	m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_GravityEnabled = true;
}

CPlayer::~CPlayer()
{
}

void CPlayer::InitPlayer()
{
	m_Health = HEALTH_MAX;
	m_Height = PLAYERSIZE;
	m_MoveSpeed = RUN_SPEED;
	m_DashTimer = DASH_MAX;
	m_JumpStrength = JUMP_STRENGTH;
	m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_Inertia = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_currWeapon = 0;
}

void CPlayer::Update()
{
	m_pInputHandler->Update();
	if (debugGodMode)
	{
		m_Health = HEALTH_MAX;
		HandleInput();
		Move();
		return;
	}

	m_IsDamaged = false;

	float weaponCD = 0.f;
	switch (m_currWeapon)
	{
		case 0: // ピストル
			weaponCD = PISTOL_CD;
			break;
		case 1: // ショットガン
			weaponCD = SHOTGUN_CD;
			break;
		case 2: // 空
			weaponCD = 0.f;
			break;
		default:
			weaponCD = 0.f;
			break;
	}

	m_Shot = false;

	if (m_InvFrameTimer > 0.5f)
	{
		m_InvFrame = false;
		m_InvFrameTimer = 0.f;
	}
	else if (m_InvFrame)
	{
		m_InvFrameTimer += 0.016f;
	}

	if(m_DashTimer < DASH_MAX)
		m_DashTimer += 0.006f; 

	if (!m_CanShoot && (m_ShootCooldownTimer < weaponCD))
	{
		m_CanShoot = false;
		m_ShootCooldownTimer += 0.016f;
	}
	else
	{
		m_CanShoot = true;
		m_ShootCooldownTimer = 0.f;
	}
	
	HandleInput();

	if (!m_IsSliding)
	{
		m_Height = PLAYERSIZE;
		m_MoveSpeed = RUN_SPEED;
	}

	HandleAirPhys();
	Move();
}


void CPlayer::Draw(SCENE_DATA& sceneData)
{
}

void CPlayer::HandleInput()
{

	CalculateVectors();
	CalculateInertia();
	m_DashDirection = GetForwardVector();

	D3DXVECTOR3 inputVel = D3DXVECTOR3(0.f, m_Velocity.y, 0.f );
	bool hasInput = false;

	if(!m_IsSliding)
	{
#if _DEBUG
		if (m_pInputHandler->GetKeyDown('G'))
		{

			debugGodMode = !debugGodMode;
			
		}

#endif
		
		if (m_pInputHandler->GetKey('W'))
		{
			inputVel += m_Forward * m_MoveSpeed;
			m_DashDirection = GetForwardVector();
			hasInput = true;
		}

		if (m_pInputHandler->GetKey('S'))
		{
			inputVel += -m_Forward * m_MoveSpeed;
			m_DashDirection = -GetForwardVector();
			hasInput = true;
		}

		if (m_pInputHandler->GetKey('A'))
		{
			inputVel += -m_Right * m_MoveSpeed;
			m_DashDirection = -GetRightVector();
			hasInput = true;
		}

		if (m_pInputHandler->GetKey('D'))
		{
			inputVel += m_Right * m_MoveSpeed;
			m_DashDirection = GetRightVector();
			hasInput = true;
		}
	}

	if (!m_IsJumping && !m_IsDashing && !m_IsSliding)
	{
		if (hasInput && m_IsOnGround)
		{
			m_State = Walking;
		}
		else if (m_IsOnGround)
		{
			m_State = Idle;
		}
	}
	if(debugGodMode)
	{
		inputVel.y = 0.f;

		if (m_pInputHandler->GetKey(VK_SPACE))
		{
			inputVel.y += m_MoveSpeed;
		}

		if (m_pInputHandler->GetKey(VK_CONTROL))
		{
			inputVel.y -= m_MoveSpeed;
		}

		m_Velocity = inputVel * 1.5f;
		
		return;
	}

	m_Velocity = inputVel;

	if (m_pInputHandler->GetKeyDown(VK_LBUTTON))
	{
		Shoot();
	}

	if (m_pInputHandler->GetKeyDown(VK_RBUTTON))
	{
		SpecialAction();
	}

	if (m_pInputHandler->GetKeyDown(VK_SPACE))
	{
		Jump();
	}

	if (m_pInputHandler->GetKeyDown('Q'))
	{
		PrevWeapon();
	}

	if (m_pInputHandler->GetKeyDown('E'))
	{
		NextWeapon();
	}

	if (m_pInputHandler->GetKeyDown(VK_SHIFT) && !m_IsSliding)
	{
		Dash();
	}

	if (m_pInputHandler->GetKey(VK_CONTROL) || m_pInputHandler->GetKey('C'))
	{
		m_Inertia += inputVel;
		Slide();
	}
	else
	{
		m_IsSliding = false;
	}

	if (m_IsInertiaEnabled)
	{
		m_Velocity.x += m_Inertia.x;
		m_Velocity.z += m_Inertia.z;
	}

}

void CPlayer::CalculateVectors()
{
	D3DXVECTOR3 vecZ = D3DXVECTOR3(0.f, 0.f, 1.f);
	D3DXVECTOR3 vecX = D3DXVECTOR3(1.f, 0.f, 0.f);

	D3DXMATRIX rotY;
	D3DXMatrixRotationY(&rotY, m_vRotation.y);

	D3DXVec3TransformCoord(&m_Forward, &m_Forward, &rotY);
	D3DXVec3TransformCoord(&m_Right, &m_Right, &rotY);

	D3DXVec3Normalize(&m_Forward, &m_Forward);
	D3DXVec3Normalize(&m_Right, &m_Right);
}

void CPlayer::ApplyForce(const D3DXVECTOR3& force, float mass = 1.f)
{

	m_Acceleration += force / mass;

}

void CPlayer::Move()
{

	if (!m_IsSliding)
	{
	}

	m_vPosition += m_Velocity ;

	UpdateAxis();

}


void CPlayer::Shoot()
{
	if( !m_CanShoot || m_ShootCooldownTimer > 0.f )
	{
		return;
	}

	m_Shot = true;
	m_CanShoot = false;

}

void CPlayer::Jump()
{
	// 接地中、またはコヨーテタイム（猶予時間）内であればジャンプを許可する.
	if ((m_IsOnGround || m_GroundedTimer > 0.f) && !m_IsJumping)
	{
		m_State = Jumping;
		m_Velocity.y = m_JumpStrength;
		m_IsOnGround = false;
		m_GroundedTimer = 0.f;  // 猶予時間を消費する.
		m_IsJumping = true;
		return;
	}

}

void CPlayer::Dash()
{
	if (m_DashTimer <= 1.0f)
	{
		return;
	}

	m_DashTimer -= 1.0f;

	m_IsDashing = true;
	m_State = Dashing;

	{

		D3DXVECTOR3 dashDirection = m_DashDirection;
		D3DXVec3Normalize(&dashDirection, &dashDirection);

		ApplyDamage(0.0f);
		m_Velocity += dashDirection * (RUN_SPEED + DASH_SPEED);
		m_Inertia += dashDirection * (RUN_SPEED + DASH_SPEED) * 0.5f;
	}

}

void CPlayer::Slide()
{

	D3DXVECTOR3 horizontalVel = D3DXVECTOR3( m_Velocity.x, 0.f, m_Velocity.z);
	float vel = D3DXVec3Length(&m_Velocity);

	m_IsSliding = true;
	m_MoveSpeed = CROUCH_SPEED;
	m_Height = CROUCHSIZE;
	m_State = Sliding;

}

void CPlayer::SpecialAction()
{



}

void CPlayer::UpdateAxis()
{

	UpdateRayY(m_vPosition.y - m_Height * 0.95f);
	UpdateCrossRay(*m_pHeadCrossRay, m_vPosition.y  + 0.1f);
	UpdateCrossRay(*m_pCrossRay, m_vPosition.y - m_Height * 0.5f);
	m_pRayY->Axis = -m_FloorNormal;
	for (int i = 0; i < CROSSRAY::enDir::max; i++)
	{
		D3DXVECTOR3 dir;
		D3DXVec3Cross(&dir, &m_pRayY->Axis, &m_pCrossRay->Ray[i].Axis);
		D3DXVec3Normalize(&dir, &dir);
		m_pCrossRay->Ray[i].Axis = dir;
		m_pHeadCrossRay->Ray[i].Axis = dir;
	}

}


// 摩擦による慣性の減衰を計算
void CPlayer::CalculateInertia()
{
	// 摩擦による慣性の減衰を計算
	if( m_IsSliding )
	{
		m_Inertia.x = (m_Inertia.x - (m_Inertia.x * SLIDE_FRICTION));
		m_Inertia.z = (m_Inertia.z - (m_Inertia.z * SLIDE_FRICTION));
	}
	else
	{
		m_Inertia.x = (m_Inertia.x - (m_Inertia.x * FRICTION));
		m_Inertia.z = (m_Inertia.z - (m_Inertia.z * FRICTION));
	}

	// 慣性が非常に小さくなったら0にする
	if (m_Inertia.x < 0.0f && m_Inertia.x > -0.00f)
	{
		m_Inertia.x = 0.f;
	}

	if (m_Inertia.z < 0.00f && m_Inertia.z > -0.00f)
	{
		m_Inertia.z = 0.f;
	}

	D3DXVECTOR3 inertia = m_Inertia;
	inertia.y = 0.f;
	float len = D3DXVec3Length(&inertia);

	if(len <= RUN_SPEED)
	{
		m_IsDashing = false;
	}


}

void CPlayer::HandleAirPhys()
{
	float feetY = m_vPosition.y - m_Height;
	float headY = m_vPosition.y + 0.1f;

	// 坂道向けに許容値を大きめに設定する.
	const float GROUND_SNAP_DISTANCE = 0.35f;      // 坂道向けにかなり大きめ.
	const float GROUND_PENETRATION = 0.05f;        // めり込みをより寛容に許容する.
	const float CEILING_BUFFER = 0.01f;
	const float GROUNDED_GRACE_TIME = 0.1f;        // 坂道でのジャンプ用「コヨーテタイム」.
	const float MAX_SLOPE_ANGLE = 0.7f;            // 約45度（角度のコサイン値）.
	const float SLOPE_SLIDE_THRESHOLD = 0.5f;      // これより急だと滑り落ちる.

	float distanceToFloor = feetY - m_FloorY;

	// 床の法線から坂道の角度を計算する.
	float slopeAngle = m_FloorNormal.y;  // 1.0 = 平面、0.0 = 垂直な壁.
	bool isOnWalkableSlope = slopeAngle >= MAX_SLOPE_ANGLE;
	bool isOnSteepSlope = slopeAngle < MAX_SLOPE_ANGLE && slopeAngle > SLOPE_SLIDE_THRESHOLD;

	// 坂道向けに大きめの許容値で接地判定を行う.
	if (distanceToFloor >= -GROUND_PENETRATION &&
		distanceToFloor <= GROUND_SNAP_DISTANCE &&
		m_Velocity.y <= 0.f &&
		isOnWalkableSlope)
	{
		// 歩行可能な地面の上にいる - 床にスナップする.
		m_IsOnGround = true;
		m_GroundedTimer = GROUNDED_GRACE_TIME;
		
		// 瞬時ではなく滑らかに床へスナップする.
		float snapSpeed = 0.3f;
		float targetY = m_FloorY + m_Height;
		m_vPosition.y += (targetY - m_vPosition.y) * snapSpeed;
		
		m_Velocity.y = 0.f;
		m_Inertia.y = 0.f;

		if (m_IsJumping && m_Velocity.y <= 0.0f)
		{
			m_IsJumping = false;
		}
	}
	else if (distanceToFloor < -GROUND_PENETRATION && isOnWalkableSlope)
	{
		// 地面にめり込んでいる - 上へ押し戻す.
		m_vPosition.y = m_FloorY + m_Height;
		m_IsOnGround = true;
		m_GroundedTimer = GROUNDED_GRACE_TIME;
		m_Velocity.y = 0.f;
		m_Inertia.y = 0.f;
		m_IsJumping = false;
	}
	else if (isOnSteepSlope && distanceToFloor <= GROUND_SNAP_DISTANCE)
	{
		// 急な坂道の上にいる - 滑り落ちる！
		m_IsOnGround = true;  // 一部の処理上はまだ「接地」扱いとする.
		m_GroundedTimer = GROUNDED_GRACE_TIME;
		m_IsJumping = false;
		
		// 滑る方向（坂を下る向き）を計算する.
		D3DXVECTOR3 slopeRight;
		D3DXVECTOR3 up(0.f, 1.f, 0.f);
		D3DXVec3Cross(&slopeRight, &up, &m_FloorNormal);
		D3DXVec3Normalize(&slopeRight, &slopeRight);
		
		D3DXVECTOR3 slideDir;
		D3DXVec3Cross(&slideDir, &m_FloorNormal, &slopeRight);
		D3DXVec3Normalize(&slideDir, &slideDir);
		
		// 必ず下方向へ滑るようにする.
		if (slideDir.y > 0.f)
		{
			slideDir = -slideDir;
		}
		
		// 坂道を滑る力を適用する（急なほど速くなる）.
		float slideForce = (1.0f - slopeAngle) * GRAVITY * 2.0f;
		m_Velocity.x += slideDir.x * slideForce;
		m_Velocity.z += slideDir.z * slideForce;
		m_Inertia.x += slideDir.x * slideForce * 0.5f;
		m_Inertia.z += slideDir.z * slideForce * 0.5f;
		
		// 坂道から離れないよう、わずかに下向きの速度を与える.
		m_Velocity.y = -0.1f;
	}
	else
	{
		// 空中.
		m_GroundedTimer -= 0.016f;
		
		if (m_GroundedTimer <= 0.f)
		{
			m_IsOnGround = false;
		}
		
		m_Velocity.y -= GRAVITY;

		if (m_vPosition.y <= -55.f)
		{
			m_vPosition.x = 0.f;
			m_vPosition.z = 0.f;
			m_vPosition.y = 160.f;
			m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
			m_Inertia = D3DXVECTOR3(0.f, 0.f, 0.f);
		}
	}

	// 天井との当たり判定.
	if (m_Velocity.y > 0.f)
	{
		float distanceToCeiling = m_CeilingY - headY;

		if (distanceToCeiling < CEILING_BUFFER)
		{
			m_Velocity.y = 0.f;
			m_Inertia.y = 0.f;
		}
	}
}

void CPlayer::ApplyDamage(float damage) {
	if (!m_InvFrame)
	{
		m_Health -= damage;
		m_IsDamaged = true;
		m_InvFrameTimer = 0.0f;
		m_InvFrame = true;
	}
}

void CPlayer::ApplyKnockback(const D3DXVECTOR3& sourcePos, float strength)
{
	D3DXVECTOR3 knockDir = m_vPosition - sourcePos;
	knockDir.y = 0.f;
	if (D3DXVec3LengthSq(&knockDir) > 1e-4f)
	{
		D3DXVec3Normalize(&knockDir, &knockDir);
		m_Velocity += knockDir * strength;
		m_Inertia += knockDir * strength * 0.5f;
	}
}


