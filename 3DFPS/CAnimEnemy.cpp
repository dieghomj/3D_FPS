#include "CAnimEnemy.h"

CAnimEnemy::CAnimEnemy()
	: CAnimCharacter()
	, m_PlayerPos(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
	, m_IsAlive(true)
	, m_State(Spawning)
	, m_FloorY(0.0f)
	, m_FloorNormal(D3DXVECTOR3(0.0f, 1.0f, 0.0f))
	, m_pHeadCrossRay(nullptr)
	, m_Health(100.0f)
	, m_RotationSpeed(0.9f)
{
}

CAnimEnemy::~CAnimEnemy()
{
}

void CAnimEnemy::InitEnemy()
{
	m_Radius = 0.5f;
	m_FloorY = 0.0f;
	m_FloorNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	// 頭部の衝突レイを初期化
	if (!m_pHeadCrossRay)
	{
		m_pHeadCrossRay = new CROSSRAY();
	}

	// クロスレイを更新
	UpdateCrossRay();
}

void CAnimEnemy::Update()
{
	if(IsActive() == false)
	{
		return;
	}

	if(IsDead())
	{
		CAnimCharacter::Update();
		return;
	}

	D3DXVECTOR3 pos = GetPosition();
	const float GRAVITY = 0.21f;
	const float SNAP_DISTANCE = 0.005f;  // 床にスナップする距離


	// 床との距離を計算
	float distanceToFloor = pos.y - m_FloorY;
	if (distanceToFloor > SNAP_DISTANCE)
	{

		if (m_FloorY <= -FLT_MAX)
		{
			m_FloorY = 0.0f;
		}

		m_IsGrounded = false;
		// 空中にいる場合は重力を適用
		if(m_GravityEnabled)
			pos.y -= GRAVITY;
	
		if (pos.y < m_FloorY)
		{
			pos.y = m_FloorY;
		}
	}
	else
	{
		// 床に近い場合は床にスナップ（ランプを上る）
		pos.y = m_FloorY;
		m_IsGrounded = true;
	}

	m_vPosition = pos;

	if (m_State == Launched)
	{
		m_vPosition += m_LaunchVelocity;
		if( m_IsGrounded )
		{
			m_State = Idle;
			m_LaunchVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
	}

	//レイの位置をプレイヤーの座標にそろえる
	m_pRayY->Position = m_vPosition;
	//地面めり込み回避のためプレイヤーの位置よりも少し上にしておく
	m_pRayY->Position.y = m_vPosition.y + 0.45;
	m_pRayY->RotationY = m_vRotation.y;
	m_pRayY->Length = 5.5f;

	// クロスレイを更新
	UpdateCrossRay();

	CAnimCharacter::Update();
}

void CAnimEnemy::UpdateCrossRay()
{
	if (!m_pCrossRay || !m_pHeadCrossRay) return;


	D3DXVECTOR3 pos = GetPosition() ;
	D3DXVECTOR3 headPos = pos; 
	pos.y += m_vPosition.y + 0.01;  // 足元の高さに調整
	headPos.y += m_vPosition.y - 0.1f;  // 頭部の高さに調整
	float rayLength = 2.5f;  // 半径より少し長く

	for(int i = 0; i < CROSSRAY::max; ++i)
	{
		m_pCrossRay->Ray[i].Position = pos;
		m_pCrossRay->Ray[i].Length = rayLength;
		m_pHeadCrossRay->Ray[i].Position = headPos;
		m_pHeadCrossRay->Ray[i].Length = rayLength;
	}

}

void CAnimEnemy::LaunchAtPlayer(float speed )
{

	m_State = Launched;
	D3DXVECTOR3 direction = m_PlayerPos - GetPosition();
	D3DXVec3Normalize(&direction, &direction);
	D3DXVECTOR3 velocity = direction * speed;
	D3DXVECTOR3 pos = GetPosition();
	m_LaunchVelocity = velocity;
	pos += velocity;
	SetPosition(pos);

}


void CAnimEnemy::Draw(SCENE_DATA& sceneData)
{
	if (IsActive() == false)
	{
		return;
	}
	CAnimCharacter::Draw(sceneData);
}

void CAnimEnemy::FacePlayer(float& distance, bool invert)
{
	D3DXVECTOR3 toPlayer = m_PlayerPos - GetPosition();
	distance = D3DXVec3Length(&toPlayer);
	D3DXVECTOR3 prevFwd = m_vForward;
	m_vForward = m_PlayerPos - GetPosition();
	m_vForward.y = 0.0f;

	float angle = (atan2f(m_vForward.x, m_vForward.z) - atan2f(prevFwd.x, prevFwd.z));

	if(invert)
		SetRotation(0.0f, D3DXToRadian(180.f) + GetRotation().y + angle , 0.0f);
	else
		SetRotation(0.0f, GetRotation().y + angle, 0.0f);
}
