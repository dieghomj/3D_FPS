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

	void AddVelocity(const D3DXVECTOR3& vel) { m_Velocity += vel; }
	void UpdateRayY(float posY);
	void UpdateCrossRay(CROSSRAY& crossRay, float posY);

	//弾を飛ばしたいか確認
	bool IsShot() const { return m_Shot; }
	void SetRadius(float radius) { m_Radius = radius; }
	void SetVelocity(const D3DXVECTOR3& vel) { m_Velocity = vel; }
	
	//キャラクターの半径を取得
	void EnableGravity(bool enable) { m_GravityEnabled = enable; }
	float GetRadius() const { return m_Radius; }
	float GetHeight() const { return m_Height; }
	D3DXVECTOR3 GetVelocity() const { return m_Velocity; }
	//Y軸方向へ伸ばしたレイを取得
	RAY GetRayY() const { return *m_pRayY; }
	//前後左右に伸ばしたレイを取得
	CROSSRAY GetCrossRay() { return *m_pCrossRay; }

protected:

	D3DXVECTOR3 m_Velocity;		//速度ベクトル

	bool	m_Shot;		//弾を飛ばすフラグ
	float m_Radius;	//キャラクターの半径
	float m_Height;	//キャラクターの高さ
	bool m_GravityEnabled; //重力有効フラグ

	RAY*		m_pRayY;		//Y方向へ伸ばしたレイ
	CROSSRAY*	m_pCrossRay;	//前後左右に伸ばしたレイ
};