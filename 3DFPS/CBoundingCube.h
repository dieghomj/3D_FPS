#pragma once
#include "CStaticMesh.h"

class CBoundingCube
{

	public:
	CBoundingCube();
	~CBoundingCube();
	HRESULT CreateCubeForMesh(const CStaticMesh& pMesh);

	D3DXVECTOR3 GetPosition() const { return m_Position; }

	D3DXVECTOR3 GetMin() const { return m_Min; }
	D3DXVECTOR3 GetMax() const { return m_Max; }

	bool IsHit(const CBoundingCube& other) const;
	
	void SetPosition(const D3DXVECTOR3& pos) { m_Position = pos; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Rotation = rot; }
	void SetRotation(float x, float y, float z) { m_Rotation = D3DXVECTOR3(x, y, z); }
	void SetScale(float scale) { m_Scale = scale; }

	void SetMinMax(const D3DXVECTOR3& min, const D3DXVECTOR3& max) { m_Min = min; m_Max = max; };

private:

private:

	D3DXVECTOR3	m_Position;	//íÜêSç¿ïW
	D3DXVECTOR3 m_Rotation;	//âÒì]d
	float	m_Scale;	//ägèk

	D3DXVECTOR3 m_Min;		//ç≈è¨ç¿ïW
	D3DXVECTOR3 m_Max;		//ç≈ëÂç¿ïW

};

