#pragma once
#include <d3d11.h>
#include <d3dx9math.h>

class CMiniMapTexture
{
public:
	CMiniMapTexture();
	~CMiniMapTexture();

	bool Init(ID3D11Device& device, ID3D11DeviceContext& context, int pixelWidth, int pixelHeight);
	void Release();

	// grid: pointer to grid (stride used), regionW/H are maze cell counts,
	// playerPos is world pos (x,z), cellWorldSize must match maze wallSize
	void UpdateFromGrid(const int* grid, int stride, int regionW, int regionH, const D3DXVECTOR3& playerPos, float cellWorldSize);

	ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	ID3D11Texture2D* m_pTexture;
	ID3D11ShaderResourceView* m_pSRV;
	int m_width;
	int m_height;
};
