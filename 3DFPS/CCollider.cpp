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
	D3DXMatrixIdentity(&m_mRotation);
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

HRESULT CCollider::InitDebugRender(CDirectX11& pDx11)
{
	if (!m_pDbgCollider)
	{
		m_pDbgCollider = new CDebugColliderRender();
	}
	return m_pDbgCollider->Init(pDx11);
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

	// メッシュからローカルAABBを取得
	m_BoxLocalMin = m_pBCube->GetMin();
	m_BoxLocalMax = m_pBCube->GetMax();
	m_Shape = COLLIDER_SHAPE_CUBE;

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
	D3DXMATRIX mS, mT, mWorld;
	D3DXMatrixScaling(&mS, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixRotationYawPitchRoll(&m_mRotation, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	D3DXMatrixTranslation(&mT, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	mWorld = mS * m_mRotation * mT;

	// 球: 中心は座標変換、半径は最大スケール成分でスケール
	if (m_pBSphere != nullptr && m_Shape == COLLIDER_SHAPE_SPHERE)
	{
		D3DXVECTOR3 worldCenter;
		D3DXVec3TransformCoord(&worldCenter, &m_SphereLocalCenter, &mWorld);
		m_pBSphere->SetPosition(worldCenter);

		const float maxScale = max(max(fabsf(m_vScale.x), fabsf(m_vScale.y)), fabsf(m_vScale.z));
		m_pBSphere->SetRadius(m_SphereLocalRadius * maxScale);
	}

	// 立方体(OBB): ワールド行列を保存し、AABBも計算
	if (m_pBCube != nullptr && m_Shape == COLLIDER_SHAPE_CUBE)
	{
		// ワールド行列を保存（OBB用）
		m_pBCube->SetWorldMatrix(mWorld);
		m_pBCube->SetLocalMinMax(m_BoxLocalMin, m_BoxLocalMax);

		// AABB計算（ブロードフェーズ用）
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
		m_pBCube->SetPosition((wMin + wMax) * 0.5f);
	}
}

bool CCollider::CheckCollision(CCollider* pOther, D3DXVECTOR3* pPenetration)
{
	if (!pOther) return false;

	COLLIDER_SHAPE shapeA = m_Shape;
	COLLIDER_SHAPE shapeB = pOther->GetShape();

	// 球 vs 球
	if (shapeA == COLLIDER_SHAPE_SPHERE && shapeB == COLLIDER_SHAPE_SPHERE)
	{
		return SphereVsSphere(m_pBSphere, pOther->GetBSphere(), pPenetration);
	}

	// 球 vs ボックス
	if (shapeA == COLLIDER_SHAPE_SPHERE && shapeB == COLLIDER_SHAPE_CUBE)
	{
		return SphereVsOBB(m_pBSphere, pOther->GetBBox(), pOther->GetRotationMatrix(), pPenetration);
	}

	// ボックス vs 球
	if (shapeA == COLLIDER_SHAPE_CUBE && shapeB == COLLIDER_SHAPE_SPHERE)
	{
		bool hit = SphereVsOBB(pOther->GetBSphere(), m_pBCube, m_mRotation, pPenetration);
		if (hit && pPenetration)
		{
			// 押し出し方向を反転
			*pPenetration = -*pPenetration;
		}
		return hit;
	}

	// ボックス vs ボックス（簡易AABB判定）
	if (shapeA == COLLIDER_SHAPE_CUBE && shapeB == COLLIDER_SHAPE_CUBE)
	{
		return m_pBCube->IsHit(*pOther->GetBBox());
	}

	return false;
}

bool CCollider::SphereVsSphere(CBoundingSphere* pSphereA, CBoundingSphere* pSphereB, D3DXVECTOR3* pPenetration)
{
	if (!pSphereA || !pSphereB) return false;

	D3DXVECTOR3 diff = pSphereA->GetPosition() - pSphereB->GetPosition();
	float distance = D3DXVec3Length(&diff);
	float radiusSum = pSphereA->GetRadius() + pSphereB->GetRadius();

	if (distance < radiusSum && distance > 0.0001f)
	{
		if (pPenetration)
		{
			D3DXVECTOR3 dir;
			D3DXVec3Normalize(&dir, &diff);
			*pPenetration = dir * (radiusSum - distance);
		}
		return true;
	}

	return false;
}

bool CCollider::SphereVsOBB(CBoundingSphere* pSphere, CBoundingCube* pBox, 
							const D3DXMATRIX& boxRotation, D3DXVECTOR3* pPenetration)
{
	if (!pSphere || !pBox) return false;

	// OBBの中心と半サイズを取得
	D3DXVECTOR3 boxCenter = pBox->GetCenter();
	D3DXVECTOR3 boxHalfExtents = pBox->GetHalfExtents();

	// スケールを考慮した半サイズ
	const D3DXMATRIX& worldMatrix = pBox->GetWorldMatrix();
	
	// ワールド行列からスケール成分を抽出
	D3DXVECTOR3 scaleX(worldMatrix._11, worldMatrix._12, worldMatrix._13);
	D3DXVECTOR3 scaleY(worldMatrix._21, worldMatrix._22, worldMatrix._23);
	D3DXVECTOR3 scaleZ(worldMatrix._31, worldMatrix._32, worldMatrix._33);
	float sx = D3DXVec3Length(&scaleX);
	float sy = D3DXVec3Length(&scaleY);
	float sz = D3DXVec3Length(&scaleZ);

	D3DXVECTOR3 scaledHalfExtents(boxHalfExtents.x * sx, boxHalfExtents.y * sy, boxHalfExtents.z * sz);

	// OBBの3つの軸を取得（回転行列の各行）
	D3DXVECTOR3 axisX(boxRotation._11, boxRotation._12, boxRotation._13);
	D3DXVECTOR3 axisY(boxRotation._21, boxRotation._22, boxRotation._23);
	D3DXVECTOR3 axisZ(boxRotation._31, boxRotation._32, boxRotation._33);
	D3DXVec3Normalize(&axisX, &axisX);
	D3DXVec3Normalize(&axisY, &axisY);
	D3DXVec3Normalize(&axisZ, &axisZ);

	// 球の中心からボックス中心へのベクトル
	D3DXVECTOR3 sphereCenter = pSphere->GetPosition();
	D3DXVECTOR3 diff = sphereCenter - boxCenter;

	// OBBのローカル座標系での球の位置を計算
	float localX = D3DXVec3Dot(&diff, &axisX);
	float localY = D3DXVec3Dot(&diff, &axisY);
	float localZ = D3DXVec3Dot(&diff, &axisZ);

	// 最近接点をクランプで計算
	float clampedX = max(-scaledHalfExtents.x, min(localX, scaledHalfExtents.x));
	float clampedY = max(-scaledHalfExtents.y, min(localY, scaledHalfExtents.y));
	float clampedZ = max(-scaledHalfExtents.z, min(localZ, scaledHalfExtents.z));

	// 最近接点をワールド座標に変換
	D3DXVECTOR3 closestPoint = boxCenter + 
		axisX * clampedX + 
		axisY * clampedY + 
		axisZ * clampedZ;

	// 球の中心と最近接点の距離
	D3DXVECTOR3 closestDiff = sphereCenter - closestPoint;
	float distance = D3DXVec3Length(&closestDiff);
	float sphereRadius = pSphere->GetRadius();

	if (distance < sphereRadius)
	{
		if (pPenetration)
		{
			if (distance > 0.0001f)
			{
				D3DXVECTOR3 dir;
				D3DXVec3Normalize(&dir, &closestDiff);
				*pPenetration = dir * (sphereRadius - distance);
			}
			else
			{
				// 球がボックス内部にある場合、最も近い面に押し出す
				float distX = scaledHalfExtents.x - fabsf(localX);
				float distY = scaledHalfExtents.y - fabsf(localY);
				float distZ = scaledHalfExtents.z - fabsf(localZ);

				if (distX < distY && distX < distZ)
				{
					float sign = (localX >= 0) ? 1.0f : -1.0f;
					*pPenetration = axisX * sign * (distX + sphereRadius);
				}
				else if (distY < distZ)
				{
					float sign = (localY >= 0) ? 1.0f : -1.0f;
					*pPenetration = axisY * sign * (distY + sphereRadius);
				}
				else
				{
					float sign = (localZ >= 0) ? 1.0f : -1.0f;
					*pPenetration = axisZ * sign * (distZ + sphereRadius);
				}
			}
		}
		return true;
	}

	return false;
}

bool CCollider::SphereVsAABB(CBoundingSphere* pSphere, CBoundingCube* pBox, D3DXVECTOR3* pPenetration)
{
	if (!pSphere || !pBox) return false;

	D3DXVECTOR3 sphereCenter = pSphere->GetPosition();
	float sphereRadius = pSphere->GetRadius();

	D3DXVECTOR3 boxMin = pBox->GetMin();
	D3DXVECTOR3 boxMax = pBox->GetMax();

	// 最近接点を計算
	D3DXVECTOR3 closestPoint;
	closestPoint.x = max(boxMin.x, min(sphereCenter.x, boxMax.x));
	closestPoint.y = max(boxMin.y, min(sphereCenter.y, boxMax.y));
	closestPoint.z = max(boxMin.z, min(sphereCenter.z, boxMax.z));

	D3DXVECTOR3 diff = sphereCenter - closestPoint;
	float distance = D3DXVec3Length(&diff);

	if (distance < sphereRadius)
	{
		if (pPenetration)
		{
			if (distance > 0.0001f)
			{
				D3DXVECTOR3 dir;
				D3DXVec3Normalize(&dir, &diff);
				*pPenetration = dir * (sphereRadius - distance);
			}
			else
			{
				// 球がボックス内部にある場合
				D3DXVECTOR3 boxCenter = (boxMin + boxMax) * 0.5f;
				D3DXVECTOR3 boxHalf = (boxMax - boxMin) * 0.5f;
				D3DXVECTOR3 localPos = sphereCenter - boxCenter;

				float distX = boxHalf.x - fabsf(localPos.x);
				float distY = boxHalf.y - fabsf(localPos.y);
				float distZ = boxHalf.z - fabsf(localPos.z);

				if (distX < distY && distX < distZ)
				{
					float sign = (localPos.x >= 0) ? 1.0f : -1.0f;
					*pPenetration = D3DXVECTOR3(sign * (distX + sphereRadius), 0, 0);
				}
				else if (distY < distZ)
				{
					float sign = (localPos.y >= 0) ? 1.0f : -1.0f;
					*pPenetration = D3DXVECTOR3(0, sign * (distY + sphereRadius), 0);
				}
				else
				{
					float sign = (localPos.z >= 0) ? 1.0f : -1.0f;
					*pPenetration = D3DXVECTOR3(0, 0, sign * (distZ + sphereRadius));
				}
			}
		}
		return true;
	}

	return false;
}

void CCollider::Release()
{
	SAFE_DELETE(m_pBSphere);
	SAFE_DELETE(m_pBCube);
	SAFE_DELETE(m_pDbgCollider);
}

void CCollider::DebugDraw(CDirectX11& pDx11, D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	m_pDbgCollider->DrawCollider(pDx11, mView, mProj, m_Shape, *this);
}

