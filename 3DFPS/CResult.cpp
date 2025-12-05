#include "stdafx.h"
#include "CResult.h"

CResultScene::CResultScene(CDirectX9& dx9, CDirectX11& dx11, HWND hWnd, CTime& time, CSceneManager& manager)
	: CScene(dx9, dx11, hWnd, time, manager)
	, m_Font(nullptr) {
}

CResultScene::~CResultScene()
{
	SAFE_DELETE(m_Font);
}

void CResultScene::Release()
{
}

void CResultScene::Create()
{
	m_Font = new CFont();
}

HRESULT CResultScene::LoadData()
{
	return m_Font->Init(*m_pDx11);
}

void CResultScene::Start()
{
	m_pDx11->SetDepth(false);
}

void CResultScene::Update()
{
	CScene::Update();

	// Press Enter to restart (or go to title)
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		// Reset stats and go back to gameplay scene
		CGameStats::Reset();
		m_pManager->ChangeScene("MENU"); // adjust to your gameplay scene key
	}
}

void CResultScene::Draw()
{
	m_pDx11->SetDepth(false);

	m_Font->SetColor(1.f, 1.f, 1.f);
	m_Font->SetAlpha(1.f);

	TCHAR buf[128];

	m_Font->Render(_T("RESULT"), WND_W / 2 - 150.f, 120.f, 60.f);

	_stprintf_s(buf, _T("Enemies killed: %d"), CGameStats::EnemiesKilled);
	m_Font->Render(buf, WND_W / 2 - 220.f, 220.f, 36.f);

	float sec = CGameStats::TimeMs / 1000.f;
	float min = sec / 60.f;
	float remSec = sec - (static_cast<int>(min) * 60.f);

	_stprintf_s(buf, _T("Time played: %02.f:%02.f"), min, remSec);
	m_Font->Render(buf, WND_W / 2 - 220.f, 270.f, 36.f);

	_stprintf_s(buf, _T("Score: %d"), CGameStats::Score);
	m_Font->Render(buf, WND_W / 2 - 220.f, 320.f, 42.f);

	m_Font->Render(_T("Press ENTER to play again"), WND_W / 2 - 260.f, 420.f, 32.f);
}
