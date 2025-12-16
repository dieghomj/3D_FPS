#include "stdafx.h"
#include "CCollider.h"
#include "CBoundingSphere.h"
#include "CBoundingCube.h"
#include "CDebugColliderRender.h"
#include "CStaticMesh.h"


CCollider::CCollider()
	: m_vPosition()
	, m_vRotation()
	, m_vScale(1.0f, 1.0f, 1.0f)
	, m_pBSphere(nullptr)
	, m_pBCube(nullptr)
	, m_pDbgCollider(nullptr)
	, m_Shape(COLLIDER_SHAPE_SPHERE)
{
}

CCollider::~CCollider()
{
}

HRESULT CCollider::Init()
{
	m_pBCube = new CBoundingCube();
	m_pBSphere = new CBoundingSphere();
	m_pDbgCollider = new CDebugColliderRender();
	return S_OK;
}

HRESULT CCollider::CreateSphereForMesh(const CStaticMesh& pMesh)
{
	if (!m_pBSphere && FAILED(Init())) return E_FAIL;
	if (!m_pBSphere) return E_FAIL;

	if (FAILED(m_pBSphere->CreateSphereForMesh(pMesh)))
	{
		return E_FAIL;
	}

	// ローカル球情報を保持
	m_SphereLocalCenter = m_pBSphere->GetPosition();
	m_SphereLocalRadius = m_pBSphere->GetRadius();
	m_Shape = COLLIDER_SHAPE_SPHERE;

	RecalculateWorldBounds();
	return S_OK;
}

HRESULT CCollider::CreateBoxForMesh(const CStaticMesh& pMesh)
{
	if (!m_pBCube && FAILED(Init())) return E_FAIL;
	if (!m_pBCube) return E_FAIL;

	if (FAILED(m_pBCube->CreateCubeForMesh(pMesh)))
	{
		return E_FAIL;
	}

	// Capture local AABB from mesh
	m_BoxLocalMin = m_pBCube->GetMin();
	m_BoxLocalMax = m_pBCube->GetMax();
	m_Shape = COLLIDER_SHAPE_BOX;

	RecalculateWorldBounds();
	return S_OK;
}

HRESULT CCollider::CreateColMesh()
{

	return E_NOTIMPL;

}

void CCollider::SetPosition(const D3DXVECTOR3& pos)
{
	m_vPosition = pos;
	RecalculateWorldBounds();
}

void CCollider::SetRotation(const D3DXVECTOR3& rot)
{
	m_vRotation = rot;
	RecalculateWorldBounds();
}

void CCollider::SetScale(const D3DXVECTOR3& scale)
{
	m_vScale = scale;
	RecalculateWorldBounds();
}

CBoundingSphere* CCollider::GetBSphere()
{
	return m_pBSphere;
}

CBoundingCube* CCollider::GetBBox()
{
	return m_pBCube;
}

void CCollider::RecalculateWorldBounds()
{
	// ワールド行列 = S * R * T
	D3DXMATRIX mS, mR, mT, mWorld;
	D3DXMatrixScaling(&mS, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixRotationYawPitchRoll(&mR, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	D3DXMatrixTranslation(&mT, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	mWorld = mS * mR * mT;

	// 球: 中心は座標変換、半径は最大スケール成分でスケール
	if (m_pBSphere)
	{
		D3DXVECTOR3 worldCenter;
		D3DXVec3TransformCoord(&worldCenter, &m_SphereLocalCenter, &mWorld);
		m_pBSphere->SetPosition(worldCenter);

		const float maxScale = max(max(fabsf(m_vScale.x), fabsf(m_vScale.y)), fabsf(m_vScale.z));
		m_pBSphere->SetRadius(m_SphereLocalRadius * maxScale);
	}

	// 立方(軸平行AABB): ローカルAABBの8頂点をSRTで変換してワールドAABBを再構築
	if (m_pBCube)
	{
		const D3DXVECTOR3& mn = m_BoxLocalMin;
		const D3DXVECTOR3& mx = m_BoxLocalMax;

		D3DXVECTOR3 localCorners[8] = {
			{mn.x, mn.y, mn.z},
			{mx.x, mn.y, mn.z},
			{mn.x, mx.y, mn.z},
			{mx.x, mx.y, mn.z},
			{mn.x, mn.y, mx.z},
			{mx.x, mn.y, mx.z},
			{mn.x, mx.y, mx.z},
			{mx.x, mx.y, mx.z},
		};

		D3DXVECTOR3 wMin(FLT_MAX, FLT_MAX, FLT_MAX);
		D3DXVECTOR3 wMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (int i = 0; i < 8; ++i)
		{
			D3DXVECTOR3 w;
			D3DXVec3TransformCoord(&w, &localCorners[i], &mWorld);
			wMin.x = min(wMin.x, w.x); wMax.x = max(wMax.x, w.x);
			wMin.y = min(wMin.y, w.y); wMax.y = max(wMax.y, w.y);
			wMin.z = min(wMin.z, w.z); wMax.z = max(wMax.z, w.z);
		}

		m_pBCube->SetMinMax(wMin, wMax);
		// 中心（任意）
		m_pBCube->SetPosition((wMin + wMax) * 0.5f);
	}
}


void CCollider::Release()
{
	SAFE_DELETE(m_pBSphere);
	SAFE_DELETE(m_pBCube);
	SAFE_DELETE(m_pDbgCollider);
}

void CCollider::DebugDraw(CDirectX11& pDx11,D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	m_pDbgCollider->DrawCollider(pDx11, mView, mProj, m_Shape, *this);
}

