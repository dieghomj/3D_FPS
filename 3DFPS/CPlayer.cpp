#include "CPlayer.h"

static constexpr float GRAVITY = 0.0198f;
static constexpr float FRICTION = 0.0550f;


static constexpr float HEALTH_MAX = 100.f;

static constexpr float PLAYERSIZE = 4.5;
static constexpr float CROUCHSIZE = 1.6f;
static constexpr float PLAYERRADIUS = 0.5f;

static constexpr float RUN_SPEED = 0.45f;
static constexpr float MAX_RUN_SPEED = 0.8f;

static constexpr float CROUCH_SPEED = 0.027f;

static constexpr float JUMP_STRENGTH = 0.58f;

static constexpr float PISTOL_CD = 0.15f; // seconds
static constexpr float SHOTGUN_CD = 0.15f; // seconds

static constexpr float DASH_SPEED = 2.5f;
static constexpr float DASH_DISTANCE = 5.5f;
static constexpr float DASH_COOLDOWN = 0.05f; // seconds
static constexpr float DASH_MAX = 3.f; // seconds

static constexpr float SLIDE_FRICTION = 0.009f;
static constexpr float SLIDE_START_SPEED = 0.3f;

CPlayer::CPlayer()
	: CCharacter()
	, m_pInputHandler			(nullptr)
	
	, m_State					(Idle)
	
	, m_Height					(PLAYERSIZE)
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
	, m_Velocity				(0.f, 0.f, 0.f)
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
	, m_IsGravityEnabled		(true)

	, m_IsInertiaEnabled(true)
	, m_InvFrame(false)
	, m_InvFrameTimer(0.f)
{
	m_pInputHandler = new CInput();
	m_pHeadCrossRay = new CROSSRAY();
	m_Radius = PLAYERRADIUS;
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
}

void CPlayer::Update()
{
	float weaponCD = 0.f;
	switch (m_currWeapon)
	{
		case 0: // Pistol
			weaponCD = PISTOL_CD;
			break;
		case 1: // Shotgun
			weaponCD = SHOTGUN_CD;
			break;
		case 2: // Empty
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

	m_pInputHandler->Update();
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

	if (m_IsInertiaEnabled)
	{
		inputVel.x += m_Inertia.x;
		inputVel.z += m_Inertia.z;
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
		//float length = D3DXVec3Length(&m_Velocity);
		//if (length > MAX_RUN_SPEED)
		//{
		//	D3DXVec3Normalize(&m_Velocity, &m_Velocity);
		//	m_Velocity = m_Velocity * MAX_RUN_SPEED;
		//}
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

	m_IsDashing = true;
	m_State = Dashing;

	{

		D3DXVECTOR3 dashDirection = m_DashDirection;
		D3DXVec3Normalize(&dashDirection, &dashDirection);

		m_Velocity += dashDirection * (RUN_SPEED + DASH_SPEED);
		m_Inertia += dashDirection * (RUN_SPEED + DASH_SPEED) * 0.5f;
	}

}

void CPlayer::Slide()
{

	D3DXVECTOR3 horizontalVel = D3DXVECTOR3( m_Velocity.x, 0.f, m_Velocity.z);
	float vel = D3DXVec3Length(&m_Velocity);

	if (vel < SLIDE_START_SPEED && !m_IsSliding)
	{
		return;
	}

	if(vel < 0.1f)
	{
		m_IsSliding = false;
		m_Inertia.x = 0.f;
		m_Inertia.z = 0.f;
		return;
	}

	m_IsSliding = true;
	m_MoveSpeed = CROUCH_SPEED;
	m_Height = CROUCHSIZE;
	m_State = Sliding;

}

void CPlayer::SpecialAction()
{



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
	float feetY = m_vPosition.y - m_Height;			//プレイヤーの足元のY座標
	float headY = m_vPosition.y + m_Height * 0.3f;	//プレイヤーの頭のY座標

	const float GROUND_SNAP_DISTANCE = 0.015f;		//地面に吸着する距離
	const float GROUND_PENETRATION = 0.01f;			//地面にめり込んだと見なす距離
	const float CEILING_BUFFER = 0.01f;				//天井に当たったと見なす距離

	float distanceToFloor = feetY - m_FloorY;		//プレイヤーの足元と地面の距離


	if (distanceToFloor >= -GROUND_PENETRATION &&
		fabsf(distanceToFloor) <= GROUND_SNAP_DISTANCE &&
		m_Velocity.y <= 0.f)
	{
		// On ground - snap to floor
		m_vPosition.y = m_FloorY + m_Height;
		m_IsOnGround = true;
		m_Velocity.y = 0.f;
		m_Inertia.y = 0.f;

		// End jump when touching ground while falling
		if (m_IsJumping && m_Velocity.y <= 0.0f)
		{
			m_IsJumping = false;
		}
	}
	//地面にめり込んだと見なす距離よりも下にある場合
	else if (distanceToFloor < -GROUND_PENETRATION)
	{
		m_vPosition.y = m_FloorY + m_Height;
		m_IsOnGround = true;
		m_Velocity.y = 0.f;
		m_Inertia.y = 0.f;
		m_IsJumping = false;
	}
	else
	{
		//空中にいる場合の処理
		m_IsOnGround = false;
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

	//天井判定
	if (m_Velocity.y > 0.f)  
	{
		float distanceToCeiling = m_CeilingY - headY;

		if (distanceToCeiling < CEILING_BUFFER)
		{
			m_vPosition.y = m_CeilingY - m_Height - 0.0003f; 
			m_Velocity.y = 0.f;
			m_Inertia.y = 0.f;
			m_IsJumping = false;
		}
	}
}

void CPlayer::ApplyDamage(float damage) {
	if (!m_InvFrame)
	{
		m_Health -= damage;
		m_InvFrameTimer = 0.0f;
		m_InvFrame = true;
	}
}

void CPlayer::UpdateAxis()
{
	//レイの位置をプレイヤーの座標にそろえる
	m_pRayY->Position = m_vPosition;
	//地面めり込み回避のためプレイヤーの位置よりも少し上にしておく
	m_pRayY->Position.y = m_vPosition.y - m_Height*0.001f;
	m_pRayY->RotationY = m_vRotation.y;
	m_pRayY->Length = m_Height + 0.9f;

	UpdateCrossRay();
}

void CPlayer::UpdateCrossRay()
{
	// 水平方向の速度ベクトルを取得
	D3DXVECTOR3 horizontalVel(m_Velocity.x, 0.f, m_Velocity.z);
	float speed = D3DXVec3Length(&horizontalVel);

	// レイの長さを速度に応じて変化させる
	const float RAY_LENGTH = 1.5f;

	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		m_pCrossRay->Ray[dir].Position = m_vPosition;
		m_pCrossRay->Ray[dir].Position.y = m_vPosition.y - m_Height * 0.55f;
		m_pCrossRay->Ray[dir].RotationY = m_vRotation.y;
		m_pCrossRay->Ray[dir].Length = RAY_LENGTH;

		m_pHeadCrossRay->Ray[dir].Position = m_vPosition;
		m_pHeadCrossRay->Ray[dir].Position.y = m_vPosition.y + 0.5f;
		m_pHeadCrossRay->Ray[dir].RotationY = m_vRotation.y;
		m_pHeadCrossRay->Ray[dir].Length = RAY_LENGTH;
	}
}
