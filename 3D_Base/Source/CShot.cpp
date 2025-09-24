#include "CShot.h"

CShot::CShot()
	: m_Display			(false)
	, m_MoveDirection	()
	, m_MoveSpeed		()
	, m_DisplayTime		()
{
}

CShot::~CShot()
{
}

void CShot::Update()
{
	if (m_Display == true) {
		//移動方向に移動速度をかけ合わせたものを座標に反映
		m_vPosition += m_MoveDirection * m_MoveSpeed;

		m_DisplayTime--;
		if ( m_DisplayTime < 0 ) {
			//見えない所に置いておく
			m_vPosition = D3DXVECTOR3(0.f, -10.f, 0.f);
			m_Display = false;
		}
	}
}

void CShot::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	if (m_Display == true) {
		CStaticMeshObject::Draw(View, Proj, Light, Camera);
	}
}

void CShot::Reload(const D3DXVECTOR3& Pos, float RotY)
{
	//弾が飛んでたら終了
	if (m_Display == true) return;
	
	m_vPosition		= Pos;
	m_Display		= true;
	m_vRotation.y	= RotY;		//弾の向き（見た目）も変える
	m_MoveSpeed		= 0.2f;		//移動速度　※とりあえず0.2fを設定
	m_DisplayTime	= FPS * 3;	//約3秒くらい表示する

	//Z軸ベクトル
	m_MoveDirection = D3DXVECTOR3(0.f, 0.f, 1.f);

	//Y軸回転行列
	D3DXMATRIX mRotationY;
	//Y軸回転行列を作成
	D3DXMatrixRotationY(
		&mRotationY,		//(out)行列
		m_vRotation.y);	//プレイヤーのY方向の回転値

	//Y軸回転行列を使ってZ軸ベクトルを座標変換する
	D3DXVec3TransformCoord(
		&m_MoveDirection,	//(out)Z軸ベクトル
		&m_MoveDirection,	//(in)Z軸ベクトル
		&mRotationY);		//Y軸回転行列
}

