#pragma once 
#include "CGameObject.h"

class CCamera
	:public CGameObject
{
public:
	CCamera();
	~CCamera();

	// CGameObject を介して継承されました
	void Update() override;

	void Draw(SCENE_DATA& sceneData) override;

	// レンズ設定（FOV/アスペクト/ニア/ファー）
	void SetPerspective(float fovY, float aspect, float zn, float zf);
	// リサイズ時などにアスペクトだけ更新
	void SetAspect(float aspect) { m_Aspect = aspect; }

	void SetLens(float fovY, float aspect, float zn, float zf);

	void LookAt(const D3DXVECTOR3& target);

	void Strafe(float distance);
	void Walk(float distance);

	void OffsetRotX(D3DXVECTOR3 pivot, float angle);
	void OffsetRotY(D3DXVECTOR3 pivot, float angle);
	void ResetCameraRot();

	void Pitch(float pitch);
	void Yaw(float yaw);

	void UpdateViewMatrix(D3DXMATRIX& mView, D3DXMATRIX& mProj);

	D3DXVECTOR3 GetForward() const { return m_vLook; }
	D3DXVECTOR3 GetUp()      const { return m_vUp; }
	D3DXVECTOR3 GetRight()   const { return m_vRight; }
	float GetYaw()    const { return m_Yaw; }
	float GetPitch()  const { return m_Pitch; }
	float GetFovY()   const { return m_FovY; }
	float GetAspect() const { return m_Aspect; }
	float GetNearZ()  const { return m_NearZ; }
	float GetFarZ()   const { return m_FarZ; }

	void DisableStatic() { m_bStaticCamera = false; }

private:

	D3DXVECTOR3 m_vLook;
	D3DXVECTOR3 m_vUp;
	D3DXVECTOR3	m_vRight;

	D3DXVECTOR3 m_vStaticCamTarget;
	bool m_bStaticCamera;


	float m_Yaw;
	float m_Pitch;
	float m_FovY;
	float m_Aspect;
	float m_NearZ;
	float m_FarZ;
	float m_NearWindowHeight;
	float m_FarWindowHeight;
};