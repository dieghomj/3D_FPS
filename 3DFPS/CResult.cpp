#include "stdafx.h"
#include "CResult.h"
#include "CMenu.h"	

CResultScene::CResultScene(CDirectX9& dx9, CDirectX11& dx11, HWND hWnd, CTime& time, CSceneManager& manager)
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
	, m_SelectedOption(RESULT_OPTION_LEVEL_SELECT)
	, m_GoToLevelSelect(false)
{
}

CResultScene::~CResultScene()
{
	SAFE_DELETE(m_Font);
	SAFE_DELETE(m_pBG);
	SAFE_DELETE(m_pBGSprite);
	SAFE_DELETE(m_pFade);
	SAFE_DELETE(m_pFadeSprite);
}

void CResultScene::Release()
{
}

void CResultScene::Create()
{
	m_Font = new CFont();
	m_pBG = new CUIObject();
	m_pBGSprite = new CSprite2D();
	m_pFade = new CUIObject();
	m_pFadeSprite = new CSprite2D();
}

HRESULT CResultScene::LoadData()
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

void CResultScene::Start()
{
	m_pDx11->SetDepth(false);
	m_SelectedOption = RESULT_OPTION_LEVEL_SELECT;
	m_IsFading = false;
	m_FadeAlpha = 0.0f;
	m_BGScrollOffset = 0.0f;
	m_GoToLevelSelect = false;

	if (m_pFadeSprite)
	{
		m_pFadeSprite->SetAlpha(0.0f);
	}

	// Compute final score
	CGameStats::ComputeScore();
}

void CResultScene::Update()
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
			CGameStats::Reset();

			if (m_GoToLevelSelect)
			{
				CMenu::s_OpenToLevelSelect = true;
				m_pManager->ChangeScene("MENU");
			}
			else
			{
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
			m_SelectedOption = RESULT_OPTION_COUNT - 1;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_SelectedOption++;
		if (m_SelectedOption >= RESULT_OPTION_COUNT)
			m_SelectedOption = 0;
	}

	// Confirm selection
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Decide);
		m_IsFading = true;
		m_FadeAlpha = 0.0f;
		m_GoToLevelSelect = (m_SelectedOption == RESULT_OPTION_LEVEL_SELECT);

		if (m_pFadeSprite)
		{
			m_pFadeSprite->SetAlpha(0.0f);
		}
	}
}

void CResultScene::Draw()
{
	m_pDx11->SetDepth(false);

	// Draw scrolling background
	m_pBG->Draw();

	m_Font->SetAlpha(1.0f);

	// Highest score at the top
	m_Font->SetColor(1.0f, 0.85f, 0.0f);
	TCHAR highScoreBuf[64];
	_stprintf_s(highScoreBuf, _T("HIGHEST SCORE: %d"), CGameStats::HighestScore);
	m_Font->Render(highScoreBuf, static_cast<float>(WND_W / 2 - 180), 30.0f, 36.0f);

	// Title
	m_Font->SetColor(1.0f, 0.1f, 0.05f);
	m_Font->Render(_T("RESULT"), static_cast<float>(WND_W / 2 - 80), 90.0f, 60.0f);

	// Stats
	m_Font->SetColor(1.0f, 1.0f, 1.0f);
	TCHAR buf[128];
	float startY = 170.0f;
	float lineHeight = 45.0f;
	float labelX = static_cast<float>(WND_W / 2 - 200);

	// Enemies killed
	_stprintf_s(buf, _T("Enemies Killed: %d"), CGameStats::EnemiesKilled);
	m_Font->Render(buf, labelX, startY, 32.0f);

	// Deaths
	_stprintf_s(buf, _T("Deaths: %d"), CGameStats::DeathCounter);
	m_Font->Render(buf, labelX, startY + lineHeight, 32.0f);

	// Remaining time
	unsigned long remainingMs = CGameStats::GetRemainingTimeMs();
	float remainingSec = remainingMs / 1000.0f;
	int remMin = static_cast<int>(remainingSec) / 60;
	int remSec = static_cast<int>(remainingSec) % 60;
	_stprintf_s(buf, _T("Remaining Time: %02d:%02d"), remMin, remSec);
	m_Font->Render(buf, labelX, startY + lineHeight * 3, 32.0f);

	// Combo score
	m_Font->SetColor(0.5f, 1.0f, 0.5f);
	_stprintf_s(buf, _T("Combo Bonus: +%d"), CGameStats::ComboScore);
	m_Font->Render(buf, labelX, startY + lineHeight * 4, 32.0f);

	// Total score (larger, highlighted)
	m_Font->SetColor(1.0f, 0.3f, 0.1f);
	_stprintf_s(buf, _T("TOTAL SCORE: %d"), CGameStats::Score);
	m_Font->Render(buf, labelX - 20, startY + lineHeight * 5 + 20, 42.0f);

	// New high score indicator
	if (CGameStats::Score >= CGameStats::HighestScore && CGameStats::Score > 0)
	{
		m_Font->SetColor(1.0f, 0.85f, 0.0f);
		m_Font->Render(_T("NEW HIGH SCORE!"), static_cast<float>(WND_W / 2 - 140), startY + lineHeight * 6 + 30, 36.0f);
	}

	// Navigation options
	float optionY = static_cast<float>(WND_H - 130);

	if (m_SelectedOption == RESULT_OPTION_LEVEL_SELECT)
		m_Font->SetColor(1.0f, 0.2f, 0.06f);
	else
		m_Font->SetColor(1.0f, 1.0f, 1.0f);
	m_Font->Render(_T("> LEVEL SELECT"), static_cast<float>(WND_W / 2 - 110), optionY, 32.0f);

	if (m_SelectedOption == RESULT_OPTION_MAIN_MENU)
		m_Font->SetColor(1.0f, 0.2f, 0.06f);
	else
		m_Font->SetColor(1.0f, 1.0f, 1.0f);
	m_Font->Render(_T("> MAIN MENU"), static_cast<float>(WND_W / 2 - 100), optionY + 40.0f, 32.0f);

	// Instructions
	m_Font->SetColor(0.7f, 0.7f, 0.7f);
	m_Font->Render(_T("UP/DOWN to select, ENTER to confirm"), static_cast<float>(WND_W / 2 - 180), static_cast<float>(WND_H - 30), 24.0f);

	// Draw fade overlay last
	if (m_pFade)
	{
		m_pFade->Draw();
	}
}