#include "CPlayer.h"

static constexpr float GRAVITY = 0.0098f;
static constexpr float FRICTION = 0.090f;
static constexpr float SLIDE_FRICTION = 0.020f;

static constexpr float HEALTH_MAX = 100.0f;

static constexpr float PLAYERSIZE = 2.0f;
static constexpr float CROUCHSIZE = 1.0f;

static constexpr float CROUCH_SPEED = 0.1f;
static constexpr float WALK_SPEED = 0.2f;

static constexpr float JUMP_STRENGTH = 0.18f;

static constexpr float SHOOT_COOLDOWN = 0.5f; // seconds

static constexpr float DASH_SPEED = 0.5f;
static constexpr float DASH_DISTANCE = 1.5f;
static constexpr float DASH_DURATION = 0.03f; // seconds

CPlayer::CPlayer()
	: CCharacter()
	, m_State(Idle)
	, m_MoveSpeed(WALK_SPEED)
	, m_JumpStrength(JUMP_STRENGTH)
	, m_Health(HEALTH_MAX)
	, m_currWeapon(0)
	, m_Forward(0.f, 0.f, 1.f)
	, m_Right(1.f, 0.f, 0.f)
	, m_Velocity(0.f, 0.f, 0.f)
	, m_Acceleration(0.f, 0.f, 0.f)
	, m_Inertia(0.f, 0.f, 0.f)
	, m_pInputHandler(nullptr)
	, m_IsOnGround(true)
	, m_CanShoot(true)
	, m_IsJumping(false)
	, m_IsDashing(false)
	, m_ShootCooldownTimer(0.f)
	, m_DashTimer(0.f)
	, m_FloorY(0.f)
	, m_Height(PLAYERSIZE)
	, m_IsCrouching(false)
	, m_DashDirection(0.f, 0.f, 1.f)
	, m_IsSliding(false)
{
	m_pInputHandler = new CInput();
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update()
{
	m_pInputHandler->Update();
	HandleInput();

	if (!m_IsCrouching)
	{
		m_Height = PLAYERSIZE;
	}
	else
	{
		m_Height = CROUCHSIZE;
	}

	HandleJumpPhys();
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

	D3DXVECTOR3 vecVel = D3DXVECTOR3(m_Inertia.x, m_Velocity.y, m_Inertia.z );

	if (m_pInputHandler->GetKey('W'))
	{
		vecVel += m_Forward * m_MoveSpeed;
		m_DashDirection = GetForwardVector();

	}
	//else vecVel += m_Forward * 0;

	if (m_pInputHandler->GetKey('S'))
	{
		vecVel += -m_Forward * m_MoveSpeed;
		m_DashDirection = -GetForwardVector();

	}
	//else vecVel += m_Forward * 0;

	if (m_pInputHandler->GetKey('A'))
	{
		vecVel += -m_Right * m_MoveSpeed;
		m_DashDirection = -GetRightVector();

	}
	//else vecVel += m_Right * 0;

	if (m_pInputHandler->GetKey('D'))
	{
		vecVel += m_Right * m_MoveSpeed;
		m_DashDirection = GetRightVector();

	}
	//else vecVel += m_Right * 0;

	float length = D3DXVec3Length(&vecVel);
	
	if (length <= 0.f)
	{
		m_State = Idle;
	}
	else if ( !m_IsJumping )
	{
		m_State = Walking;
	}
	m_Velocity = vecVel;

	if (m_pInputHandler->GetKeyDown(VK_LBUTTON))
	{
		Shoot();
	}

	if (m_pInputHandler->GetKeyDown(VK_SPACE))
	{
		Jump();
	}

	//if (m_pInputHandler->GetKeyDown('Q'))
	//{
	//	NextWeapon();
	//}

	//if (m_pInputHandler->GetKeyDown('E'))
	//{
	//	NextWeapon();
	//}

	if (m_pInputHandler->GetKeyDown(VK_SHIFT))
	{
		Dash();
	}

	if (m_pInputHandler->GetKeyDown(VK_CONTROL))
	{
		Crouch();
	}
	else
	{
		m_IsCrouching = false;
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

void CPlayer::ApplyForce(const D3DXVECTOR3& force)
{

	m_Acceleration += force;

}

void CPlayer::Move()
{
	m_vPosition += m_Velocity ;

	//レイの位置をプレイヤーの座標にそろえる
	m_pRayY->Position = m_vPosition;
	//地面めり込み回避のためプレイヤーの位置よりも少し上にしておく
	m_pRayY->Position.y = m_vPosition.y - 0.2f;
	m_pRayY->RotationY += m_vRotation.y;

	//十字（前後左右に伸ばした）レイの設定	
	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		m_pCrossRay->Ray[dir].Position = m_vPosition;
		m_pCrossRay->Ray[dir].Position.y = m_FloorY + 0.1f;
		m_pCrossRay->Ray[dir].RotationY += m_vRotation.y;
	}

}

void CPlayer::Shoot()
{

}

void CPlayer::Jump()
{

	if (IsGrounded())
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

	D3DXVECTOR3 dashDirection = m_DashDirection;
	D3DXVec3Normalize(&dashDirection, &dashDirection);

	m_Velocity += dashDirection * DASH_SPEED;
	m_Inertia = dashDirection * DASH_SPEED;
	
}

void CPlayer::Crouch()
{
}

void CPlayer::CalculateInertia()
{
	m_Inertia = (m_Inertia - (m_Inertia * FRICTION));
	if (m_Inertia.x < 0.01f && m_Inertia.x > -0.01f)
	{
		m_Inertia.x = 0.f;
	}

	if (m_Inertia.z < 0.01f && m_Inertia.z > -0.01f)
	{
		m_Inertia.z = 0.f;
	}
}

void CPlayer::HandleJumpPhys()
{
	float WSPACE = 0.1f;	//
	if (m_vPosition.y < m_FloorY + PLAYERSIZE)
	{
		m_vPosition.y = m_FloorY + PLAYERSIZE;
		m_Velocity.y = 0.f;
	}
	
	if (abs(m_vPosition.y - (m_FloorY + PLAYERSIZE)) <= WSPACE)
	{
		m_IsOnGround = true;
	}
	else
	{
		m_IsOnGround = false;
	}

 	if (IsGrounded())
	{

		if (m_IsJumping && m_Velocity.y <= 0.00f)
		{
			m_IsJumping = false;
		}

	}
	else
	{
		m_Velocity.y -= GRAVITY;
	}
}
