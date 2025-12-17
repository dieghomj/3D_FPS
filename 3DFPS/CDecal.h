#pragma once
#include "CSprite3D.h"

class CDecal : public CSprite3D
{
public:
	CDecal();
	virtual ~CDecal();

	// Override Init to use decal shader
	virtual HRESULT Init(CDirectX11& pDx11, LPCTSTR lpFileName, SPRITE_STATE& pSs) override;

	// Specialized decal rendering
	void RenderDecal(D3DXMATRIX& mView, D3DXMATRIX& mProj, const D3DXVECTOR3& normal);

	// Set fade distance
	void SetFadeDistance(float distance) { m_FadeDistance = distance; }


private:
	
	virtual HRESULT CreateShader();
	float m_FadeDistance;

};