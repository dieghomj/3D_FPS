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
	// メニューテキスト用のフォントを作成する.
	m_pMenuFont = new CFont();
	m_pMenuBG = new CUIObject();
	m_pMenuOption = new CUIObject();
	m_pMenuBGSprite = new CSprite2D;
	m_pCursorSprite = new CSprite2D();
	m_pCursor = new CUIObject();
	// フェード用のオーバーレイ.
	m_pFade = new CUIObject();
	m_pFadeSprite = new CSprite2D();
}

HRESULT CMenu::LoadData()
{
	// フォントを初期化する.
	if (FAILED(m_pMenuFont->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	CSprite2D::SPRITE_STATE BackGroundSS = {
		{WND_W, WND_H},
		{1024,1024},
		{1024,1024},
	};

	CSprite2D::SPRITE_STATE MenuBGSS = {
		{150, 60},
		{400,450},
		{360,80},
	};

	CSprite2D::SPRITE_STATE CursorSS = {
		{32, 32},
		{512, 512},
		{512, 512},
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

	if(FAILED(m_pCursorSprite->Init(*m_pDx11,
		_T("Data\\Texture\\Cross.png"), CursorSS)))
	{
		return E_FAIL;
	}

	m_pMenuBG->AttachSprite(*m_pMenuBGSprite);
	m_pCursor->AttachSprite(*m_pCursorSprite);
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
		CMenu::s_OpenToLevelSelect = false; // フラグをリセットする.
	}
	else
	{
		m_MenuState = STATE_MAIN_MENU;
	}

	m_settingsMenu.Reset();
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

	CSoundManager::PlayLoop(CSoundManager::BGM_Title);

	m_BGScrollOffset += m_BGScrollSpeed;
	if (m_BGScrollOffset >= 1.0f)
	{
		m_BGScrollOffset -= 1.0f;  // シームレスにループするよう折り返す.
	}

	// 背景スプライトにスクロールオフセットを適用する.
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
	case STATE_SETTINGS:
		UpdateSettings();
		break;
	}

	// 選択中の項目に合わせてカーソル位置を更新する.
	D3DXVECTOR3 cursorPos;
	cursorPos.x = m_mousePos.x;
	cursorPos.y = m_mousePos.y;
	cursorPos.z = 1.0f;
	m_pCursor->SetPosition(cursorPos);
	CScene::Update();
	
}

void CMenu::UpdateMainMenu()
{
	// マウスホバーによる選択.
	float optionX = static_cast<float>(WND_W / 2 - 160);
	float optionWidth = 320.0f;
	float optionHeight = 50.0f;

	// 各項目のY座標（DrawMainMenu と一致させること）.
	float optionY[MENU_OPTION_COUNT];
	optionY[MENU_OPTION_START]    = static_cast<float>(WND_H / 2 - 40);
	optionY[MENU_OPTION_SETTINGS] = static_cast<float>(WND_H / 2 + 40);
	optionY[MENU_OPTION_EXIT]     = static_cast<float>(WND_H / 2 + 120);

	auto isInside = [&](float x, float y, float w, float h)
	{
		return (m_mousePos.x >= x && m_mousePos.x <= x + w && m_mousePos.y >= y && m_mousePos.y <= y + h);
	};

	// 選択中の項目を決定する（ENTER 押下時とマウスクリック時に共通で使用）.
	auto confirm = [&](int option)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Decide);
		if (option == MENU_OPTION_START)
		{
			// レベル選択画面へ移動する.
			m_MenuState = STATE_LEVEL_SELECT;
			m_SelectedLevel = LEVEL_1;
		}
		else if (option == MENU_OPTION_SETTINGS)
		{
			// 設定画面へ移動する.
			m_MenuState = STATE_SETTINGS;
			m_settingsMenu.Reset();
		}
		else if (option == MENU_OPTION_EXIT)
		{
			PostQuitMessage(0);
		}
	};

	// マウスホバーで項目を選択し、クリックで決定する.
	for (int i = 0; i < MENU_OPTION_COUNT; ++i)
	{
		if (isInside(optionX, optionY[i] - 20.f, optionWidth, optionHeight))
		{
			m_SelectedOption = i;
			if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
			{
				confirm(i);
				return;
			}
		}
	}

	// キーボードで項目を移動する.
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

	// 決定.
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		confirm(m_SelectedOption);
	}
}

// 設定画面の更新.
void CMenu::UpdateSettings()
{
	// 共通設定メニューが「戻る」を返したらメインメニューへ戻る.
	if (m_settingsMenu.Update())
	{
		m_MenuState = STATE_MAIN_MENU;
		m_SelectedOption = MENU_OPTION_SETTINGS;
	}
}

