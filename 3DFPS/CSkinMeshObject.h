#pragma once

#include "CGameObject.h"
#include "CSkinMesh.h"
#include "CBoundingSphere.h"
#include "CCollider.h"

/************************************************************
*	スキンメッシュオブジェクトクラス.
**/
class CSkinMeshObject
	: public CGameObject
{
public:
	CSkinMeshObject();
	virtual ~CSkinMeshObject() override;

	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	virtual void Draw( SCENE_DATA& sceneData ) override;

	//メッシュを接続する.
	void AttachMesh(CSkinMesh& pMesh);
	//メッシュを切り離す.
	void DetachMesh();

	//コライダー作成
	CCollider* GetCollider() const {
		return m_pCollider;
	}

	CBoundingCube* GetBCube() const {
		return m_pCollider->GetBBox();
	}

	//バウンディングスフィア取得
	CBoundingSphere* GetBSphere() const {
		return m_pCollider->GetBSphere();
	}
	//モデルに合わせたバウンディングスフィア作成のラッパー関数
	HRESULT CreateBSphereForMesh(const CStaticMesh& pMesh) {
		return m_pCollider->CreateSphereForMesh(pMesh);
	}
	//バウンディングスフィアをオブジェクト位置に合わせる
	//※モデルの原点が中心の場合を想定
	void UpdateColliderPos() {
		m_pCollider->SetPosition(m_vPosition);
	}

protected:
	CSkinMesh*					m_pMesh;
	CCollider*					m_pCollider;
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;	//アニメーションコントローラ
};
