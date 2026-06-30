#include "stdafx.h"
#include "CDebugColliderRender.h"
#include "CBoundingCube.h"
#include "CBoundingSphere.h"

CDebugColliderRender::CDebugColliderRender()
	: m_pDx11(nullptr)
	, m_pDbgBSphere(nullptr)
	, m_pDbgBBox(nullptr)
{
}

CDebugColliderRender::~CDebugColliderRender()
{
	Release();
}

HRESULT CDebugColliderRender::Init(CDirectX11& pDx11)
{
	m_pDx11 = &pDx11;
	m_pDbgBSphere = new CDebugBSphere();
	m_pDbgBSphere->Init(m_pDx11->GetDevice());
	m_pDbgBBox = new CDebugBBox();
	m_pDbgBBox->Init(m_pDx11->GetDevice());

	return S_OK;
}

void CDebugColliderRender::Release()
{
	SAFE_DELETE(m_pDbgBSphere);
	SAFE_DELETE(m_pDbgBBox);
}

void CDebugColliderRender::DrawCollider(CDirectX11& pDx11, D3DXMATRIX& mView, D3DXMATRIX& mProj, CCollider::COLLIDER_SHAPE shape, CCollider& collider)
{
	CBoundingSphere* pSphere = nullptr;
	CBoundingCube* pBox = nullptr;
	m_pDx11 = &pDx11;

	switch (shape)
	{

	case CCollider::COLLIDER_SHAPE_SPHERE:
		pSphere = collider.GetBSphere();
		DrawSphere(m_pDx11->GetContext(),
			mView,
			mProj,
			pSphere->GetPosition(),
			pSphere->GetRadius(),
			D3DXVECTOR4(0.1f, 1.0f, 0.1f, 1.0f));
		break;

	case CCollider::COLLIDER_SHAPE_CUBE:

		pBox = collider.GetBBox();

		// OBB描画（回転を考慮）
		{
			D3DXVECTOR3 corners[8];
			pBox->GetWorldCorners(corners);
			m_pDbgBBox->DrawOBB(m_pDx11->GetContext(),
				mView,
				mProj,
				corners,
				D3DXVECTOR4(0.1f, 1.0f, 0.1f, 1.0f));
		}

		break;
	default:
		break;
	}
}

void CDebugColliderRender::DrawSphere(ID3D11DeviceContext* ctx,
	const D3DXMATRIX& mView,
	const D3DXMATRIX& mProj,
	const D3DXVECTOR3& center,
	float radius,
	const D3DXVECTOR4& color)
{
	if (m_pDbgBSphere)
	{
		m_pDbgBSphere->DrawSphere(ctx, mView, mProj, center, radius, color);
	}
}

void CDebugColliderRender::DrawBox(ID3D11DeviceContext* ctx,
	const D3DXMATRIX& mView,
	const D3DXMATRIX& mProj,
	const D3DXVECTOR3& min,
	const D3DXVECTOR3& max,
	const D3DXVECTOR4& color)
{
	if (m_pDbgBBox)
	{
		m_pDbgBBox->DrawBox(ctx, mView, mProj, min, max, color);
	}
}