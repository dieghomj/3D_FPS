#include "stdafx.h"
#include "CAnimCharacter.h"

CAnimCharacter::CAnimCharacter()
	: CCharacter()
	, m_pSkinMesh(nullptr)
	, m_pAnimCtrl(nullptr)
	, m_AnimNo(1)
	, m_AnimTime(0.f)
	, m_AnimSpeed(0.04f)
	, m_vBonePos(0.f, 0.f, 0.f)
{

}

CAnimCharacter::~CAnimCharacter()
{
}

void CAnimCharacter::InitAnimation()
{
	m_pSkinMesh->ChangeAnimSet(m_AnimNo, m_pAnimCtrl);
}

void CAnimCharacter::Update()
{
	
	m_AnimTime += (float)m_AnimSpeed;

}

//デバッグのみ使用
void CAnimCharacter::RenderStatic(SCENE_DATA& sceneData)
{
	CCharacter::Draw(sceneData);
}

void CAnimCharacter::Draw(SCENE_DATA& sceneData)
{

	D3DXMATRIX& View = sceneData.mView;
	D3DXMATRIX& Proj = sceneData.mProj;
	LIGHT& Light = sceneData.Light;
	CAMERA& Camera = sceneData.Camera;
	FOG& Fog = sceneData.Fog;

	if (m_pSkinMesh == nullptr) {
		return;
	}

	m_pSkinMesh->SetAnimSpeed(m_AnimSpeed);

	//描画直前で座標や回転情報などを更新.
	m_pSkinMesh->SetPosition(m_vPosition);
	m_pSkinMesh->SetRotation(m_vRotation);
	m_pSkinMesh->SetScale(m_vScale);
	//レンダリング.
	m_pSkinMesh->Render(View, Proj, Light, Camera.vPosition,Fog,
		m_pAnimCtrl);	//クローンを設定
}

void CAnimCharacter::AttachSkinMesh( CSkinMesh& pMesh)
{

	m_pSkinMesh = &pMesh;

	//アニメーションコントローラを取得
	LPD3DXANIMATIONCONTROLLER pAC = m_pSkinMesh->GetAnimationController();

	//アニメーションコントローラのクローンを作成
	if (FAILED(
		pAC->CloneAnimationController(
			pAC->GetMaxNumAnimationOutputs(),
			pAC->GetMaxNumAnimationSets(),
			pAC->GetMaxNumTracks(),
			pAC->GetMaxNumEvents(),
			&m_pAnimCtrl)))
	{
		_ASSERT_EXPR(false, L"アニメーションコントローラのクローン作成失敗");
	}

	m_pSkinMesh->ChangeAnimSet(m_AnimNo, m_pAnimCtrl);
}

bool CAnimCharacter::SetAnimNo(int no, int flag)
{
	bool isFinished = (m_AnimTime >= m_pSkinMesh->GetAnimPeriod(no));

	if (isFinished || (FORCE_CHANGE | flag))
	{
		if(m_AnimNo == no)
			return isFinished;
		m_AnimTime = 0.f;
		m_AnimNo = no;
		m_pSkinMesh->ChangeAnimSet(m_AnimNo, m_pAnimCtrl);
		//ここでアニメーションブレンド関数を呼ぶ
		
		return isFinished;
	}



	return false;
}