void CMenu::UpdateLevelSelect()
{
	float optionX = static_cast<float>(WND_W / 2 - 200);
	float optionWidth = 400.0f;
	float optionHeight = 40.0f;
	float startY = static_cast<float>(WND_H / 2 - 80);
	float backY = startY + (LEVEL_COUNT * 50.0f) + 80.0f;

	auto isInside = [&](float x, float y, float w, float h)
	{
		return (m_mousePos.x >= x && m_mousePos.x <= x + w && m_mousePos.y >= y && m_mousePos.y <= y + h);
	};

	// マウスホバーでレベル項目を選択する.
	for (int i = 0; i < LEVEL_COUNT; i++)
	{
		float y = startY + (i * 50.0f) - 15.0f;
		if (isInside(optionX, y, optionWidth, optionHeight))
		{
			m_SelectedLevel = i;
			if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
			{
				CSoundManager::PlaySE(CSoundManager::SE_Decide);
				if (CGameStats::UnlockedLevel[m_SelectedLevel])
				{
					CGameStats::LevelSelection = m_SelectedLevel;
					m_IsFading = true;
					m_FadeAlpha = 0.0f;
					m_FadeSpeed = 0.1f;
					if (m_pFadeSprite)
					{
						m_pFadeSprite->SetAlpha(0.0f);
					}
				}
				return;
			}
		}
	}

	// 「戻る」項目のホバー／クリック.
	if (isInside(optionX, backY - 15.0f, optionWidth, optionHeight))
	{
		m_SelectedLevel = LEVEL_BACK;
		if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		{
			CSoundManager::PlaySE(CSoundManager::SE_Select);
			m_MenuState = STATE_MAIN_MENU;
			return;
		}
	}


	if (GetAsyncKeyState('G') & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Alarm);
		for(int i = 0; i < LEVEL_COUNT; i++)
		{
			CGameStats::UnlockedLevel[i] = true;
		}
		return;
	}

	// レベル間を移動する（UP/DOWN）。「戻る」項目も含める.
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

	// メインメニューへ戻る（ESCショートカット）.
	if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_MenuState = STATE_MAIN_MENU;
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Decide);

		if (m_SelectedLevel == LEVEL_BACK)
		{
			m_MenuState = STATE_MAIN_MENU;
			return;
		}

		if (CGameStats::UnlockedLevel[m_SelectedLevel])
		{
			CGameStats::LevelSelection = m_SelectedLevel;
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

	// レベル名.
	const TCHAR* levelNames[] = {	_T("TUTORIAL"), 
									_T("LEVEL 1"),
									_T("LEVEL 2")};

	float startY = static_cast<float>(WND_H / 2 - 80);
	for (int i = 0; i < LEVEL_COUNT; i++)
	{
		if (m_SelectedLevel == i)
			m_pMenuFont->SetColor(1.0f, 0.2f, 0.06f);
		else
			m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);

		TCHAR levelText[64];
		if(CGameStats::UnlockedLevel[i])
			_stprintf_s(levelText, _T("> %s"), levelNames[i]);
		else
			_stprintf_s(levelText, _T("> %s - [LOCKED]"), levelNames[i]);
		m_pMenuFont->Render(levelText, static_cast<float>(WND_W / 2 - 120), startY + (i * 50.0f), 35.0f);
	}

	// 「戻る」項目.
	float backY = startY + (LEVEL_COUNT * 50.0f) + 80.0f;
	if (m_SelectedLevel == LEVEL_BACK)
		m_pMenuFont->SetColor(1.0f, 0.2f, 0.06f);
	else
		m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);
	m_pMenuFont->Render(_T("> BACK"), static_cast<float>(WND_W / 2 - 60), backY, 35.0f);

	// 操作説明.
	m_pMenuFont->SetColor(0.7f, 0.7f, 0.7f);
	TCHAR instructText[128];
	_stprintf_s(instructText, _T("UP/DOWN: Select | ENTER: Confirm | ESC: Back"));
	m_pMenuFont->Render(instructText, static_cast<float>(WND_W / 2 - 310), static_cast<float>(WND_H - 50), 28.0f);
}

void CMenu::Draw()
{
	m_pDx11->SetDepth(false);
	m_pMenuBG->Draw();
	m_pCursor->Draw();

	switch (m_MenuState)
	{
	case STATE_MAIN_MENU:
		DrawMainMenu();
		break;
	case STATE_LEVEL_SELECT:
		DrawLevelSelect();
		break;
	case STATE_SETTINGS:
		DrawSettings();
		break;
	}

	// フェードのオーバーレイは最後に描画し、全体を覆うようにする.
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

	// 各項目（ラベルとY座標は UpdateMainMenu と一致させること）.
	const TCHAR* optionText[MENU_OPTION_COUNT];
	optionText[MENU_OPTION_START]    = _T("> START GAME");
	optionText[MENU_OPTION_SETTINGS] = _T("> SETTINGS");
	optionText[MENU_OPTION_EXIT]     = _T("> EXIT");

	float optionY[MENU_OPTION_COUNT];
	optionY[MENU_OPTION_START]    = static_cast<float>(WND_H / 2 - 40);
	optionY[MENU_OPTION_SETTINGS] = static_cast<float>(WND_H / 2 + 40);
	optionY[MENU_OPTION_EXIT]     = static_cast<float>(WND_H / 2 + 120);

	for (int i = 0; i < MENU_OPTION_COUNT; ++i)
	{
		if (m_SelectedOption == i)
			m_pMenuFont->SetColor(1.0f, 0.2f, 0.06f);
		else
			m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);
		m_pMenuFont->Render(optionText[i], static_cast<float>(WND_W / 2 - 100), optionY[i], 40.0f);
	}

	m_pMenuFont->SetColor(0.7f, 0.7f, 0.7f);
	TCHAR instructText[128];
	_stprintf_s(instructText, _T("Use UP/DOWN to select, ENTER to confirm"));
	m_pMenuFont->Render(instructText, static_cast<float>(WND_W / 2 - 200), static_cast<float>(WND_H - 50), 35.0f);
}

// 設定画面の描画.
void CMenu::DrawSettings()
{
	m_pMenuFont->SetColor(1.0f, 0.1f, 0.05f);
	m_pMenuFont->SetAlpha(1.0f);

	TCHAR titleText[64];
	_stprintf_s(titleText, _T("SETTINGS"));
	m_pMenuFont->Render(titleText, static_cast<float>(WND_W / 2 - 110), 90.0f, 60.0f);

	// 共通設定メニューUIを描画する.
	m_settingsMenu.Draw(m_pMenuFont, static_cast<float>(WND_H / 2 - 60));
}
