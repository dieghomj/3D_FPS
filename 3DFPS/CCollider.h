#pragma once
#include "CDirectX11.h"

class CStaticMesh;
class CBoundingSphere;
class CBoundingCube;
class CDebugColliderRender;

class CCollider
{
public:

	enum COLLIDER_SHAPE
	{
		COLLIDER_SHAPE_SPHERE,
		COLLIDER_SHAPE_BOX,
		COLLIDER_SHAPE_MESH,
		COLLIDER_SHAPE_MAX
	};

public:
	CCollider();
	~CCollider();

	HRESULT Init();

	HRESULT CreateSphereForMesh(const CStaticMesh& pMesh);
	HRESULT CreateBoxForMesh(const CStaticMesh& pMesh);
	HRESULT CreateColMesh(); // Mesh collider creation not implemented

	void UpdateCollider() { SetPosition(m_vPosition); SetRotation(m_vRotation); SetScale(m_vScale);  }

	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetScale(const D3DXVECTOR3& scale);

	CBoundingSphere* GetBSphere();
	CBoundingCube* GetBBox();

	void Release();
	void DebugDraw(CDirectX11& pDx11, D3DXMATRIX& mView, D3DXMATRIX& mProj);

private:

	// 位置/回転/スケールからワールド境界を再計算
	void RecalculateWorldBounds();

private:

	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vRotation;
	D3DXVECTOR3	m_vScale;

	D3DXVECTOR3 m_SphereLocalCenter = D3DXVECTOR3(0, 0, 0);
	float m_SphereLocalRadius = 0.0f; 
	D3DXVECTOR3 m_BoxLocalMin = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 m_BoxLocalMax = D3DXVECTOR3(0, 0, 0);

	CBoundingSphere* m_pBSphere;
	CBoundingCube* m_pBCube;

	CDebugColliderRender*	m_pDbgCollider;	
	COLLIDER_SHAPE			m_Shape = COLLIDER_SHAPE_SPHERE;


};

