#pragma once

#include "CDirectX11.h"
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>

/**************************************************
*	キューブマップによるスカイボックスを描画するクラス。
*	キューブマップテクスチャを用いてカメラの周囲に立方体を描画する。
**/
class CSkybox
{
public:
	CSkybox();
	~CSkybox();

	// キューブマップのDDSテクスチャでスカイボックスを初期化する
	HRESULT Init(CDirectX11& dx11, LPCTSTR lpCubemapFile);

	// リソースを解放する
	void Release();

	// スカイボックスを描画する（深度書き込みを無効にし、他のオブジェクトより前に呼び出す）
	void Render(const D3DXMATRIX& mView, const D3DXMATRIX& mProj, const D3DXVECTOR3& camPos);

private:
	// スカイボックス用の定数バッファ構造体
	struct CBUFFER_SKYBOX
	{
		D3DXMATRIX mWVP;
	};

	// スカイボックス用の頂点構造体
	struct VERTEX
	{
		D3DXVECTOR3 Pos;
	};

	// シェーダーリソースを作成する
	HRESULT CreateShader();

	// 立方体用の頂点／インデックスバッファを作成する
	HRESULT CreateBuffers();

	// キューブマップテクスチャを読み込む
	HRESULT LoadCubemap(LPCTSTR lpCubemapFile);

private:
	CDirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pVertexBuffer;
	ID3D11Buffer*			m_pIndexBuffer;
	ID3D11Buffer*			m_pConstantBuffer;

	ID3D11ShaderResourceView*	m_pCubemapSRV;
	ID3D11SamplerState*			m_pSamplerState;

	// スカイボックス描画用の深度ステンシルステート
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11RasterizerState*		m_pRasterizerState;

	bool                        m_initialized = false;
};
