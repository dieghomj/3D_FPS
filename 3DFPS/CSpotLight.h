#pragma once
class CSpotLight
{

public:
	CSpotLight();
	~CSpotLight();

	void Init();
	void Release();
	void Update();
	void Render(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, D3DXVECTOR3& CameraPos, FOG& Fog);

	//位置取得
	D3DXVECTOR3 GetPosition() const {
		return m_vPosition;
	}
	//方向取得
	D3DXVECTOR3 GetDirection() const {
		return m_vDirection;
	}
	//色取得
	D3DXCOLOR GetColor() const {
		return m_Color;
	}
	//範囲取得
	float GetRange() const {
		return m_Range;
	}
	//内側角度取得
	float GetInnerAngle() const {
		return m_InnerAngle;
	}
	//外側角度取得
	float GetOuterAngle() const {
		return m_OuterAngle;
	}

	float GetIntensity() const {
		return m_Intensity;
	}

	int GetSceneIndex() const {
		return m_Index;
	}


	//位置設定
	void SetPosition(const D3DXVECTOR3& vPosition) {
		m_vPosition = vPosition;
	}
	//方向設定
	void SetDirection(const D3DXVECTOR3& vDirection) {
		m_vDirection = vDirection;
	}
	//色設定
	void SetColor(const D3DXCOLOR& Color) {
		m_Color = Color;
	}
	//範囲設定
	void SetRange(float Range) {
		m_Range = Range;
	}
	//内側角度設定
	void SetInnerAngle(float InnerAngle) {
		m_InnerAngle = InnerAngle;
	}
	//外側角度設定
	void SetOuterAngle(float OuterAngle) {
		m_OuterAngle = OuterAngle;
	}

	void SetIntensity(float Intensity) {
		m_Intensity = Intensity;
	}

	void SetSceneIndex(int index)
	{
		m_Index = index;
	}

private:
	D3DXVECTOR3 m_vPosition;	//位置
	D3DXVECTOR3 m_vDirection;	//方向
	D3DXCOLOR	m_Color;		//色
	float		m_Range;		//範囲
	float		m_InnerAngle;	//内側角度
	float		m_OuterAngle;	//外側角度
	float		m_Intensity;	//強度
	int			m_Index;
};

