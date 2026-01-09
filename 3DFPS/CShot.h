#pragma once

#include "CSpriteObject.h"
#include "CStaticMeshObject.h"

/**************************************************
*	弾クラス
**/
class CShot
	: public CStaticMeshObject
{
public:



public:
	CShot();
	virtual ~CShot() override;

	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;

	//弾を再設定
	void Reload(const D3DXVECTOR3& Pos, const D3DXVECTOR3& Direction, float RotY);

	void SetDisplay(bool disp) { m_Display = disp; }

	void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }

	virtual bool IsHit(CCollider* col, float rad);

	virtual bool IsDisplay() const { return m_Display; }

	float GetCadence() const { return m_Cadence; }		//<---プレイヤーのクラス

protected:
	bool		m_Display;			//表示切替
	int			m_DisplayTime;		//表示する時間
	
	D3DXVECTOR3	m_MoveDirection;	//移動方向
	float		m_MoveSpeed;		//移動速度
	float		m_Cadence;			//連射速度
	
};

