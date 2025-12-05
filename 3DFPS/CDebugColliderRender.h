#pragma once
#include <d3d11.h>
#include "CDirectX9.h"
#include "CDirectX11.h"
#include "CDebugBSphere.h"
#include "CDebugBBox.h"
#include "CCollider.h"

class CDebugColliderRender
{
	public:
	CDebugColliderRender();
	~CDebugColliderRender();
	HRESULT Init(CDirectX11& pDx11);
	void    Release();

	void DrawCollider(CDirectX11& pDx11, D3DXMATRIX& mView, D3DXMATRIX& mProj, CCollider::COLLIDER_SHAPE shape, CCollider& collider);

	void DrawSphere(ID3D11DeviceContext* ctx,
		const D3DXMATRIX& mView,
		const D3DXMATRIX& mProj,
		const D3DXVECTOR3& center,
		float radius,
		const D3DXVECTOR4& color);
	void DrawBox(ID3D11DeviceContext* ctx,
		const D3DXMATRIX& mView,
		const D3DXMATRIX& mProj,
		const D3DXVECTOR3& min,
		const D3DXVECTOR3& max,
		const D3DXVECTOR4& color);
private:
	CDebugBSphere*	m_pDbgBSphere;
	CDebugBBox*		m_pDbgBBox;
	CDirectX11*		m_pDx11;

};

