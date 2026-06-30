#pragma once
#include "CSprite3D.h"

class CDecal : public CSprite3D
{
public:
	CDecal();
	virtual ~CDecal();

	// デカール用シェーダを使用するためInitをオーバーライド.
	virtual HRESULT Init(CDirectX11& pDx11, LPCTSTR lpFileName, SPRITE_STATE& pSs) override;

	// デカール専用のレンダリング.
	void RenderDecal(D3DXMATRIX& mView, D3DXMATRIX& mProj, const D3DXVECTOR3& normal);

	// フェード距離を設定.
	void SetFadeDistance(float distance) { m_FadeDistance = distance; }


private:
	
	virtual HRESULT CreateShader();
	float m_FadeDistance;

};