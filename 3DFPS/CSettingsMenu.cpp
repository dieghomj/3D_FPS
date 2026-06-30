#include "CSettingsMenu.h"
#include "CSettings.h"
#include "CFont.h"
#include "CSoundManager.h"
#include "Global.h"
#include <tchar.h>
#include <windows.h>

// スライダーバーのセル数.
static constexpr int BAR_CELLS = 20;

CSettingsMenu::CSettingsMenu()
	: m_selected(ROW_VOLUME)
{
}

// 選択状態を先頭の項目へ初期化する.
void CSettingsMenu::Reset()
{
	m_selected = ROW_VOLUME;
}

// 値スライダーのバー文字列を生成する（filledRatio は 0.0～1.0）.
void CSettingsMenu::BuildBar(float filledRatio, TCHAR* outBuffer, int bufferCount) const
{
	if (filledRatio < 0.f) { filledRatio = 0.f; }
	if (filledRatio > 1.f) { filledRatio = 1.f; }

	int filled = static_cast<int>(filledRatio * BAR_CELLS + 0.5f);

	int idx = 0;
	outBuffer[idx++] = _T('[');
	for (int i = 0; i < BAR_CELLS && idx < bufferCount - 2; ++i)
	{
		outBuffer[idx++] = (i < filled) ? _T('#') : _T('-');
	}
	outBuffer[idx++] = _T(']');
	outBuffer[idx] = _T('\0');
}

// 入力を処理し設定値を更新する.
bool CSettingsMenu::Update()
{
	// 項目選択（上下）.
	if (GetAsyncKeyState(VK_UP) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_selected--;
		if (m_selected < 0) { m_selected = ROW_COUNT - 1; }
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
		m_selected++;
		if (m_selected >= ROW_COUNT) { m_selected = 0; }
	}

	// 値変更（左右）.
	bool changed = false;
	int dir = 0;
	if (GetAsyncKeyState(VK_LEFT)  & 0x0001) { dir = -1; }
	if (GetAsyncKeyState(VK_RIGHT) & 0x0001) { dir = +1; }

	if (dir != 0)
	{
		if (m_selected == ROW_VOLUME)
		{
			CSettings::AddVolume(dir * CSettings::VOLUME_STEP);
			// 音量変更を即座に全サウンドへ反映する.
			CSoundManager::ApplyMasterVolume();
			changed = true;
		}
		else if (m_selected == ROW_SENSE)
		{
			CSettings::AddSensitivity(dir * CSettings::SENSE_STEP);
			// 感度は CScene が毎フレーム参照するため、ここでの反映は不要.
			changed = true;
		}
	}
	if (changed)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Select);
	}

	// 決定・戻る（ENTER で戻る項目を選択、または ESC）.
	if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
	{
		CSoundManager::PlaySE(CSoundManager::SE_Decide);
		CSettings::Save();
		return true;
	}
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		if (m_selected == ROW_BACK)
		{
			CSoundManager::PlaySE(CSoundManager::SE_Decide);
			CSettings::Save();
			return true;
		}
	}

	return false;
}

// 設定メニューを描画する.
void CSettingsMenu::Draw(CFont* pFont, float baseY)
{
	if (pFont == nullptr) { return; }

	const float labelX  = static_cast<float>(WND_W / 2 - 300);
	const float rowGap  = 70.0f;
	const float fontSz  = 34.0f;

	pFont->SetAlpha(1.0f);

	// 各項目（選択中は強調色）.
	TCHAR line[128];
	TCHAR bar[64];

	// 音量.
	if (m_selected == ROW_VOLUME) { pFont->SetColor(1.0f, 0.2f, 0.06f); }
	else                          { pFont->SetColor(1.0f, 1.0f, 1.0f); }
	BuildBar(static_cast<float>(CSettings::GetVolume()) / static_cast<float>(CSettings::VOLUME_MAX), bar, 64);
	_stprintf_s(line, _T("VOLUME       %s  %d"), bar, CSettings::GetVolume());
	pFont->Render(line, static_cast<int>(labelX), static_cast<int>(baseY), fontSz);

	// マウス感度.
	if (m_selected == ROW_SENSE) { pFont->SetColor(1.0f, 0.2f, 0.06f); }
	else                         { pFont->SetColor(1.0f, 1.0f, 1.0f); }
	float senseRatio = (CSettings::GetSensitivity() - CSettings::SENSE_MIN) /
		(CSettings::SENSE_MAX - CSettings::SENSE_MIN);
	BuildBar(senseRatio, bar, 64);
	_stprintf_s(line, _T("MOUSE SENSE  %s  %.2f"), bar, CSettings::GetSensitivity());
	pFont->Render(line, static_cast<int>(labelX), static_cast<int>(baseY + rowGap), fontSz);

	// 戻る.
	if (m_selected == ROW_BACK) { pFont->SetColor(1.0f, 0.2f, 0.06f); }
	else                        { pFont->SetColor(1.0f, 1.0f, 1.0f); }
	pFont->Render(_T("> BACK"), static_cast<int>(labelX), static_cast<int>(baseY + rowGap * 2.f), fontSz);

	// 操作説明.
	pFont->SetColor(0.7f, 0.7f, 0.7f);
	pFont->Render(_T("UP/DOWN: Select   LEFT/RIGHT: Adjust   ENTER/ESC: Back"),
		static_cast<int>(WND_W / 2 - 360), static_cast<int>(baseY + rowGap * 3.f), 26.0f);
}
