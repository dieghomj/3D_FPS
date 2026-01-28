#pragma once

#include "CDirectX11.h"
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>

/**************************************************
*	Skybox class for rendering a cubemap skybox.
*	Renders a cube around the camera using a cubemap texture.
**/
class CSkybox
{
public:
	CSkybox();
	~CSkybox();

	// Initialize skybox with a cubemap DDS texture
	HRESULT Init(CDirectX11& dx11, LPCTSTR lpCubemapFile);

	// Release resources
	void Release();

	// Render the skybox (call before other objects, with depth write disabled)
	void Render(const D3DXMATRIX& mView, const D3DXMATRIX& mProj, const D3DXVECTOR3& camPos);

private:
	// Constant buffer structure for skybox
	struct CBUFFER_SKYBOX
	{
		D3DXMATRIX mWVP;
	};

	// Vertex structure for skybox
	struct VERTEX
	{
		D3DXVECTOR3 Pos;
	};

	// Create shader resources
	HRESULT CreateShader();

	// Create vertex/index buffers for cube
	HRESULT CreateBuffers();

	// Load cubemap texture
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

	// Depth stencil states for rendering skybox
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11RasterizerState*		m_pRasterizerState;

	bool                        m_initialized = false;
};
