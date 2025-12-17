#include "CPlayer.h"

static constexpr float GRAVITY = 0.0128f;
static constexpr float FRICTION = 0.060f;


static constexpr float HEALTH_MAX = 100.f;

static constexpr float PLAYERSIZE = 2.0f;
static constexpr float CROUCHSIZE = 0.7f;

static constexpr float RUN_SPEED = 0.2f;
static constexpr float MAX_RUN_SPEED = 0.5f;

static constexpr float CROUCH_SPEED = 0.035f;

static constexpr float JUMP_STRENGTH = 0.65f;

static constexpr float SHOOT_COOLDOWN = 0.15f; // seconds

static constexpr float DASH_SPEED = 0.8f;
static constexpr float DASH_DISTANCE = 2.5f;
static constexpr float DASH_COOLDOWN = 0.05f; // seconds
static constexpr float DASH_MAX = 3; // seconds

static constexpr float SLIDE_FRICTION = 0.020f;
static constexpr float SLIDE_START_SPEED = 0.3f;

CPlayer::CPlayer()
	: CCharacter()
	
	, m_pInputHandler(nullptr)
	, m_State(Idle)
	, m_Height(PLAYERSIZE)
	, m_MoveSpeed(RUN_SPEED)
	, m_JumpStrength(JUMP_STRENGTH)
	, m_Health(HEALTH_MAX)
	
	, m_DashTimer(DASH_MAX)
	
	, m_currWeapon(0)
	, m_ShootCooldownTimer(0.f)
	, m_CanShoot(true)
	
	, m_FloorY(0.f)
	, m_Forward(0.f, 0.f, 1.f)
	, m_Right(1.f, 0.f, 0.f)
	, m_Velocity(0.f, 0.f, 0.f)
	, m_Acceleration(0.f, 0.f, 0.f)
	, m_Inertia(0.f, 0.f, 0.f)
	, m_DashDirection(0.f, 0.f, 1.f)

	, m_IsOnGround(true)
	, m_IsJumping(false)
	, m_IsDashing(false)
	, m_IsCrouching(false)
	, m_IsSliding(false)

