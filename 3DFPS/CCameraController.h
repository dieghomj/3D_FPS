#pragma once
#include "CCamera.h"

class CCameraController
{

	public:
	CCameraController();
	CCameraController(CCamera* camera);

	~CCameraController();

	void Update(float deltaTime);

	void ThirdPersonCamera(
		const D3DXVECTOR3& TargetPos, float distance, POINT delta, float sense);

	void FirstPersonCamera(POINT delta, float sense);
	void FirstPersonCamera(CGameObject* target, POINT delta, float sense);

	void StaticCamera(const D3DXVECTOR3& TargetPos, POINT delta, float sense);

	void UpdateObjectRotationFromCamera(D3DXVECTOR3* TargetRot);
	void UpdateOffSet(D3DXVECTOR3* offset);

	void HandleInput();

	void SetPosition(float x, float y, float z) { m_vPosition = D3DXVECTOR3(x, y, z); };
	void SetRotation(float pitch, float yaw, float roll) { m_vRotation = D3DXVECTOR3(yaw, pitch, roll); };
	void SetTPOffset(float x, float y, float z) { m_TP_offset = D3DXVECTOR3(x, y, z); };
	void SetTPOffset(const D3DXVECTOR3& offset) { m_TP_offset = offset; };
	D3DXVECTOR3 GetPosition() const { return m_vPosition; };
	D3DXVECTOR3 GetRotation() const { return m_vRotation; };
	D3DXVECTOR3 GetForward() const { return m_pCamera->GetForward(); };

private:
	CCamera* m_pCamera;
	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vRotation;
	
	D3DXVECTOR3 m_FP_offset;
	D3DXVECTOR3 m_TP_offset;

	float m_distance;
	float m_minDistance;
	float m_maxLookUp;
	float m_maxLookDown;

};

