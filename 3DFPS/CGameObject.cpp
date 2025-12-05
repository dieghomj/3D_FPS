#include "CGameObject.h"

//コンストラクタ.
CGameObject::CGameObject()
	: m_vPosition	()
	, m_vRotation	()
	, m_vScale		( 1.0f, 1.0f, 1.0f )
{
}

//デストラクタ.
CGameObject::~CGameObject()
{
}