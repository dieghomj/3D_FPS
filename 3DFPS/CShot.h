#pragma once

#include "CSpriteObject.h"

/**************************************************
*	弾クラス
**/
class CShot
	: public CSpriteObject
{
public:



public:
	CShot();
	virtual ~CShot() override;

	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj) override;

	//弾を再設定
	void Reload(const D3DXVECTOR3& Pos, float RotY);

	void SetDisplay(bool disp) { m_Display = disp; }

	virtual bool IsHit(CGameObject* obj, float rad);

	virtual bool IsDisplay() const { return m_Display; }

	float GetCadence() const { return m_Cadence; }		//<---プレイヤーのクラス

protected:
	bool		m_Display;			//表示切替
	int			m_DisplayTime;		//表示する時間
	
	D3DXVECTOR3	m_MoveDirection;	//移動方向
	float		m_MoveSpeed;		//移動速度
	float		m_Cadence;			//連射速度
	
};
