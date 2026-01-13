#pragma once

#include "CGameObject.h"
#include "CStaticMesh.h"
#include "CBoundingSphere.h"
#include "CBoundingCube.h"
#include "CCollider.h"
#include "CRay.h"

/************************************************************
*	スタティックメッシュオブジェクトクラス.
**/
class CStaticMeshObject
	: public CGameObject
{

public:
	CStaticMeshObject();
	virtual ~CStaticMeshObject() override;

	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	virtual void Draw( SCENE_DATA& sceneData ) override;

	//メッシュを接続する.
	virtual void AttachMesh( CStaticMesh& pMesh ){
		m_pMesh = &pMesh;
	}
	//メッシュを切り離す.
	virtual void DetachMesh(){
		m_pMesh = nullptr;
	}

	HRESULT CreateCollider(CCollider::COLLIDER_SHAPE shape);

	//バウンディングスフィア取得
	CCollider* GetCollider() const {
		return m_pCollider;
	}

	//バウンディングスフィアをオブジェクト位置に合わせる
	//※モデルの原点が中心の場合を想定
	void UpdateCollider() {
		m_pCollider->SetPosition(m_vPosition);
		m_pCollider->SetRotation(m_vRotation);
		m_pCollider->SetScale(m_vScale);
	}

	//レイとメッシュの当たり判定
	bool IsHitForRay(
		const RAY& pRay,					//レイ構造体
		float* pDistance,					//(out)距離
		D3DXVECTOR3* pIntersect,			//(out)交差点
		D3DXVECTOR3* pNormal = nullptr);	//(out)法線(ベクトル)

	//壁からの位置を計算する
	void CalculatePositionFromWall(CROSSRAY* pCrossRay);

	void ScaleAnim(float dt, float speed);
	void ScaleMorphAnim(float dt, float amp, float speed);
	void RotateAnim(float dt, float speed);
	void UpDownAnim(float dt, float amp, float speed);
	void VibrateAnim(float dt, float amp, float speed);

	void SetWorldMatrix(D3DXMATRIX mWorld) { m_mWorld = mWorld;};

protected:


private:
	//交差位置のポリゴンの頂点を見つける
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices);	//(out)頂点情報

	//回転値調整（１周以上している時の調整）
	void ClampDirection(float* dir);	//再帰関数


	float m_ScaleAnimTime = 0.0f;
	float m_ScaleMorphAnimTime = 0.0f;
	float m_UpDownAnimTime = 0.0f;
	float m_VibrateAnimTime = 0.0f;
	float m_RotateAnimTime = 0.0f;

protected:
	CStaticMesh*		m_pMesh;
	CCollider*			m_pCollider;
	D3DXMATRIX			m_mWorld;
};
