#include "CShot.h"

CShot::CShot()
	: m_Display			(false)
	, m_DisplayTime		()
	, m_MoveDirection	()
	, m_MoveSpeed		()
	, m_Cadence			(100.0f)
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

void CShot::Draw(SCENE_DATA& sceneData)
{
	if (m_Display == true) {
		CStaticMeshObject::Draw(sceneData);
	}
}

bool CShot::IsHit(CGameObject* obj, float rad)
{
	if (m_Display == true)
	{
	}
	return false;
}

void CShot::Reload(const D3DXVECTOR3& Pos, const D3DXVECTOR3& Direction, float RotY)
{
	//弾が飛んでたら終了
	if (m_Display == true) return;
	
	m_vPosition		= Pos;
	m_Display		= true;
	m_MoveSpeed		= 1.5f;								//移動速度　※とりあえず0.2fを設定
	m_vRotation.y	= RotY;								//Y軸回転値
	m_DisplayTime	= FPS * 2;							//約3秒くらい表示する

	//Z軸ベクトル
	m_MoveDirection = Direction;

}

