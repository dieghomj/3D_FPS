#pragma once

#include "CSprite3D.h"
#include "CDirectX11.h"

/**************************************************
*	Laser Shot Effect Class
*	Renders a laser beam from start to end position
*	that fades over time
**/
class CLaserShot
{
public:
	struct LASER_DATA
	{
		D3DXVECTOR3 startPos;
		D3DXVECTOR3 endPos;
		D3DXVECTOR3 direction;
		float length;
		float lifeTime;
		float maxLifeTime;
		bool isActive;
	};

	CLaserShot();
	~CLaserShot();

	// Initialize the laser sprite
	HRESULT Init(CDirectX11& pDx11, LPCTSTR lpFileName);

	// Fire a new laser from start to end
	void Fire(const D3DXVECTOR3& startPos, const D3DXVECTOR3& endPos);

	// Fire a laser with direction and length
	void Fire(const D3DXVECTOR3& startPos, const D3DXVECTOR3& direction, float length);

	// Update laser lifetime and fade
	void Update(float deltaTime);

	// Render the laser beam
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj);

	// Check if laser is still active
	bool IsActive() const { return m_LaserData.isActive; }

	// Set laser properties
	void SetWidth(float width) { m_Width = width; }
	void SetLifeTime(float lifeTime) { m_MaxLifeTime = lifeTime; }
	void SetColor(const D3DXVECTOR4& color) { m_Color = color; }

private:
	// Create world matrix oriented along the laser direction
	D3DXMATRIX CreateLaserWorldMatrix(const D3DXVECTOR3& startPos, 
		const D3DXVECTOR3& direction, float length);

private:
	CDirectX11* m_pDx11;
	ID3D11Device* m_pDevice11;
	ID3D11DeviceContext* m_pContext11;

	ID3D11VertexShader* m_pVertexShader;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11Buffer* m_pConstantBuffer;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11ShaderResourceView* m_pTexture;
	ID3D11SamplerState* m_pSampleLinear;

	LASER_DATA m_LaserData;

	float m_Width;
	float m_MaxLifeTime;
	D3DXVECTOR4 m_Color;

	// Shader constant buffer structure
	struct SHADER_CONSTANT_BUFFER
	{
		D3DXMATRIX mWVP;
		D3DXVECTOR4 vColor;
		D3DXVECTOR4 vUV;
	};

	// Vertex structure
	struct VERTEX
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR2 Tex;
	};

	HRESULT CreateShader();
	HRESULT CreateModel();
	HRESULT CreateTexture(LPCTSTR lpFileName);
	HRESULT CreateSampler();
};
