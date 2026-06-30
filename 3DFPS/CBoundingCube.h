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

	// ローカル座標のmin/max（メッシュから取得した元の値）
	D3DXVECTOR3 GetLocalMin() const { return m_LocalMin; }
	D3DXVECTOR3 GetLocalMax() const { return m_LocalMax; }

	// OBB用：ワールド行列を取得
	const D3DXMATRIX& GetWorldMatrix() const { return m_WorldMatrix; }

	// 回転を考慮した8頂点を取得
	void GetWorldCorners(D3DXVECTOR3 outCorners[8]) const;

	// OBB用：ハーフサイズと中心を取得
	D3DXVECTOR3 GetHalfExtents() const;
	D3DXVECTOR3 GetCenter() const;

	bool IsHit(const CBoundingCube& other) const;

	void SetPosition(const D3DXVECTOR3& pos) { m_Position = pos; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Rotation = rot; }
	void SetRotation(float x, float y, float z) { m_Rotation = D3DXVECTOR3(x, y, z); }
	void SetScale(float scale) { m_Scale = scale; }

	void SetMinMax(const D3DXVECTOR3& min, const D3DXVECTOR3& max) { m_Min = min; m_Max = max; }
	void SetLocalMinMax(const D3DXVECTOR3& min, const D3DXVECTOR3& max) { m_LocalMin = min; m_LocalMax = max; }
	void SetWorldMatrix(const D3DXMATRIX& world) { m_WorldMatrix = world; }

private:

private:

	D3DXVECTOR3	m_Position;	// 中心座標
	D3DXVECTOR3 m_Rotation;	// 回転
	float	m_Scale;	// 拡縮

	D3DXVECTOR3 m_Min;		// ワールドAABBの最小座標
	D3DXVECTOR3 m_Max;		// ワールドAABBの最大座標

	D3DXVECTOR3 m_LocalMin;	// ローカルの最小座標
	D3DXVECTOR3 m_LocalMax;	// ローカルの最大座標

	D3DXMATRIX m_WorldMatrix;	// ワールド変換行列（OBB用）

};