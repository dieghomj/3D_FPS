#include "CMiniMap.h"
#include <algorithm>
#include <cstdint>
#include <vector>
#include <algorithm>

CMiniMapTexture::CMiniMapTexture()
	: m_pDevice(nullptr)
	, m_pContext(nullptr)
	, m_pTexture(nullptr)
	, m_pSRV(nullptr)
	, m_width(0)
	, m_height(0)
{
}

CMiniMapTexture::~CMiniMapTexture()
{
	Release();
}

bool CMiniMapTexture::Init(ID3D11Device& device, ID3D11DeviceContext& context, int pixelWidth, int pixelHeight)
{
	if (pixelWidth <= 0 || pixelHeight <= 0) return false;

	Release();

	*m_pDevice = device;
	*m_pContext = context;
	m_width = pixelWidth;
	m_height = pixelHeight;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU writeable
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture)))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = desc.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &srvd, &m_pSRV)))
	{
		m_pTexture->Release();
		m_pTexture = nullptr;
		return false;
	}

	return true;
}

void CMiniMapTexture::Release()
{
	if (m_pSRV) { m_pSRV->Release(); m_pSRV = nullptr; }
	if (m_pTexture) { m_pTexture->Release(); m_pTexture = nullptr; }
	m_pDevice = nullptr;
	m_pContext = nullptr;
	m_width = m_height = 0;
}

// Helper to pack RGBA into uint32: memory will be R,G,B,A because of little-endian layout.
static inline uint32_t PackRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
{
	return (static_cast<uint32_t>(a) << 24) |
		(static_cast<uint32_t>(b) << 16) |
		(static_cast<uint32_t>(g) << 8) |
		(static_cast<uint32_t>(r));
}

void CMiniMapTexture::UpdateFromGrid(const int* grid, int stride, int regionW, int regionH, const D3DXVECTOR3& playerPos, float cellWorldSize)
{
	if (!m_pContext || !m_pTexture || !grid) return;

	// mapping: choose pixels per cell
	int pxPerCellX = max(1, m_width / regionW);
	int pxPerCellY = max(1, m_height / regionH);

	// Map the entire texture
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (FAILED(m_pContext->Map(m_pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
		return;

	uint8_t* dest = reinterpret_cast<uint8_t*>(mapped.pData);
	int rowPitch = mapped.RowPitch; // bytes per row

	// clear to floor color (light gray)
	uint32_t floorCol = PackRGBA(200, 200, 200, 255);
	for (int y = 0; y < m_height; ++y)
	{
		uint32_t* row = reinterpret_cast<uint32_t*>(dest + y * rowPitch);
		for (int x = 0; x < m_width; ++x)
		{
			row[x] = floorCol;
		}
	}

	// colors
	uint32_t wallCol = PackRGBA(30, 30, 30, 255);
	uint32_t playerCol = PackRGBA(220, 30, 30, 255);

	// draw cells: if wall on north/west/etc. draw pixel strips
	for (int gy = 0; gy < regionH; ++gy)
	{
		for (int gx = 0; gx < regionW; ++gx)
		{
			int cell = grid[gy * stride + gx];

			// pixel top-left for this cell
			int px = gx * pxPerCellX;
			int py = gy * pxPerCellY;

			// draw north wall
			if (!(cell & 1 /*CMaze::North assumed 1?*/))
			{
				int wy = py; // top row of cell
				if (wy >= 0 && wy < m_height)
				{
					uint32_t* row = reinterpret_cast<uint32_t*>(dest + wy * rowPitch);
					for (int x = px; x < min(m_width, px + pxPerCellX); ++x) row[x] = wallCol;
				}
			}
			// draw west wall
			// assume West bit value not known here; user should adapt masks to CMaze constants
			// Draw vertical line at px
			if (!(cell & 8 /*CMaze::West assumed 8?*/))
			{
				int wx = px;
				if (wx >= 0 && wx < m_width)
				{
					for (int y = py; y < min(m_height, py + pxPerCellY); ++y)
					{
						uint32_t* row = reinterpret_cast<uint32_t*>(dest + y * rowPitch);
						row[wx] = wallCol;
					}
				}
			}
			// For border South/East you can add similar logic as needed
		}
	}

	// draw player: convert world position -> grid index
	float worldOffsetX = (regionW / 2.0f) * cellWorldSize;
	float worldOffsetZ = (regionH / 2.0f) * cellWorldSize;
	int pxg = static_cast<int>((playerPos.x + worldOffsetX) / cellWorldSize);
	int pzg = static_cast<int>((playerPos.z + worldOffsetZ) / cellWorldSize);
	pxg = std::clamp(pxg, 0, regionW - 1);
	pzg = std::clamp(pzg, 0, regionH - 1);

	// draw small square for player
	int centerX = pxg * pxPerCellX + pxPerCellX / 2;
	int centerY = pzg * pxPerCellY + pxPerCellY / 2;
	int radius = max(1, min(pxPerCellX, pxPerCellY) / 2);

	for (int y = centerY - radius; y <= centerY + radius; ++y)
	{
		if (y < 0 || y >= m_height) continue;
		uint32_t* row = reinterpret_cast<uint32_t*>(dest + y * rowPitch);
		for (int x = centerX - radius; x <= centerX + radius; ++x)
		{
			if (x < 0 || x >= m_width) continue;
			row[x] = playerCol;
		}
	}

	m_pContext->Unmap(m_pTexture, 0);
}