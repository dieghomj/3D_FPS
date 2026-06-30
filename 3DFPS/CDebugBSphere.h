#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <vector>

class CDebugBSphere
{
public:
    CDebugBSphere();
    ~CDebugBSphere();

    HRESULT Init(ID3D11Device* device);
    void    Release();

    void DrawSphere(ID3D11DeviceContext* ctx,
        const D3DXMATRIX& mView,
        const D3DXMATRIX& mProj,
        const D3DXVECTOR3& center,
        float radius,
        const D3DXVECTOR4& color);

private:
    struct VERTEX { D3DXVECTOR3 Pos; D3DXVECTOR4 Color; };
    struct CBUFFER_VP { D3DXMATRIX mWVP; };

    HRESULT CreateShader(ID3D11Device* device);
    HRESULT CreateBuffers(ID3D11Device* device, int maxVerts);
    void    ApplyVP(ID3D11DeviceContext* ctx, const D3DXMATRIX& mVP);
    void    AppendRing(std::vector<VERTEX>& out, const D3DXVECTOR3& c, float r,
        const D3DXVECTOR4& color, int segments, int axis); // 0:XY,1:XZ,2:YZ

private:
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
    ID3D11InputLayout* m_pLayout = nullptr;
    ID3D11Buffer* m_pCBuffer = nullptr;
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    int                 m_MaxVBVerts = 0;

    std::vector<VERTEX> m_TmpVerts;
};