#include "stdafx.h"
#include "CGameOver.h"
#include "CGameStats.h"

CGameOver::CGameOver(CDirectX9& dx9, CDirectX11& dx11, HWND hWnd, CTime& time, CSceneManager& manager)
	: CScene(dx9, dx11, hWnd, time, manager)
	, m_Font(nullptr)
	, m_pBG(nullptr)
	, m_pBGSprite(nullptr)
	, m_pCursor(nullptr)
	, m_pCursorSprite(nullptr)
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
	SAFE_DELETE(m_pCursor);
	SAFE_DELETE(m_pCursorSprite);
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
	m_pCursor = new CUIObject();
	m_pCursorSprite = new CSprite2D();
	m_pFade = new CUIObject();
	m_pFadeSprite = new CSprite2D();
}

HRESULT CGameOver::LoadData()
{
	if (FAILED(m_Font->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	// 背景スプライト（メニューと同じ）.
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

	// カーソルスプライト.
	CSprite2D::SPRITE_STATE CursorSS = {
		{32, 32},
		{512, 512},
		{512, 512},
	};

	if (FAILED(m_pCursorSprite->Init(*m_pDx11,
		_T("Data\\Texture\\Cross.png"), CursorSS)))
	{
		return E_FAIL;
	}
	m_pCursor->AttachSprite(*m_pCursorSprite);

	// フェード用オーバーレイ.
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

	// 背景スクロールアニメーションを更新.
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

	// カーソル位置を更新.
	if (m_pCursor)
	{
		D3DXVECTOR3 cursorPos;
		cursorPos.x = m_mousePos.x;
		cursorPos.y = m_mousePos.y;
		cursorPos.z = 1.0f;
		m_pCursor->SetPosition(cursorPos);
		m_pCursor->Update();
	}

	// フェード遷移を処理.
	if (m_IsFading)
	{
		m_FadeAlpha += m_FadeSpeed;
		if (m_FadeAlpha >= 1.0f)
		{
			m_FadeAlpha = 1.0f;

			if (m_GoToRetry)
			{
				// 統計をリセットしてゲームを再開.
				CGameStats::Reset();
				m_pManager->ChangeScene("GAME");
			}
			else
			{
				// メインメニューへ移動.
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

	// 選択肢に対するマウスのホバー/クリック.
	float optionX = static_cast<float>(WND_W / 2 - 200);
	float optionWidth = 400.0f;
	float optionHeight = 45.0f;
	float optionY = static_cast<float>(WND_H / 2 + 20);

	auto isInside = [&](float x, float y, float w, float h)
	{
		return (m_mousePos.x >= x && m_mousePos.x <= x + w && m_mousePos.y >= y && m_mousePos.y <= y + h);
	};

	if (isInside(optionX, optionY - 10.0f, optionWidth, optionHeight))
	{
		m_SelectedOption = GAMEOVER_OPTION_RETRY;
		if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		{
			CSoundManager::PlaySE(CSoundManager::SE_Decide);
			m_IsFading = true;
			m_FadeAlpha = 0.0f;
			m_GoToRetry = true;
			if (m_pFadeSprite)
			{
				m_pFadeSprite->SetAlpha(0.0f);
			}
		}
	}
	else if (isInside(optionX, optionY + 40.0f, optionWidth, optionHeight))
	{
		m_SelectedOption = GAMEOVER_OPTION_MAIN_MENU;
		if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		{
			CSoundManager::PlaySE(CSoundManager::SE_Decide);
			m_IsFading = true;
			m_FadeAlpha = 0.0f;
			m_GoToRetry = false;
			if (m_pFadeSprite)
			{
				m_pFadeSprite->SetAlpha(0.0f);
			}
		}
	}

	// 選択肢を移動.
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

	// 選択を確定.
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

	// スクロールする背景を描画.
	m_pBG->Draw();

	if (m_pCursor)
	{
		m_pCursor->Draw();
	}

	m_Font->SetAlpha(1.0f);

	// ゲームオーバーのタイトル.
	m_Font->SetColor(1.0f, 0.15f, 0.1f);
	m_Font->Render(_T("GAME OVER"), static_cast<float>(WND_W / 2 - 180), static_cast<float>(WND_H / 2 - 120), 80.0f);

	// ナビゲーション選択肢.
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

	// 操作説明.
	m_Font->SetColor(0.7f, 0.7f, 0.7f);
	m_Font->Render(_T("UP/DOWN to select, ENTER to confirm"), static_cast<float>(WND_W / 2 - 180), static_cast<float>(WND_H - 50), 24.0f);

	// フェード用オーバーレイを最後に描画.
	if (m_pFade)
	{
		m_pFade->Draw();
	}
}