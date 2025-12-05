#pragma once
#include "CCharacter.h"
#include "CSkinMesh.h"

class CAnimCharacter
	: public CCharacter
{

public:

	enum
	{
		NO_FLAG = 0,	//フラグ無し.
		FORCE_CHANGE = 1,	//強制的にアニメーションを切り替えるフラグ.
		BLEND_CHANGE = 2,	//アニメーションをブレンドして切り替えるフラグ.
	};

	CAnimCharacter();
	virtual ~CAnimCharacter() override;
	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void InitAnimation();
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;
	//デバッグのみ使用
	void RenderStatic(SCENE_DATA& sceneData);
	
	//メッシュを接続する.
	void AttachSkinMesh(CSkinMesh& pMesh);
	void DetachSkinMesh() {
		m_pSkinMesh = nullptr;
	}
	void GetSkinMesh(CSkinMesh** ppMesh) {
		*ppMesh = m_pSkinMesh;
	}

	void SetAnimSpeed(double speed) { m_AnimSpeed = speed; }
	bool SetAnimNo(int no, int flag = 0);


private:

	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;	//アニメーションコントローラ
	CSkinMesh* m_pSkinMesh;
	int m_AnimNo;
	float m_AnimTime;
	float m_AnimSpeed;
	D3DXVECTOR3 m_vBonePos;
	
};

