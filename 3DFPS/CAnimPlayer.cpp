#include "stdafx.h"
#include "CAnimPlayer.h"

CAnimPlayer::CAnimPlayer()
	: CAnimCharacter()
	, m_vVelocity	(0.f, 0.f, 0.f)
	, m_PlayerHealth(100.f)
	, m_TurnSpeed	(0.05f)
	, m_MoveSpeed	(0.06f)
	, m_MoveState	(Stop)
	, m_PlayerState (Idle)
	, m_pInput		(nullptr)
	, m_bTankControlMode(true)
	, m_AnimationState(AnimIdle)
	, m_bIsFlashOn(false)
	, m_vDirection(0.f, 0.f, 1.f)
{
	m_pInput = new CInput();
}

CAnimPlayer::~CAnimPlayer()
{
	SAFE_DELETE(m_pInput);
}

void CAnimPlayer::Update()
{
	bool animFin = false;
	if (m_PlayerState == Damaged)
	{
		animFin = SetAnimNo(m_AnimationState, FORCE_CHANGE);
		if (animFin)
			m_PlayerState = Idle;
	}
	else
	if (m_PlayerState == Attacking)
	{
		animFin = SetAnimNo(m_AnimationState, FORCE_CHANGE);
		if (animFin)
			m_PlayerState = Idle;
	}
	else
	SetAnimNo(m_AnimationState);

	HandleInput();

	if (!IsFlashOn())
	{
		m_LightIntensity += 0.001f;
		if (m_LightIntensity > MAX_LIGHT_INT)
			m_LightIntensity = MAX_LIGHT_INT;
	}
	else
	{
		m_LightIntensity -= 0.005f;
		if (m_LightIntensity < 0.f)
			m_LightIntensity = 0.f;
	}

	//レイの位置をプレイヤーの座標にそろえる
	m_pRayY->Position = m_vPosition;
	//地面めり込み回避のためプレイヤーの位置よりも少し上にしておく
	m_pRayY->Position.y += 0.2f;
	m_pRayY->RotationY = m_vRotation.y;

	//十字（前後左右に伸ばした）レイの設定	
	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		m_pCrossRay->Ray[dir].Position = m_vPosition;
		m_pCrossRay->Ray[dir].Position.y += 0.1f;
		m_pCrossRay->Ray[dir].RotationY = m_vRotation.y;
	}

	CAnimCharacter::Update();
	
}

void CAnimPlayer::Draw(SCENE_DATA& sceneData)
{
	CAnimCharacter::Draw(sceneData);
}

void CAnimPlayer::ApplyDamage(float damage)
{
	m_PlayerHealth -= damage;
	if (m_PlayerHealth < 0.f)
		m_PlayerHealth = 0.f;
	m_PlayerState = Damaged;
}

void CAnimPlayer::ApplyHeal(float heal)
{
	m_PlayerHealth += heal;
	if (m_PlayerHealth > 100.f)
		m_PlayerHealth = 100.f;
}

void CAnimPlayer::ApplyLightEffect(float amount)
{
	m_LightIntensity += amount;
}

void CAnimPlayer::AnimControl()
{

	switch (m_PlayerState) {
	
		case Idle:
			//待機
			m_AnimationState = AnimIdle;
			break;
		case Attacking:
			//攻撃
			m_AnimationState = AnimAttack;
			break;
		case Running:
			//走る
			m_AnimationState = AnimRun;
			break;
		case Jumping:
			//ジャンプ
			m_AnimationState = AnimJump;
			break;
		case Damaged:
			//ダメージ
			m_AnimationState = AnimDamaged;
			break;
	}
	
}

void CAnimPlayer::RadioControl()
{
	//Z軸ベクトル(Z+方向への単位ベクトル)
	//※大きさ（長さ）が１のベクトルを単位ベクトルという
	D3DXVECTOR3 vecAxisZ(0.f, 0.f, 1.f);
	D3DXMATRIX mRotY;

	D3DXMatrixRotationY(
		&mRotY,
		m_vRotation.y);
	D3DXVec3TransformCoord(
		&m_vDirection,
		&vecAxisZ,
		&mRotY);

	D3DXVECTOR3 vecAxisX(1.f, 0.f, 0.f);

	//Y方向の回転行列
	D3DXMATRIX mRotationY;
	//Y軸回転行列を作成
	D3DXMatrixRotationY(
		&mRotationY,		//(out)行列
		m_vRotation.y);		//プレイヤーのY方向の回転値

	//Y軸回転行列を使ってZ軸ベクトルを座標変換する
	D3DXVec3TransformCoord(
		&vecAxisZ,		//(out)Z軸ベクトル
		&vecAxisZ,		//(in)Z軸ベクトル
		&mRotationY);	//Y軸回転行列

	D3DXVec3TransformCoord(
		&vecAxisX,		//(out)Z軸ベクトル
		&vecAxisX,		//(in)Z軸ベクトル
		&mRotationY);	//Y軸回転行列

	//移動状態によって処理を分ける
	switch (m_MoveState) {
		case MoveState::Forward:	//前進
			m_vPosition += vecAxisZ * m_MoveSpeed;
			break;
		case MoveState::Backward:	//後退
			m_vPosition -= vecAxisZ * m_MoveSpeed;
			break;
		case MoveState::Left:	//左移動
			m_vPosition -= vecAxisX * m_MoveSpeed;
			break;
		case MoveState::Right:	//右移動
			m_vPosition += vecAxisX * m_MoveSpeed;
			break;
		case MoveState::Up:	//上昇
			m_vPosition.y += m_MoveSpeed;
			break;
		case MoveState::Down:	//下降
			m_vPosition.y -= m_MoveSpeed;
 			break;
		default:
			break;
	}

	if (m_MoveState != MoveState::Stop)
	{
		SetAnimSpeed(0.04f);
		m_PlayerState = Running;
	}
	else
	{
		if ((m_PlayerState != Attacking) && (m_PlayerState != Jumping))
		{
			m_PlayerState = Idle;
			SetAnimSpeed(0.01f);
		}
	}

	//上記の移動処理が終われば停止状態にしておく
	m_MoveState = MoveState::Stop;
}

void CAnimPlayer::HandleInput()
{ 
	m_pInput->Update();

	if (m_pInput->GetKeyUp(VK_LBUTTON))
	{
		m_bIsFlashOn = !m_bIsFlashOn;
	}
	if (m_pInput->GetKeyDown(VK_UP) || m_pInput->GetKeyDown('W')) {
		m_MoveState = MoveState::Forward;
	}
	//後退
	if (m_pInput->GetKeyDown(VK_DOWN) || m_pInput->GetKeyDown('S')) {
		m_MoveState = MoveState::Backward;
	}
	if (m_pInput->GetKeyDown(VK_LEFT) || m_pInput->GetKeyDown('A')) {
		if(m_bTankControlMode)
			m_vRotation.y -= m_TurnSpeed;
		else
			m_MoveState = MoveState::Left;
	}
	if (m_pInput->GetKeyDown(VK_RIGHT) || m_pInput->GetKeyDown('D')) {
		if (m_bTankControlMode)
			m_vRotation.y += m_TurnSpeed;
		else
			m_MoveState = MoveState::Right;
	}
	if (m_pInput->GetKeyDown(VK_SPACE)) {
		m_MoveState = MoveState::Up;
	}
	if (m_pInput->GetKeyDown(VK_LCONTROL)) {
		m_MoveState = MoveState::Down;
	}

	RadioControl();
	AnimControl();
}




