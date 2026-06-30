#include "stdafx.h"
#include "CBoundingCube.h"

CBoundingCube::CBoundingCube()
	: m_Position(0.f, 0.f, 0.f)
	, m_Rotation(0.f, 0.f, 0.f)
	, m_Scale(1.f)
	, m_Min(0.f, 0.f, 0.f)
	, m_Max(0.f, 0.f, 0.f)
	, m_LocalMin(0.f, 0.f, 0.f)
	, m_LocalMax(0.f, 0.f, 0.f)
{
	D3DXMatrixIdentity(&m_WorldMatrix);
}

CBoundingCube::~CBoundingCube()
{
}

HRESULT CBoundingCube::CreateCubeForMesh(const CStaticMesh& pMesh)
{
	LPD3DXMESH pD3DMesh = pMesh.GetMeshForRay();
	if (!pD3DMesh) return E_FAIL;

	BYTE* pVertices = nullptr;
	if (FAILED(pD3DMesh->LockVertexBuffer(D3DLOCK_READONLY, reinterpret_cast<void**>(&pVertices))))
	{
		return E_FAIL;
	}

	DWORD numVertices = pD3DMesh->GetNumVertices();
	DWORD stride = pD3DMesh->GetNumBytesPerVertex();

	D3DXVECTOR3 minPos(FLT_MAX, FLT_MAX, FLT_MAX);
	D3DXVECTOR3 maxPos(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (DWORD i = 0; i < numVertices; ++i)
	{
		D3DXVECTOR3* pPos = reinterpret_cast<D3DXVECTOR3*>(pVertices + i * stride);
		minPos.x = min(minPos.x, pPos->x);
		minPos.y = min(minPos.y, pPos->y);
		minPos.z = min(minPos.z, pPos->z);
		maxPos.x = max(maxPos.x, pPos->x);
		maxPos.y = max(maxPos.y, pPos->y);
		maxPos.z = max(maxPos.z, pPos->z);
	}

	pD3DMesh->UnlockVertexBuffer();

	m_Min = minPos;
	m_Max = maxPos;
	m_LocalMin = minPos;
	m_LocalMax = maxPos;
	m_Position = (minPos + maxPos) * 0.5f;

	return S_OK;
}

bool CBoundingCube::IsHit(const CBoundingCube& other) const
{
	// AABB衝突判定（回転なしの場合）
	if (m_Max.x < other.m_Min.x || m_Min.x > other.m_Max.x) return false;
	if (m_Max.y < other.m_Min.y || m_Min.y > other.m_Max.y) return false;
	if (m_Max.z < other.m_Min.z || m_Min.z > other.m_Max.z) return false;
	return true;
}

void CBoundingCube::GetWorldCorners(D3DXVECTOR3 outCorners[8]) const
{
	const D3DXVECTOR3& mn = m_LocalMin;
	const D3DXVECTOR3& mx = m_LocalMax;

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

	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&outCorners[i], &localCorners[i], &m_WorldMatrix);
	}
}

D3DXVECTOR3 CBoundingCube::GetHalfExtents() const
{
	return (m_LocalMax - m_LocalMin) * 0.5f;
}

D3DXVECTOR3 CBoundingCube::GetCenter() const
{
	D3DXVECTOR3 localCenter = (m_LocalMin + m_LocalMax) * 0.5f;
	D3DXVECTOR3 worldCenter;
	D3DXVec3TransformCoord(&worldCenter, &localCenter, &m_WorldMatrix);
	return worldCenter;
}