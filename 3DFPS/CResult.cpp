#include "stdafx.h"
#include "CResult.h"
#include "CMenu.h"	

CResultScene::CResultScene(CDirectX9& dx9, CDirectX11& dx11, HWND hWnd, CTime& time, CSceneManager& manager)
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
	, m_SelectedOption(RESULT_OPTION_LEVEL_SELECT)
	, m_GoToLevelSelect(false)
{
}

CResultScene::~CResultScene()
{
	SAFE_DELETE(m_Font);
	SAFE_DELETE(m_pBG);
	SAFE_DELETE(m_pBGSprite);
	SAFE_DELETE(m_pCursor);
	SAFE_DELETE(m_pCursorSprite);
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
	m_pCursor = new CUIObject();
	m_pCursorSprite = new CSprite2D();
	m_pFade = new CUIObject();
	m_pFadeSprite = new CSprite2D();
}

HRESULT CResultScene::LoadData()
{
	if (FAILED(m_Font->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	// 背景スプライト（メニューと同じ）
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

	// カーソルスプライト
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

	// フェード用オーバーレイ
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

	// 最終スコアを計算する
	CGameStats::ComputeScore();
}

void CResultScene::Update()
{
	CScene::Update();

	// 背景のスクロールアニメーションを更新する
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

	// カーソル位置を更新する
	if (m_pCursor)
	{
		D3DXVECTOR3 cursorPos;
		cursorPos.x = m_mousePos.x;
		cursorPos.y = m_mousePos.y;
		cursorPos.z = 1.0f;
		m_pCursor->SetPosition(cursorPos);
		m_pCursor->Update();
	}

	// フェードによる画面遷移を処理する
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

	// 選択肢の操作
	float optionX = static_cast<float>(WND_W / 2 - 200);
	float optionWidth = 400.0f;
	float optionHeight = 40.0f;
	float optionY = static_cast<float>(WND_H - 130);

	auto isInside = [&](float x, float y, float w, float h)
	{
		return (m_mousePos.x >= x && m_mousePos.x <= x + w && m_mousePos.y >= y && m_mousePos.y <= y + h);
	};

	if (isInside(optionX, optionY - 10.0f, optionWidth, optionHeight))
	{
		m_SelectedOption = RESULT_OPTION_LEVEL_SELECT;
		if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		{
			CSoundManager::PlaySE(CSoundManager::SE_Decide);
			m_IsFading = true;
			m_FadeAlpha = 0.0f;
			m_GoToLevelSelect = true;
			if (m_pFadeSprite)
			{
				m_pFadeSprite->SetAlpha(0.0f);
			}
		}
	}
	else if (isInside(optionX, optionY + 30.0f, optionWidth, optionHeight))
	{
		m_SelectedOption = RESULT_OPTION_MAIN_MENU;
		if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		{
			CSoundManager::PlaySE(CSoundManager::SE_Decide);
			m_IsFading = true;
			m_FadeAlpha = 0.0f;
			m_GoToLevelSelect = false;
			if (m_pFadeSprite)
			{
				m_pFadeSprite->SetAlpha(0.0f);
			}
		}
	}

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

	// 選択を確定する
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

	// スクロールする背景を描画する
	m_pBG->Draw();

	if (m_pCursor)
	{
		m_pCursor->Draw();
	}

	m_Font->SetAlpha(1.0f);

	// 最高スコアを最上部に表示する
	m_Font->SetColor(1.0f, 0.85f, 0.0f);
	TCHAR highScoreBuf[64];
	_stprintf_s(highScoreBuf, _T("HIGHEST SCORE: %d"), CGameStats::HighestScore);
	m_Font->Render(highScoreBuf, static_cast<float>(WND_W / 2 - 180), 30.0f, 36.0f);

	// タイトル
	m_Font->SetColor(1.0f, 0.1f, 0.05f);
	m_Font->Render(_T("RESULT"), static_cast<float>(WND_W / 2 - 80), 90.0f, 60.0f);

	// 各種統計
	m_Font->SetColor(1.0f, 1.0f, 1.0f);
	TCHAR buf[128];
	float startY = 170.0f;
	float lineHeight = 45.0f;
	float labelX = static_cast<float>(WND_W / 2 - 200);

	// 倒した敵の数
	_stprintf_s(buf, _T("Enemies Killed: %d"), CGameStats::EnemiesKilled);
	m_Font->Render(buf, labelX, startY, 32.0f);

	// 死亡回数
	_stprintf_s(buf, _T("Deaths: %d"), CGameStats::DeathCounter);
	m_Font->Render(buf, labelX, startY + lineHeight, 32.0f);

	// 残り時間
	unsigned long remainingMs = CGameStats::GetRemainingTimeMs();
	float remainingSec = remainingMs / 1000.0f;
	int remMin = static_cast<int>(remainingSec) / 60;
	int remSec = static_cast<int>(remainingSec) % 60;
	_stprintf_s(buf, _T("Remaining Time: %02d:%02d"), remMin, remSec);
	m_Font->Render(buf, labelX, startY + lineHeight * 3, 32.0f);

	// コンボスコア
	m_Font->SetColor(0.5f, 1.0f, 0.5f);
	_stprintf_s(buf, _T("Combo Bonus: +%d"), CGameStats::ComboScore);
	m_Font->Render(buf, labelX, startY + lineHeight * 4, 32.0f);

	// 合計スコア（大きく、強調して表示）
	m_Font->SetColor(1.0f, 0.3f, 0.1f);
	_stprintf_s(buf, _T("TOTAL SCORE: %d"), CGameStats::Score);
	m_Font->Render(buf, labelX - 20, startY + lineHeight * 5 + 20, 42.0f);

	// ハイスコア更新の表示
	if (CGameStats::Score >= CGameStats::HighestScore && CGameStats::Score > 0)
	{
		m_Font->SetColor(1.0f, 0.85f, 0.0f);
		m_Font->Render(_T("NEW HIGH SCORE!"), static_cast<float>(WND_W / 2 - 140), startY + lineHeight * 6 + 30, 36.0f);
	}

	// 操作の選択肢
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

	// 操作説明
	m_Font->SetColor(0.7f, 0.7f, 0.7f);
	m_Font->Render(_T("UP/DOWN to select, ENTER to confirm"), static_cast<float>(WND_W / 2 - 180), static_cast<float>(WND_H - 30), 24.0f);

	// フェード用オーバーレイを最後に描画する
	if (m_pFade)
	{
		m_pFade->Draw();
	}
}