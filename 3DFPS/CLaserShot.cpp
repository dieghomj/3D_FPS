#include "CLaserShot.h"

// Shader file path
const TCHAR LASER_SHADER_NAME[] = _T("Data\\Shader\\Sprite3D.hlsl");

CLaserShot::CLaserShot()
	: m_pDx11(nullptr)
	, m_pDevice11(nullptr)
	, m_pContext11(nullptr)
	, m_pVertexShader(nullptr)
	, m_pVertexLayout(nullptr)
	, m_pPixelShader(nullptr)
	, m_pConstantBuffer(nullptr)
	, m_pVertexBuffer(nullptr)
	, m_pTexture(nullptr)
	, m_pSampleLinear(nullptr)
	, m_LaserData()
	, m_Width(0.15f)
	, m_MaxLifeTime(0.1f)
	, m_Color(1.0f, 1.0f, 1.0f, 1.0f)
{
	m_LaserData.isActive = false;
	m_LaserData.lifeTime = 0.0f;
	m_LaserData.maxLifeTime = m_MaxLifeTime;
}

CLaserShot::~CLaserShot()
{
	SAFE_RELEASE(m_pSampleLinear);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);
}

HRESULT CLaserShot::Init(CDirectX11& pDx11, LPCTSTR lpFileName)
{
	m_pDx11 = &pDx11;
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	if (FAILED(CreateShader()))
	{
		return E_FAIL;
	}

	if (FAILED(CreateModel()))
	{
		return E_FAIL;
	}

	if (FAILED(CreateTexture(lpFileName)))
	{
		return E_FAIL;
	}

	if (FAILED(CreateSampler()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CLaserShot::Fire(const D3DXVECTOR3& startPos, const D3DXVECTOR3& endPos)
{
	D3DXVECTOR3 direction = endPos - startPos;
	float length = D3DXVec3Length(&direction);
	D3DXVec3Normalize(&direction, &direction);

	Fire(startPos, direction, length);
}

void CLaserShot::Fire(const D3DXVECTOR3& startPos, const D3DXVECTOR3& direction, float length)
{
	m_LaserData.startPos = startPos;
	m_LaserData.direction = direction;
	m_LaserData.length = length;
	m_LaserData.endPos = startPos + direction * length;
	m_LaserData.lifeTime = m_MaxLifeTime;
	m_LaserData.maxLifeTime = m_MaxLifeTime;
	m_LaserData.isActive = true;
}

void CLaserShot::Update(float deltaTime)
{
	if (!m_LaserData.isActive)
		return;

	m_LaserData.lifeTime -= deltaTime;

	if (m_LaserData.lifeTime <= 0.0f)
	{
		m_LaserData.isActive = false;
		m_LaserData.lifeTime = 0.0f;
	}
}

void CLaserShot::Render(D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	if (!m_LaserData.isActive)
		return;

	// Calculate alpha based on remaining lifetime
	float alpha = m_LaserData.lifeTime / m_LaserData.maxLifeTime;
	
	// Create world matrix for the laser
	D3DXMATRIX mWorld = CreateLaserWorldMatrix(
		m_LaserData.startPos, 
		m_LaserData.direction, 
		m_LaserData.length);

	// Set shaders
	m_pContext11->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext11->PSSetShader(m_pPixelShader, nullptr, 0);

	// Update constant buffer
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cb;

	if (SUCCEEDED(m_pContext11->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		D3DXMATRIX m = mWorld * mView * mProj;
		D3DXMatrixTranspose(&m, &m);
		cb.mWVP = m;

		// Apply color with fading alpha
		cb.vColor = D3DXVECTOR4(m_Color.x, m_Color.y, m_Color.z, m_Color.w * alpha);

		// UV coordinates (use full texture)
		cb.vUV = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pContext11->Unmap(m_pConstantBuffer, 0);
	}

	// Set constant buffer
	m_pContext11->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext11->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// Set vertex buffer
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set input layout and topology
	m_pContext11->IASetInputLayout(m_pVertexLayout);
	m_pContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set texture
	m_pContext11->PSSetSamplers(0, 1, &m_pSampleLinear);
	m_pContext11->PSSetShaderResources(0, 1, &m_pTexture);

	// Enable alpha blending
	m_pDx11->SetAlphaBlend(true);

	// Draw the laser quad
	m_pContext11->Draw(4, 0);

	// Disable alpha blending
	m_pDx11->SetAlphaBlend(false);
}

D3DXMATRIX CLaserShot::CreateLaserWorldMatrix(const D3DXVECTOR3& startPos, 
	const D3DXVECTOR3& direction, float length)
{
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);

	// The laser quad is created with Z as the length direction
	// We need to orient it along the shot direction

	// Scale: width in X, 1 in Y (or width for billboard), length in Z
	D3DXMATRIX mScale;
	D3DXMatrixScaling(&mScale, m_Width, m_Width, length);

	// Create rotation to align Z-axis with direction
	D3DXVECTOR3 defaultDir(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 dir = direction;
	D3DXVec3Normalize(&dir, &dir);

	D3DXMATRIX mRot;
	D3DXMatrixIdentity(&mRot);

	// Calculate rotation axis and angle
	D3DXVECTOR3 rotAxis;
	D3DXVec3Cross(&rotAxis, &defaultDir, &dir);
	float dotProduct = D3DXVec3Dot(&defaultDir, &dir);

	if (D3DXVec3Length(&rotAxis) > 0.0001f)
	{
		D3DXVec3Normalize(&rotAxis, &rotAxis);
		float angle = acosf(dotProduct);
		D3DXMatrixRotationAxis(&mRot, &rotAxis, angle);
	}
	else if (dotProduct < 0)
	{
		// Direction is opposite to default, rotate 180 degrees around any perpendicular axis
		D3DXMatrixRotationY(&mRot, D3DX_PI);
	}

	// Position at the center of the laser (start + half length along direction)
	D3DXVECTOR3 centerPos = startPos + dir * (length * 0.5f);
	D3DXMATRIX mTrans;
	D3DXMatrixTranslation(&mTrans, centerPos.x, centerPos.y, centerPos.z);

	// Combine: scale -> rotate -> translate
	mWorld = mScale * mRot * mTrans;

	return mWorld;
}

HRESULT CLaserShot::CreateShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	// Create vertex shader
	if (FAILED(D3DX11CompileFromFile(
		LASER_SHADER_NAME,
		nullptr, nullptr,
		"VS_Main", "vs_5_0",
		uCompileFlag, 0, nullptr,
		&pCompiledShader, &pErrors, nullptr)))
	{
		_ASSERT_EXPR(false, _T("Laser shader compile failed"));
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice11->CreateVertexShader(
		pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(),
		nullptr,
		&m_pVertexShader)))
	{
		return E_FAIL;
	}

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	if (FAILED(m_pDevice11->CreateInputLayout(
		layout, numElements,
		pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(),
		&m_pVertexLayout)))
	{
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	// Create pixel shader
	if (FAILED(D3DX11CompileFromFile(
		LASER_SHADER_NAME,
		nullptr, nullptr,
		"PS_Main", "ps_5_0",
		uCompileFlag, 0, nullptr,
		&pCompiledShader, &pErrors, nullptr)))
	{
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice11->CreatePixelShader(
		pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(),
		nullptr,
		&m_pPixelShader)))
	{
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	// Create constant buffer
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_CONSTANT_BUFFER);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(m_pDevice11->CreateBuffer(&cb, nullptr, &m_pConstantBuffer)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLaserShot::CreateModel()
{
	// Create a quad that extends along Z axis
	// Width is in X, length in Z
	float w = 0.5f;  // Half width (will be scaled)
	float l = 0.5f;  // Half length (will be scaled)

	// The quad faces outward and extends along Z
	VERTEX vertices[] =
	{
		// Position (X, Y, Z)           // UV
		{ D3DXVECTOR3(-w, 0.0f, -l), D3DXVECTOR2(0.0f, 1.0f) },  // Bottom-left
		{ D3DXVECTOR3(-w, 0.0f,  l), D3DXVECTOR2(0.0f, 0.0f) },  // Top-left
		{ D3DXVECTOR3( w, 0.0f, -l), D3DXVECTOR2(1.0f, 1.0f) },  // Bottom-right
		{ D3DXVECTOR3( w, 0.0f,  l), D3DXVECTOR2(1.0f, 0.0f) }   // Top-right
	};

	UINT uVerMax = sizeof(vertices) / sizeof(vertices[0]);

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX) * uVerMax;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	if (FAILED(m_pDevice11->CreateBuffer(&bd, &InitData, &m_pVertexBuffer)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLaserShot::CreateTexture(LPCTSTR lpFileName)
{
	if (FAILED(D3DX11CreateShaderResourceViewFromFile(
		m_pDevice11,
		lpFileName,
		nullptr, nullptr,
		&m_pTexture,
		nullptr)))
	{
		_ASSERT_EXPR(false, _T("Laser texture load failed"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLaserShot::CreateSampler()
{
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	if (FAILED(m_pDevice11->CreateSamplerState(&samDesc, &m_pSampleLinear)))
	{
		return E_FAIL;
	}

	return S_OK;
}
