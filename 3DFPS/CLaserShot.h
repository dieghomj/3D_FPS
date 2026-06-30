#pragma once

#include "CSprite3D.h"
#include "CDirectX11.h"

/**************************************************
*	レーザーショットエフェクトクラス.
*	始点から終点へレーザービームを描画し、
*	時間経過とともにフェードさせる.
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

	// レーザースプライトを初期化する.
	HRESULT Init(CDirectX11& pDx11, LPCTSTR lpFileName);

	// 始点から終点へ新しいレーザーを発射する.
	void Fire(const D3DXVECTOR3& startPos, const D3DXVECTOR3& endPos);

	// 方向と長さを指定してレーザーを発射する.
	void Fire(const D3DXVECTOR3& startPos, const D3DXVECTOR3& direction, float length);

	// レーザーの寿命とフェードを更新する.
	void Update(float deltaTime);

	// レーザービームを描画する.
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj);

	// レーザーがまだ有効かどうかを確認する.
	bool IsActive() const { return m_LaserData.isActive; }

	// レーザーのプロパティを設定する.
	void SetWidth(float width) { m_Width = width; }
	void SetLifeTime(float lifeTime) { m_MaxLifeTime = lifeTime; }
	void SetColor(const D3DXVECTOR4& color) { m_Color = color; }

private:
	// レーザーの方向に沿って向きを合わせたワールド行列を作成する.
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

	// シェーダのコンスタントバッファ構造体.
	struct SHADER_CONSTANT_BUFFER
	{
		D3DXMATRIX mWVP;
		D3DXVECTOR4 vColor;
		D3DXVECTOR4 vUV;
	};

	// 頂点構造体.
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
