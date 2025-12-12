#pragma once

/**************************************************
*	ゲームオブジェクトクラス.
**/
class CGameObject
{
public:
	CGameObject();
	//継承を行う場合は、デストラクタにvirtualを付けて仮想関数にすること.
	virtual ~CGameObject();

	//virtual 型 関数名 = 0; 純粋仮想関数.
	//子クラスに処理をお任せするので、ここでは名前だけ宣言して定義は書かない.
	virtual void Update() = 0;
	virtual void Draw(SCENE_DATA& sceneData) = 0;

	//座標設定関数.
	void SetPosition( float x, float y, float z ){
		m_vPosition.x = x;
		m_vPosition.y = y;
		m_vPosition.z = z;
	};

	void SetPosition(const D3DXVECTOR3& pos) {
		m_vPosition = pos;
	}
	//座標取得関数.
	const D3DXVECTOR3& GetPosition() const {
		return m_vPosition;
	}

	//回転設定関数.
	void SetRotation( float x, float y, float z ){
		m_vRotation.x = x;
		m_vRotation.y = y;
		m_vRotation.z = z;

		CalculateVectors();

	}

	void SetRotation(const D3DXVECTOR3& rot) {
		m_vRotation = rot;

		CalculateVectors();

	}
	//回転取得関数.
	const D3DXVECTOR3& GetRotation() const {
		return m_vRotation;
	}

	//拡縮設定関数.
	void SetScale( float x, float y, float z ){
		m_vScale.x = x;
		m_vScale.y = y;
		m_vScale.z = z;
	}
	void SetScale(float xyz) {
		m_vScale = D3DXVECTOR3(xyz, xyz, xyz);
	}
	//拡縮取得関数.
	const D3DXVECTOR3& GetScale() const {
		return m_vScale;
	}

	D3DXVECTOR3 GetForward() const { return m_vForward; };
	
	D3DXVECTOR3 GetRight() const { return m_vRight; };

private:

	void CalculateVectors()
	{
		D3DXVECTOR3 vecZ = D3DXVECTOR3(0.f, 0.f, 1.f);
		D3DXVECTOR3 vecX = D3DXVECTOR3(1.f, 0.f, 0.f);

		D3DXMATRIX rotY;
		D3DXMatrixRotationY(&rotY, m_vRotation.y);

		D3DXVec3TransformCoord(&m_vForward, &m_vForward, &rotY);
		D3DXVec3TransformCoord(&m_vRight, &m_vRight, &rotY);

		D3DXVec3Normalize(&m_vForward, &m_vForward);
		D3DXVec3Normalize(&m_vRight, &m_vRight);
	}
	;

protected://protectedは子クラスのみアクセス可能.

	D3DXVECTOR3 m_vForward;
	D3DXVECTOR3 m_vRight;
	D3DXVECTOR3	m_vPosition;
	D3DXVECTOR3	m_vRotation;
	D3DXVECTOR3	m_vScale;
};