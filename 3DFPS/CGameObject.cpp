#include "CGameObject.h"

//コンストラクタ.
CGameObject::CGameObject()
	: m_vPosition	()
	, m_vRotation	()
	, m_vScale		( 1.0f, 1.0f, 1.0f )
	, m_vForward	( 0.0f, 0.0f, 1.0f )
	, m_vRight		( 1.0f, 0.0f, 0.0f )
    , m_IsActive    ( true )
{
}

//デストラクタ.
CGameObject::~CGameObject()
{
}

void CGameObject::CalculateVectors()
{
	D3DXVECTOR3 vecZ = D3DXVECTOR3(0.f, 0.f, 1.f);
	D3DXVECTOR3 vecX = D3DXVECTOR3(1.f, 0.f, 0.f);

    D3DXVECTOR3 forward = vecZ;
    {
        D3DXMATRIX rotY;
        D3DXMatrixRotationY(&rotY, m_vRotation.y);

        D3DXMATRIX rotX;
        D3DXMatrixRotationX(&rotX, m_vRotation.x);

        // Order: yaw then pitch (rotY first, then rotX)
        D3DXMATRIX rot = rotY * rotX;

        D3DXVec3TransformCoord(&forward, &forward, &rot);
        D3DXVec3Normalize(&forward, &forward);
    }

    // ----- Right (yaw only, horizontal strafe) -----
    D3DXVECTOR3 right = vecX;
    {
        D3DXMATRIX rotY;
        D3DXMatrixRotationY(&rotY, m_vRotation.y);

        D3DXVec3TransformCoord(&right, &right, &rotY);
        D3DXVec3Normalize(&right, &right);
    }

    m_vForward = forward;
    m_vRight = right;

}
