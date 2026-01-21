#pragma once
#include <vector>
#include <d3dcompiler.h>

class CDebugBBox
{

public:

	CDebugBBox();
	~CDebugBBox();

	HRESULT Init(ID3D11Device* device);
	void    Release();
	void DrawBox(ID3D11DeviceContext* ctx,
		const D3DXMATRIX& mView,
		const D3DXMATRIX& mProj,
		const D3DXVECTOR3& min,
		const D3DXVECTOR3& max,
		const D3DXVECTOR4& color);

	// OBB描画（回転を考慮した8頂点を使用）
	void DrawOBB(ID3D11DeviceContext* ctx,
		const D3DXMATRIX& mView,
		const D3DXMATRIX& mProj,
		const D3DXVECTOR3 corners[8],
		const D3DXVECTOR4& color);

private:

	struct VERTEX { D3DXVECTOR3 Pos; D3DXVECTOR4 Color; };
	struct CBUFFER_VP { D3DXMATRIX mWVP; };
	HRESULT CreateShader(ID3D11Device* device);
	HRESULT CreateBuffers(ID3D11Device* device, int maxVerts);
	void    ApplyVP(ID3D11DeviceContext* ctx, const D3DXMATRIX& mVP);
	void    AppendBox(std::vector<VERTEX>& out, const D3DXVECTOR3& min, const D3DXVECTOR3& max,
		const D3DXVECTOR4& color);
	void    AppendOBB(std::vector<VERTEX>& out, const D3DXVECTOR3 corners[8],
		const D3DXVECTOR4& color);

private:

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pLayout;
	ID3D11Buffer* m_pCBuffer;
	ID3D11Buffer* m_pVertexBuffer;
	int                 m_MaxVBVerts;
	std::vector<VERTEX> m_TmpVerts;

};