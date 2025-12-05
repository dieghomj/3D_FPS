#include "stdafx.h"
#include "CBoundingCube.h"
#include "CStaticMesh.h"

CBoundingCube::CBoundingCube()
	: m_Position()
	, m_Min()
	, m_Max()
{
}

CBoundingCube::~CBoundingCube()
{
}

HRESULT CBoundingCube::CreateCubeForMesh(const CStaticMesh& pMesh)
{
    LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
    BYTE* pData = nullptr;

    LPD3DXMESH mesh9 = pMesh.GetMesh();
    if (!mesh9) return E_FAIL;

    if (FAILED(mesh9->GetVertexBuffer(&pVB)))
    {
        return E_FAIL;
    }

    HRESULT hr = pVB->Lock(0, 0, reinterpret_cast<void**>(&pData), D3DLOCK_READONLY);
    if (FAILED(hr))
    {
        SAFE_RELEASE(pVB);
        return hr;
    }

    // Find POSITION (usage=POSITION, usageIndex=0) offset in the vertex
    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    hr = mesh9->GetDeclaration(decl);
    if (FAILED(hr))
    {
        pVB->Unlock();
        SAFE_RELEASE(pVB);
        return hr;
    }

    UINT posOffset = 0;
    bool foundPos = false;
    for (int i = 0; decl[i].Stream != 0xFF; ++i)
    {
        if (decl[i].Usage == D3DDECLUSAGE_POSITION && decl[i].UsageIndex == 0)
        {
            posOffset = decl[i].Offset;
            foundPos = true;
            break;
        }
    }

    // Fallback: if no declaration (rare) assume position at offset 0
    if (!foundPos)
    {
        posOffset = 0;
    }

    const DWORD numVerts = mesh9->GetNumVertices();
    const UINT  stride = mesh9->GetNumBytesPerVertex();

    // Compute AABB from positions
    const BYTE* pFirstPos = pData + posOffset;

    D3DXVECTOR3 vMin, vMax;
    hr = D3DXComputeBoundingBox(
        reinterpret_cast<const D3DXVECTOR3*>(pFirstPos),
        numVerts,
        stride,
        &vMin,
        &vMax);

    pVB->Unlock();
    SAFE_RELEASE(pVB);

    if (FAILED(hr)) return hr;

    // Store local-space bounds; collider will offset to world
    m_Position = (vMin + vMax) * 0.5f; // local center (for reference)
    m_Min = vMin;
    m_Max = vMax;

    return S_OK;
}

bool CBoundingCube::IsHit(const CBoundingCube& other) const
{
    // AABB overlap test (assumes m_Min/m_Max are world-space at test time)
    if (m_Max.x < other.m_Min.x || m_Min.x > other.m_Max.x) return false;
    if (m_Max.y < other.m_Min.y || m_Min.y > other.m_Max.y) return false;
    if (m_Max.z < other.m_Min.z || m_Min.z > other.m_Max.z) return false;
    return true;
}
