#include "CMenu.h"
#include "CGameStats.h"

CMenu::CMenu(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pMenuFont	(nullptr)
	, m_pMenuBG		(nullptr)
	, m_pMenuBGSprite(nullptr)
	, m_SelectedOption(MENU_OPTION_START)
	, m_IsFading(false)
	, m_FadeAlpha(0.0f)
	, m_FadeSpeed(0.1f) 
	, m_pFade(nullptr)
	, m_pFadeSprite(nullptr)
{
}

CMenu::~CMenu()
{
	SAFE_DELETE(m_pMenuFont);
	SAFE_DELETE(m_pMenuBG);
}

void CMenu::Create()
{
	// Create font for menu text
	m_pMenuFont = new CFont();
	m_pMenuBG = new CUIObject();
	m_pMenuOption = new CUIObject();
	m_pMenuBGSprite = new CSprite2D;

	// Fade overlay
	m_pFade = new CUIObject();
	m_pFadeSprite = new CSprite2D();
}

HRESULT CMenu::LoadData()
{
	// Initialize font
	if (FAILED(m_pMenuFont->Init(*m_pDx11)))
	{
		return E_FAIL;
	}
	
	CSprite2D::SPRITE_STATE BackGroundSS = {
		{WND_W, WND_H},
		{610,570},
		{604,560},
	};
	
	CSprite2D::SPRITE_STATE MenuBGSS = {
		{150, 60},
		{400,450},
		{360,80},
	};

	if (FAILED(m_pMenuBGSprite->Init(*m_pDx11,
		_T("Data\\Texture\\UI\\MenuBG.png"), BackGroundSS)))
	{
		return E_FAIL;
	}

	CSprite2D::SPRITE_STATE FadeSS = {
	{WND_W, WND_H},  
	{0, 0},          
	{WND_W, WND_H},  
	};

	if (FAILED(m_pFadeSprite->Init(*m_pDx11, _T("Data\\Texture\\Black.png"), FadeSS)))
	{
		m_pFadeSprite->Init(*m_pDx11, _T("Data\\Texture\\UI\\MenuBG.png"), FadeSS);
	}

	m_pMenuBG->AttachSprite(*m_pMenuBGSprite);

	m_pFadeSprite->SetAlpha(0.0f);
	m_pFade->AttachSprite(*m_pFadeSprite);


	return S_OK;

}

void CMenu::Release()
{
}

void CMenu::Start()
{
	m_SelectedOption = MENU_OPTION_START;
	m_IsFading = false;
	m_FadeAlpha = 0.0f;
	if (m_pFadeSprite)
	{
		m_pFadeSprite->SetAlpha(0.0f);
	}
}

void CMenu::Update()
{
	CScene::Update();

	CSoundManager::PlayLoop(CSoundManager::BGM_Title);

	m_pMenuBG->Update();

	if (m_IsFading)
	{
		m_FadeAlpha += m_FadeSpeed;
		if (m_FadeAlpha >= 1.0f)
		{
			m_FadeAlpha = 1.0f;
			CSoundManager::Stop(CSoundManager::BGM_Title);
			m_pManager->ChangeScene("RESULT");
			return;
		}
		if (m_pFadeSprite)
		{
			m_pFadeSprite->SetAlpha(m_FadeAlpha);
		}
		return;
	}

	// Navigate options
	if (GetAsyncKeyState(VK_UP) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_SelectedOption = MENU_OPTION_START;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_SelectedOption = MENU_OPTION_EXIT;
	}

	// Change difficulty in menu with LEFT/RIGHT arrows
	if (GetAsyncKeyState(VK_LEFT) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		CGameStats::DIFFICULTY cur = CGameStats::GetDifficulty();
		if (cur == CGameStats::DIFF_EASY)      CGameStats::SetDifficulty(CGameStats::DIFF_HARD);
		else if (cur == CGameStats::DIFF_NORMAL)CGameStats::SetDifficulty(CGameStats::DIFF_EASY);
		else                                 CGameStats::SetDifficulty(CGameStats::DIFF_NORMAL);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		CGameStats::DIFFICULTY cur = CGameStats::GetDifficulty();
		if (cur == CGameStats::DIFF_EASY)       CGameStats::SetDifficulty(CGameStats::DIFF_NORMAL);
		else if (cur == CGameStats::DIFF_NORMAL)CGameStats::SetDifficulty(CGameStats::DIFF_HARD);
		else                                 CGameStats::SetDifficulty(CGameStats::DIFF_EASY);
	}

	// Select
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Decide);
		if (m_SelectedOption == MENU_OPTION_START)
		{
			// Begin fade and use the selected difficulty stored in base CScene
			m_IsFading = true;
			m_FadeAlpha = 0.0f;
			m_FadeSpeed = 0.1f;
			if (m_pFadeSprite)
			{
				m_pFadeSprite->SetAlpha(0.0f);
			}
		}
		else if (m_SelectedOption == MENU_OPTION_EXIT)
		{
			PostQuitMessage(0);
		}
	}
}

void CMenu::Draw()
{
	m_pDx11->SetDepth(false);
	m_pMenuBG->Draw();

	m_pMenuFont->SetColor(1.0f, 0.1f, 0.05f);
	m_pMenuFont->SetAlpha(1.0f);

	TCHAR titleText[64];
	_stprintf_s(titleText, _T("MYSTERY MAZE"));
	m_pMenuFont->Render(titleText, static_cast<float>(WND_W / 2 - 130), 90.0f, 60.0f);

	if (m_SelectedOption == MENU_OPTION_START)
	{
		m_pMenuFont->SetColor(1.0f, 0.2f, 0.06f);
	}
	else
	{
		m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);
	}
	TCHAR startText[64];
	_stprintf_s(startText, _T("> START GAME"));
	m_pMenuFont->Render(startText, static_cast<float>(WND_W / 2 - 100), static_cast<float>(WND_H / 2 - 20), 40.0f);

	if (m_SelectedOption == MENU_OPTION_EXIT)
	{
		m_pMenuFont->SetColor(1.0f, 0.2f, 0.06f);
	}
	else
	{
		m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);
	}
	TCHAR exitText[64];
	_stprintf_s(exitText, _T("> EXIT"));
	m_pMenuFont->Render(exitText, static_cast<float>(WND_W / 2 - 100), static_cast<float>(WND_H / 2 + 80), 40.0f);

	m_pMenuFont->SetColor(0.7f, 0.7f, 0.7f);
	TCHAR instructText[128];
	_stprintf_s(instructText, _T("Use UP/DOWN to select, LEFT/RIGHT to change difficulty, ENTER to start"));
	m_pMenuFont->Render(instructText, static_cast<float>(WND_W / 2 - 270), static_cast<float>(WND_H - 50), 35.0f);

	// Show current difficulty
	m_pMenuFont->SetColor(0.9f, 0.9f, 0.2f);
	TCHAR diffText[64];
	_stprintf_s(diffText, _T("DIFFICULTY: %s"), DifficultyToText(CGameStats::GetDifficulty()));
	m_pMenuFont->Render(diffText, static_cast<float>(WND_W / 2 - 120), static_cast<float>(WND_H / 2 + 20), 32.0f);

	// Draw fade overlay last so it covers everything
	if (m_pFade)
	{
		m_pFade->Draw();
	}
}