{
	m_pInputHandler = new CInput();
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update()
{
	m_Shot = false;

	if(m_DashTimer < DASH_MAX)
		m_DashTimer += 0.006f; 

	if (!m_CanShoot && (m_ShootCooldownTimer < SHOOT_COOLDOWN))
	{
		m_CanShoot = false;
		m_ShootCooldownTimer += 0.016f;
	}
	else
	{
		m_CanShoot = true;
		m_ShootCooldownTimer = 0.f;
	}

	m_pInputHandler->Update();
	HandleInput();

	if (!m_IsSliding)
	{
		m_Height = PLAYERSIZE;
		m_MoveSpeed = RUN_SPEED;
	}

	HandleAirPhys();
	CalculateInertia();
	Move();
}


void CPlayer::Draw(SCENE_DATA& sceneData)
{
}

void CPlayer::HandleInput()
{

	CalculateVectors();
	m_DashDirection = GetForwardVector();

	D3DXVECTOR3 inputVel = D3DXVECTOR3(0.f, m_Velocity.y, 0.f );
	bool hasInput = false;

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

	inputVel.x += m_Inertia.x;
	inputVel.z += m_Inertia.z;

	m_Velocity = inputVel;

	if (m_pInputHandler->GetKeyDown(VK_LBUTTON))
	{
		Shoot();
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

	if (m_pInputHandler->GetKeyDown(VK_SHIFT))
	{
		Dash();
	}

	if (m_pInputHandler->GetKey(VK_CONTROL))
	{
		Slide();
	}
	else
	{
		m_IsSliding = false;
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
		float length = D3DXVec3Length(&m_Velocity);
		if (length > MAX_RUN_SPEED)
		{
			D3DXVec3Normalize(&m_Velocity, &m_Velocity);
			m_Velocity = m_Velocity * MAX_RUN_SPEED;
		}
	}

	m_vPosition += m_Velocity ;
	m_Inertia += m_Velocity * 0.015f;

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

	if (m_IsOnGround && !m_IsJumping)
	{
		m_State = Jumping;
		m_Velocity.y = m_JumpStrength;
		m_IsOnGround = false;
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

	D3DXVECTOR3 dashDirection = m_DashDirection;
	D3DXVec3Normalize(&dashDirection, &dashDirection);

	m_IsDashing = true;
	m_State = Dashing;

	m_Velocity += dashDirection * DASH_SPEED;
	m_Inertia += dashDirection * DASH_SPEED;
	
}

void CPlayer::Slide()
{

	float vel = D3DXVec3Length(&m_Velocity);

	if (vel < SLIDE_START_SPEED && !m_IsSliding)
	{
		return;
	}

	if(vel < 0.08f)
	{
		m_IsSliding = false;
		return;
	}

	m_IsSliding = true;
	m_MoveSpeed = CROUCH_SPEED;
	m_Height = CROUCHSIZE;
	m_State = Sliding;

}

void CPlayer::CalculateInertia()
{

	if( m_IsSliding )
	{
		m_Inertia = (m_Inertia - (m_Inertia * SLIDE_FRICTION));
	}
	else
	{
		m_Inertia = (m_Inertia - (m_Inertia * FRICTION));

	}

	
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
	const float GROUND_SNAP_DISTANCE = 0.09f;
	const float GROUND_PENETRATION = 0.05f;

	float distanceToFloor = feetY - m_FloorY;

	if (distanceToFloor >= -GROUND_PENETRATION && distanceToFloor <= GROUND_SNAP_DISTANCE && m_Velocity.y <= 0.f)
	{
		// We're on the ground
		m_vPosition.y = m_FloorY + m_Height;
		m_IsOnGround = true;
		m_Velocity.y = 0.f;
		m_Inertia.y = 0.f;

		// End jump when we touch ground going down
		if (m_IsJumping && m_Velocity.y <= 0.0f)
		{
			m_IsJumping = false;
		}
	}
	else if (distanceToFloor < -GROUND_PENETRATION)
	{
		// Player somehow went below ground - force correction
		m_vPosition.y = m_FloorY + m_Height;
		m_IsOnGround = true;
		m_Velocity.y = 0.f;
		m_Inertia.y = 0.f;
		m_IsJumping = false;
	}
	else
	{
		// Player is in the air
		m_IsOnGround = false;
		m_Velocity.y -= GRAVITY;
		if (m_vPosition.y <= -55.f)
		{
			m_vPosition.x = 0.f;
			m_vPosition.z = 0.f;
			m_vPosition.y = 160.f;
		}
	}
}

void CPlayer::UpdateAxis()
{
	//レイの位置をプレイヤーの座標にそろえる
	m_pRayY->Position = m_vPosition;
	//地面めり込み回避のためプレイヤーの位置よりも少し上にしておく
	m_pRayY->Position.y = m_vPosition.y - m_Height*0.00f;
	m_pRayY->RotationY = m_vRotation.y;
	m_pRayY->Length = m_Height + 0.5f;

	UpdateCrossRay();
}

void CPlayer::UpdateCrossRay()
{
	D3DXVECTOR3 horizontalVel(m_Velocity.x, 0.f, m_Velocity.z);
	float speed = D3DXVec3Length(&horizontalVel);

	// Ray length = base distance + velocity buffer
	float rayLength = 0.5f + (speed * 5.0f);

	// Update all collision rays
	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		m_pCrossRay->Ray[dir].Position = m_vPosition;
		m_pCrossRay->Ray[dir].Position.y = m_vPosition.y - m_Height * 0.65f;
		m_pCrossRay->Ray[dir].RotationY = m_vRotation.y;
		m_pCrossRay->Ray[dir].Length = rayLength;  // Dynamic length!
	}
}
