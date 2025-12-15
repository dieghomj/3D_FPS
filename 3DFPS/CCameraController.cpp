#include "stdafx.h"
#include "CCameraController.h"

CCameraController::CCameraController()
	: m_pCamera		()
	, m_vPosition	(0.f, 0.f, 0.f)
	, m_vRotation	(0.f, 0.f, 0.f)
	, m_FP_offset	(0.f, 0.f, 0.f)
	, m_TP_offset	(0.f, 1.5f, -3.5f)
	, m_distance	(6.f)
{
}

CCameraController::CCameraController(CCamera* camera)
	: m_pCamera		(camera)
	, m_vPosition	(0.f, 0.f, 0.f)
	, m_vRotation	(0.f, 0.f, 0.f)
	, m_FP_offset	(0.f, 0.f, 0.f)
	, m_TP_offset	(0.f, 0.5f, -3.5f)
	, m_distance	(6.f)
	, m_minDistance	(0.5f)
	, m_maxLookUp	(D3DX_PI / 4.f)
	, m_maxLookDown	(-D3DX_PI / 4.f)
{
}

CCameraController::~CCameraController()
{
}

void CCameraController::Update(float deltaTime)
{
	m_vPosition = m_pCamera->GetPosition();
	m_vRotation.x = m_pCamera->GetPitch();
	m_vRotation.y = m_pCamera->GetYaw();
}

//三人称カメラ
void CCameraController::ThirdPersonCamera(
	const D3DXVECTOR3& TargetPos, float distance, POINT delta, float sense)
{
	float yaw = D3DXToRadian((float)delta.x * sense);
	float pitch = D3DXToRadian((float)delta.y * sense);

	//if (m_vRotation.x + pitch > m_maxLookUp) pitch = m_maxLookUp;
	//if (m_vRotation.x + pitch < m_maxLookDown) pitch = m_maxLookDown;

	if (yaw > D3DX_PI) yaw -= D3DX_PI * 2.f;
	if (yaw < -D3DX_PI) yaw += D3DX_PI * 2.f;

	m_pCamera->OffsetRotX(TargetPos, pitch);
	m_pCamera->OffsetRotY(TargetPos, yaw);
	
	D3DXVECTOR3 offsetZ = m_pCamera->GetForward() * m_TP_offset.z;
	D3DXVECTOR3 offsetY = m_pCamera->GetUp() * m_TP_offset.y;
	D3DXVECTOR3 offsetX = m_pCamera->GetRight() * m_TP_offset.x;

	m_pCamera->SetPosition(TargetPos + offsetX + offsetY + offsetZ);
	m_pCamera->SetRotation(pitch, yaw, 0);

}

void CCameraController::FirstPersonCamera(POINT delta, float sense)
{
	float yaw = D3DXToRadian((float)delta.x * sense);
	float pitch = D3DXToRadian((float)delta.y * sense);

	//if (m_vRotation.x + pitch > m_maxLookUp) pitch = m_maxLookUp;
	//if (m_vRotation.x + pitch < m_maxLookDown) pitch = m_maxLookDown;

	m_pCamera->Pitch(pitch);
	m_pCamera->Yaw(yaw);
	m_pCamera->SetRotation(pitch, yaw, 0);
	
	HandleInput();
}

void CCameraController::FirstPersonCamera(CGameObject* target,POINT delta, float sense)
{
	float yaw = D3DXToRadian((float)delta.x * sense);
	float pitch = D3DXToRadian((float)delta.y * sense);

	//if (m_vRotation.x + pitch > m_maxLookUp) pitch = m_maxLookUp;
	//if (m_vRotation.x + pitch < m_maxLookDown) pitch = m_maxLookDown;

	m_pCamera->Yaw(yaw);
	m_pCamera->Pitch(pitch);
	m_pCamera->SetRotation(pitch, yaw, 0);
	m_pCamera->SetPosition(target->GetPosition() + m_FP_offset);

	target->SetRotation(0.f, m_pCamera->GetRotation().y, 0.f);
}

void CCameraController::StaticCamera(const D3DXVECTOR3& TargetPos, POINT delta, float sense)
{

	if (m_pCamera->GetPosition() == TargetPos)
	{
		const D3DXVECTOR3 back(0.f, 0.5f, -5.f);
		m_pCamera->SetPosition(TargetPos + back);
	}

	// 常にターゲットを注視
	m_pCamera->LookAt(TargetPos);

}

void CCameraController::UpdateObjectRotationFromCamera(D3DXVECTOR3* TargetRot)
{
	TargetRot->y += m_pCamera->GetRotation().y;
}

void CCameraController::UpdateOffSet(D3DXVECTOR3* offset)
{
}

void CCameraController::HandleInput()
{

	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
		m_pCamera->Walk(0.3f);
	}
	//後退
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
		m_pCamera->Walk(-0.3f);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) {
		m_pCamera->Strafe(-0.3f);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) {
		m_pCamera->Strafe(0.3f);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		m_pCamera->SetPosition(m_pCamera->GetPosition() + D3DXVECTOR3(0, 0.3f, 0));
	}
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		m_pCamera->SetPosition(m_pCamera->GetPosition() + D3DXVECTOR3(0, -0.3f, 0));
	}

}
