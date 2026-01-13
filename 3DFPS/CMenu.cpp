#include "CMenu.h"
#include "CGameStats.h"

bool CMenu::s_OpenToLevelSelect = false;

CMenu::CMenu(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pMenuFont(nullptr)
	, m_pMenuBG(nullptr)
	, m_pMenuBGSprite(nullptr)
	, m_MenuState(STATE_MAIN_MENU)
	, m_SelectedOption(MENU_OPTION_START)
	, m_SelectedLevel(LEVEL_1)
	, m_SelectedDifficulty(CGameStats::DIFF_NORMAL)
	, m_IsFading(false)
	, m_FadeAlpha(0.0f)
	, m_FadeSpeed(0.1f)
	, m_pFade(nullptr)
	, m_pFadeSprite(nullptr)
	, m_BGScrollOffset(0.0f)
	, m_BGScrollSpeed(0.0005f)
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
	if (CMenu::s_OpenToLevelSelect)
	{
		m_MenuState = STATE_LEVEL_SELECT;
		m_SelectedLevel = LEVEL_1;
		CMenu::s_OpenToLevelSelect = false; // Reset flag
	}
	else
	{
		m_MenuState = STATE_MAIN_MENU;
	}

	m_SelectedOption = MENU_OPTION_START;
	m_SelectedDifficulty = CGameStats::GetDifficulty();
	m_IsFading = false;
	m_BGScrollOffset = 0.0f;
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

	m_BGScrollOffset += m_BGScrollSpeed;
	if (m_BGScrollOffset >= 1.0f)
	{
		m_BGScrollOffset -= 1.0f;  // Wrap around for seamless loop
	}

	// Apply scroll offset to background sprite
	if (m_pMenuBGSprite)
	{
		m_pMenuBGSprite->SetUVOffset(m_BGScrollOffset, m_BGScrollOffset);
	}

	m_pMenuBG->Update();

	if (m_IsFading)
	{
		m_FadeAlpha += m_FadeSpeed;
		if (m_FadeAlpha >= 1.0f)
		{
			m_FadeAlpha = 1.0f;
			CSoundManager::Stop(CSoundManager::BGM_Title);
			m_pManager->ChangeScene("GAME");
			return;
		}
		if (m_pFadeSprite)
		{
			m_pFadeSprite->SetAlpha(m_FadeAlpha);
		}
		return;
	}

	switch (m_MenuState)
	{
	case STATE_MAIN_MENU:
		UpdateMainMenu();
		break;
	case STATE_LEVEL_SELECT:
		UpdateLevelSelect();
		break;
	}
}

void CMenu::UpdateMainMenu()
{
	// Navigate options
	if (GetAsyncKeyState(VK_UP) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_SelectedOption--;
		if (m_SelectedOption < 0)
			m_SelectedOption = MENU_OPTION_COUNT - 1;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_SelectedOption++;
		if (m_SelectedOption >= MENU_OPTION_COUNT)
			m_SelectedOption = 0;
	}

	// Select
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Decide);
		if (m_SelectedOption == MENU_OPTION_START)
		{
			// Go to level select screen
			m_MenuState = STATE_LEVEL_SELECT;
			m_SelectedLevel = LEVEL_1;
		}
		else if (m_SelectedOption == MENU_OPTION_EXIT)
		{
			PostQuitMessage(0);
		}
	}
}

void CMenu::UpdateLevelSelect()
{
	// Navigate levels (UP/DOWN) - include BACK option
	if (GetAsyncKeyState(VK_UP) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_SelectedLevel--;
		if(m_SelectedLevel == LEVEL_COUNT)
			m_SelectedLevel = LEVEL_BACK - 2;

		if (m_SelectedLevel < 0)
			m_SelectedLevel = LEVEL_BACK;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_SelectedLevel++;
		if (m_SelectedLevel == LEVEL_COUNT)
		{
			m_SelectedLevel++;
		}
		if (m_SelectedLevel > LEVEL_BACK)
			m_SelectedLevel = 0;
	}

	// Change difficulty (LEFT/RIGHT) - only when a level is selected
	//if (m_SelectedLevel < LEVEL_COUNT)
	//{
	//	if (GetAsyncKeyState(VK_LEFT) & 0x0001)
	//	{
	//		CSoundManager::PlaySE(CSoundManager::SE_Select);
	//		m_SelectedDifficulty--;
	//		if (m_SelectedDifficulty < CGameStats::DIFF_EASY)
	//			m_SelectedDifficulty = CGameStats::DIFF_HARD;
	//	}
	//	if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
	//	{
	//		CSoundManager::PlaySE(CSoundManager::SE_Select);
	//		m_SelectedDifficulty++;
	//		if (m_SelectedDifficulty > CGameStats::DIFF_HARD)
	//			m_SelectedDifficulty = CGameStats::DIFF_EASY;
	//	}
	//}

	// Go back to main menu (ESC shortcut)
	if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_MenuState = STATE_MAIN_MENU;
	}

	// Confirm selection
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Decide);

		// Check if BACK is selected
		if (m_SelectedLevel == LEVEL_BACK)
		{
			m_MenuState = STATE_MAIN_MENU;
			return;
		}

		// Apply selections (only for unlocked levels)
		if (m_SelectedLevel == 0)
		{
			CGameStats::SetDifficulty(static_cast<CGameStats::DIFFICULTY>(m_SelectedDifficulty));
			CGameStats::LevelSelection = m_SelectedLevel + 1;

			// Begin fade to game
			m_IsFading = true;
			m_FadeAlpha = 0.0f;
			m_FadeSpeed = 0.1f;
			if (m_pFadeSprite)
			{
				m_pFadeSprite->SetAlpha(0.0f);
			}
		}
	}
}

