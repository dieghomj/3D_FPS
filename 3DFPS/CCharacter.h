#pragma once

#include "CStaticMeshObject.h"

/**************************************************
*   キャラクタークラス
**/
class CCharacter
	: public CStaticMeshObject
{
public:

public:
	CCharacter();
	virtual ~CCharacter();

	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;

	//弾を飛ばしたいか確認
	bool IsShot() const { return m_Shot; }
	void SetRadius(float radius) { m_Radius = radius; }
	//キャラクターの半径を取得
	float GetRadius() const { return m_Radius; }
	//Y軸方向へ伸ばしたレイを取得
	RAY GetRayY() const { return *m_pRayY; }
	//前後左右に伸ばしたレイを取得
	CROSSRAY GetCrossRay() { return *m_pCrossRay; }
protected:
	bool	m_Shot;		//弾を飛ばすフラグ

	float m_Radius;	//キャラクターの半径

	bool m_GravityEnabled; //重力有効フラグ

	RAY*		m_pRayY;		//Y方向へ伸ばしたレイ
	CROSSRAY*	m_pCrossRay;	//前後左右に伸ばしたレイ
};