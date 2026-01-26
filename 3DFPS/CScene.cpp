#include "CDirectX9.h"
#include "CDirectX11.h"

#include "CScene.h"

CScene::CScene(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: m_pDx9			(&pDx9)
	, m_pDx11			(&pDx11)
	, m_hWnd			(hWnd)
	, m_pTime			(&pTime)
	, m_pManager		(&pManager)

	, m_mView			()
	, m_mProj			()

	, m_GlobalLight		()
	, m_Fog				()
	, m_Camera			()
	, m_pCamera			(nullptr)
	, m_pCameraController(nullptr)

	, m_mousePos		({ 0, 0 })
	, m_mouseSeudoPos	({ WND_W / 2,WND_H / 2 })
	, m_mouseBeforePos	({ 0, 0 })
	, m_mouseDelta		({ 0, 0 })
	, m_mouseSense		( 0.1f )

{
	//サウンドデータの読み込み
	CSoundManager::GetInstance()->Load(m_hWnd);

	m_Fog.Mode = D3DFOG_LINEAR;

}

CScene::~CScene()
{

	SAFE_DELETE(m_pCamera);

	//外部で作成しているので、ここでは破棄しない
	m_pManager = nullptr;
	m_hWnd = nullptr;
	m_pDx11 = nullptr;
	m_pDx9 = nullptr;
}

void CScene::Update()
{
	m_SceneInfo.mView = m_mView;
	m_SceneInfo.mProj = m_mProj;
	m_SceneInfo.Camera = m_Camera;
	m_SceneInfo.Fog = m_Fog;
	m_SceneInfo.Light = m_GlobalLight;
	m_SceneInfo.SpotLightNum = static_cast<int>(m_pSpotLightList.size());
	m_SceneInfo.pSpotLightArray = m_pSpotLightList.empty() ? nullptr : &m_pSpotLightList[0];

	UpdateMousePos();

}

int CScene::AddSpotLight(CSpotLight* spotlight)
{
	
	SPOT_LIGHT spotLightInfo;
	spotLightInfo.LightOrigin = D3DXVECTOR4(spotlight->GetPosition(),0.f);
	spotLightInfo.LightColor = D3DXVECTOR4(spotlight->GetColor());
	spotLightInfo.LightDir = D3DXVECTOR4(spotlight->GetDirection(), 0.f);
	spotLightInfo.fIntensity = spotlight->GetIntensity();
	spotLightInfo.fRange = spotlight->GetRange();
	spotLightInfo.fInnerAngle = spotlight->GetInnerAngle();
	spotLightInfo.fOuterAngle = spotlight->GetOuterAngle();

	m_pSpotLightList.push_back(spotLightInfo);
	return m_pSpotLightList.size() - 1;
}

void CScene::UpdateSpotLight(CSpotLight* spotlight)
{
	SPOT_LIGHT spotLightInfo;
	spotLightInfo.LightOrigin = D3DXVECTOR4(spotlight->GetPosition(), 0.f);
	spotLightInfo.LightColor = D3DXVECTOR4(spotlight->GetColor());
	spotLightInfo.LightDir = D3DXVECTOR4(spotlight->GetDirection(), 0.f);
	spotLightInfo.fIntensity = spotlight->GetIntensity();
	spotLightInfo.fRange = spotlight->GetRange();
	spotLightInfo.fInnerAngle = spotlight->GetInnerAngle();
	spotLightInfo.fOuterAngle = spotlight->GetOuterAngle();
	m_pSpotLightList[spotlight->GetSceneIndex()] = spotLightInfo;
}

void CScene::UpdateMousePos()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(m_hWnd, &mousePos);

	if (ShouldLockMouse())
	{
		// Calculate delta from center
		POINT center = { WND_W / 2, WND_H / 2 };
		m_mouseDelta.x = mousePos.x - center.x;
		m_mouseDelta.y = mousePos.y - center.y;
		m_mouseSeudoPos.x += m_mouseDelta.x;
		m_mouseSeudoPos.y += m_mouseDelta.y;
		// Reset cursor to center
		ClientToScreen(m_hWnd, &center);
		SetCursorPos(center.x, center.y);
	}
	else
	{
		// Free mouse movement for menus
		m_mouseDelta.x = mousePos.x - m_mouseBeforePos.x;
		m_mouseDelta.y = mousePos.y - m_mouseBeforePos.y;
		m_mouseBeforePos = mousePos;
		m_mousePos = mousePos;
	}
}

POINT CScene::GetMouseSeudoPos()
{
	return m_mouseSeudoPos;
}

const TCHAR* CScene::DifficultyToText(CGameStats::DIFFICULTY d)
{
	switch (d)
	{
	case CGameStats::DIFF_EASY:   return _T("EASY");
	case CGameStats::DIFF_NORMAL: return _T("NORMAL");
	case CGameStats::DIFF_HARD:   return _T("HARD");
	default:                  return _T("NORMAL");
	}

}