void CMenu::DrawLevelSelect()
{
	m_pMenuFont->SetColor(1.0f, 0.1f, 0.05f);
	m_pMenuFont->SetAlpha(1.0f);

	TCHAR titleText[64];
	_stprintf_s(titleText, _T("SELECT LEVEL"));
	m_pMenuFont->Render(titleText, static_cast<float>(WND_W / 2 - 140), 90.0f, 60.0f);

	// Level names
	const TCHAR* levelNames[] = { _T("Level 0 - Tutorial"), _T("Level 1 - [LOCKED]"), _T("Level 2 - [LOCKED]"), _T("Level 3 - [LOCKED]")};

	float startY = static_cast<float>(WND_H / 2 - 80);
	for (int i = 0; i < LEVEL_COUNT; i++)
	{
		if (m_SelectedLevel == i)
			m_pMenuFont->SetColor(1.0f, 0.2f, 0.06f);
		else
			m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);

		TCHAR levelText[64];
		_stprintf_s(levelText, _T("> %s"), levelNames[i]);
		m_pMenuFont->Render(levelText, static_cast<float>(WND_W / 2 - 120), startY + (i * 50.0f), 35.0f);
	}

	// Difficulty selector
	//m_pMenuFont->SetColor(0.8f, 0.8f, 0.2f);
	//const TCHAR* diffNames[] = { _T("EASY"), _T("NORMAL"), _T("HARD") };
	//TCHAR diffText[128];
	//_stprintf_s(diffText, _T("< DIFFICULTY: %s >"), diffNames[m_SelectedDifficulty]);
	//m_pMenuFont->Render(diffText, static_cast<float>(WND_W / 2 - 140), startY + (LEVEL_COUNT * 50.0f) + 20.0f, 35.0f);

	// BACK option
	float backY = startY + (LEVEL_COUNT * 50.0f) + 80.0f;
	if (m_SelectedLevel == LEVEL_BACK)
		m_pMenuFont->SetColor(1.0f, 0.2f, 0.06f);
	else
		m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);
	m_pMenuFont->Render(_T("> BACK"), static_cast<float>(WND_W / 2 - 60), backY, 35.0f);

	// Instructions
	m_pMenuFont->SetColor(0.7f, 0.7f, 0.7f);
	TCHAR instructText[128];
	_stprintf_s(instructText, _T("UP/DOWN: Select | ENTER: Confirm | ESC: Back"));
	m_pMenuFont->Render(instructText, static_cast<float>(WND_W / 2 - 310), static_cast<float>(WND_H - 50), 28.0f);
}

void CMenu::Draw()
{
	m_pDx11->SetDepth(false);
	m_pMenuBG->Draw();

	switch (m_MenuState)
	{
	case STATE_MAIN_MENU:
		DrawMainMenu();
		break;
	case STATE_LEVEL_SELECT:
		DrawLevelSelect();
		break;
	}

	// Draw fade overlay last so it covers everything
	if (m_pFade)
	{
		m_pFade->Draw();
	}
}

void CMenu::DrawMainMenu()
{
	m_pMenuFont->SetColor(1.0f, 0.1f, 0.05f);
	m_pMenuFont->SetAlpha(1.0f);

	TCHAR titleText[64];
	_stprintf_s(titleText, _T("FAST ATTACK"));
	m_pMenuFont->Render(titleText, static_cast<float>(WND_W / 2 - 130), 90.0f, 60.0f);

	// Start option
	if (m_SelectedOption == MENU_OPTION_START)
		m_pMenuFont->SetColor(1.0f, 0.2f, 0.06f);
	else
		m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);

	TCHAR startText[64];
	_stprintf_s(startText, _T("> START GAME"));
	m_pMenuFont->Render(startText, static_cast<float>(WND_W / 2 - 100), static_cast<float>(WND_H / 2 - 20), 40.0f);

	// Exit option
	if (m_SelectedOption == MENU_OPTION_EXIT)
		m_pMenuFont->SetColor(1.0f, 0.2f, 0.06f);
	else
		m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);

	TCHAR exitText[64];
	_stprintf_s(exitText, _T("> EXIT"));
	m_pMenuFont->Render(exitText, static_cast<float>(WND_W / 2 - 100), static_cast<float>(WND_H / 2 + 80), 40.0f);

	m_pMenuFont->SetColor(0.7f, 0.7f, 0.7f);
	TCHAR instructText[128];
	_stprintf_s(instructText, _T("Use UP/DOWN to select, ENTER to confirm"));
	m_pMenuFont->Render(instructText, static_cast<float>(WND_W / 2 - 200), static_cast<float>(WND_H - 50), 35.0f);
}
