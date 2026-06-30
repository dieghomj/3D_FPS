#include "CSkinMeshObject.h"

CSkinMeshObject::CSkinMeshObject()
	: m_pMesh			( nullptr )
	, m_pCollider		( nullptr )
	, m_pAnimCtrl		( nullptr )
{
	m_pCollider = new CCollider();
}

CSkinMeshObject::~CSkinMeshObject()
{
	SAFE_DELETE( m_pCollider );
	DetachMesh();
}

void CSkinMeshObject::Update()
{
	if( m_pMesh == nullptr ){
		return;
	}
}

void CSkinMeshObject::Draw(
	SCENE_DATA& sceneData)
{

	D3DXMATRIX& View = sceneData.mView;
	D3DXMATRIX& Proj = sceneData.mProj;
	LIGHT& Light = sceneData.Light;
	CAMERA& Camera = sceneData.Camera;
	FOG& Fog = sceneData.Fog;

	if( m_pMesh == nullptr ){
		return;
	}

	//描画直前で座標や回転情報などを更新.
	m_pMesh->SetPosition( m_vPosition );
	m_pMesh->SetRotation( m_vRotation );
	m_pMesh->SetScale( m_vScale );

	//レンダリング.
	m_pMesh->Render(View, Proj, Light, Camera.vPosition, Fog,
		m_pAnimCtrl );	//クローンを設定
}

//メッシュを接続する.
void CSkinMeshObject::AttachMesh(CSkinMesh& pMesh)
{
	m_pMesh = &pMesh;

	//アニメーションコントローラを取得
	LPD3DXANIMATIONCONTROLLER pAC = m_pMesh->GetAnimationController();

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
}
//メッシュを切り離す.
void CSkinMeshObject::DetachMesh()
{
	m_pMesh = nullptr;

	SAFE_RELEASE(m_pAnimCtrl);
}
