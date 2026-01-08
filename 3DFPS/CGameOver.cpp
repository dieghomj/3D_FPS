#include "stdafx.h"
#include "CGameOver.h"
#include "CGameStats.h"

CGameOver::CGameOver(CDirectX9& dx9, CDirectX11& dx11, HWND hWnd, CTime& time, CSceneManager& manager)
	: CScene(dx9, dx11, hWnd, time, manager)
	, m_Font(nullptr)
	, m_pBG(nullptr)
	, m_pBGSprite(nullptr)
	, m_BGScrollOffset(0.0f)
	, m_BGScrollSpeed(0.0005f)
	, m_pFade(nullptr)
	, m_pFadeSprite(nullptr)
	, m_IsFading(false)
	, m_FadeAlpha(0.0f)
	, m_FadeSpeed(0.05f)
	, m_SelectedOption(GAMEOVER_OPTION_RETRY)
	, m_GoToRetry(false)
{
}

CGameOver::~CGameOver()
{
	SAFE_DELETE(m_Font);
	SAFE_DELETE(m_pBG);
	SAFE_DELETE(m_pBGSprite);
	SAFE_DELETE(m_pFade);
	SAFE_DELETE(m_pFadeSprite);
}

void CGameOver::Release()
{
}

void CGameOver::Create()
{
	m_Font = new CFont();
	m_pBG = new CUIObject();
	m_pBGSprite = new CSprite2D();
	m_pFade = new CUIObject();
	m_pFadeSprite = new CSprite2D();
}

HRESULT CGameOver::LoadData()
{
	if (FAILED(m_Font->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	// Background sprite (same as menu)
	CSprite2D::SPRITE_STATE BackGroundSS = {
		{WND_W, WND_H},
		{610, 570},
		{604, 560},
	};

	if (FAILED(m_pBGSprite->Init(*m_pDx11,
		_T("Data\\Texture\\UI\\MenuBG.png"), BackGroundSS)))
	{
		return E_FAIL;
	}

	m_pBG->AttachSprite(*m_pBGSprite);

	// Fade overlay
	CSprite2D::SPRITE_STATE FadeSS = {
		{WND_W, WND_H},
		{0, 0},
		{WND_W, WND_H},
	};

	if (FAILED(m_pFadeSprite->Init(*m_pDx11, _T("Data\\Texture\\Black.png"), FadeSS)))
	{
		m_pFadeSprite->Init(*m_pDx11, _T("Data\\Texture\\UI\\MenuBG.png"), FadeSS);
	}

	m_pFadeSprite->SetAlpha(0.0f);
	m_pFade->AttachSprite(*m_pFadeSprite);

	return S_OK;
}

void CGameOver::Start()
{
	m_pDx11->SetDepth(false);
	m_SelectedOption = GAMEOVER_OPTION_RETRY;
	m_IsFading = false;
	m_FadeAlpha = 0.0f;
	m_BGScrollOffset = 0.0f;
	m_GoToRetry = false;

	if (m_pFadeSprite)
	{
		m_pFadeSprite->SetAlpha(0.0f);
	}
}

void CGameOver::Update()
{
	CScene::Update();

	// Update background scroll animation
	m_BGScrollOffset += m_BGScrollSpeed;
	if (m_BGScrollOffset >= 1.0f)
	{
		m_BGScrollOffset -= 1.0f;
	}

	if (m_pBGSprite)
	{
		m_pBGSprite->SetUVOffset(m_BGScrollOffset, 0.0f);
	}

	m_pBG->Update();

	// Handle fade transition
	if (m_IsFading)
	{
		m_FadeAlpha += m_FadeSpeed;
		if (m_FadeAlpha >= 1.0f)
		{
			m_FadeAlpha = 1.0f;

			if (m_GoToRetry)
			{
				// Reset stats and restart game
				CGameStats::Reset();
				m_pManager->ChangeScene("GAME");
			}
			else
			{
				// Go to main menu
				CGameStats::Reset();
				m_pManager->ChangeScene("MENU");
			}
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
		m_SelectedOption--;
		if (m_SelectedOption < 0)
			m_SelectedOption = GAMEOVER_OPTION_COUNT - 1;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_SelectedOption++;
		if (m_SelectedOption >= GAMEOVER_OPTION_COUNT)
			m_SelectedOption = 0;
	}

	// Confirm selection
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Decide);
		m_IsFading = true;
		m_FadeAlpha = 0.0f;
		m_GoToRetry = (m_SelectedOption == GAMEOVER_OPTION_RETRY);

		if (m_pFadeSprite)
		{
			m_pFadeSprite->SetAlpha(0.0f);
		}
	}
}

void CGameOver::Draw()
{
	m_pDx11->SetDepth(false);

	// Draw scrolling background
	m_pBG->Draw();

	m_Font->SetAlpha(1.0f);

	// Game Over title
	m_Font->SetColor(1.0f, 0.15f, 0.1f);
	m_Font->Render(_T("GAME OVER"), static_cast<float>(WND_W / 2 - 180), static_cast<float>(WND_H / 2 - 120), 80.0f);

	// Navigation options
	float optionY = static_cast<float>(WND_H / 2 + 20);

	if (m_SelectedOption == GAMEOVER_OPTION_RETRY)
		m_Font->SetColor(1.0f, 0.2f, 0.06f);
	else
		m_Font->SetColor(1.0f, 1.0f, 1.0f);
	m_Font->Render(_T("> RETRY"), static_cast<float>(WND_W / 2 - 70), optionY, 36.0f);

	if (m_SelectedOption == GAMEOVER_OPTION_MAIN_MENU)
		m_Font->SetColor(1.0f, 0.2f, 0.06f);
	else
		m_Font->SetColor(1.0f, 1.0f, 1.0f);
	m_Font->Render(_T("> MAIN MENU"), static_cast<float>(WND_W / 2 - 100), optionY + 50.0f, 36.0f);

	// Instructions
	m_Font->SetColor(0.7f, 0.7f, 0.7f);
	m_Font->Render(_T("UP/DOWN to select, ENTER to confirm"), static_cast<float>(WND_W / 2 - 180), static_cast<float>(WND_H - 50), 24.0f);

	// Draw fade overlay last
	if (m_pFade)
	{
		m_pFade->Draw();
	}
}