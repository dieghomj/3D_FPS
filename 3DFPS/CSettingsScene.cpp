#include "CSettingsScene.h"
#include "CSettings.h"
#include "CSoundManager.h"

CSettingsScene::CSettingsScene(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pSettingsFont(nullptr)
	, m_pSettingsBG(nullptr)
	, m_pSettingsBGSprite(nullptr)
	, m_pFade(nullptr)
	, m_pFadeSprite(nullptr)
	, m_mouseSensitivity(1.0f)
	, m_volume(100)
	, m_originalMouseSensitivity(1.0f)
	, m_originalVolume(100)
	, m_selectedOption(OPTION_MOUSE_SENSITIVITY)
	, m_normalColor(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_selectedColor(D3DXVECTOR4(1.0f, 0.5f, 0.0f, 1.0f))
{
}

CSettingsScene::~CSettingsScene()
{
	SAFE_DELETE(m_pSettingsFont);
	SAFE_DELETE(m_pSettingsBG);
}

void CSettingsScene::Create()
{
	m_pSettingsFont = new CFont();
	m_pSettingsBG = new CUIObject();
	m_pSettingsBGSprite = new CSprite2D();
	m_pFade = new CUIObject();
	m_pFadeSprite = new CSprite2D();
}

HRESULT CSettingsScene::LoadData()
{
	// Initialize font
	if (FAILED(m_pSettingsFont->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	CSprite2D::SPRITE_STATE backgroundSS = {
		{WND_W, WND_H},
		{1024, 1024},
		{1024, 1024},
	};

	if (FAILED(m_pSettingsBGSprite->Init(*m_pDx11,
		_T("Data\\Texture\\UI\\MenuBG.png"), backgroundSS)))
	{
		return E_FAIL;
	}

	CSprite2D::SPRITE_STATE fadeSS = {
		{WND_W, WND_H},
		{0, 0},
		{WND_W, WND_H},
	};

	if (FAILED(m_pFadeSprite->Init(*m_pDx11, _T("Data\\Texture\\Black.png"), fadeSS)))
	{
		return E_FAIL;
;	}

	m_pSettingsBG->AttachSprite(*m_pSettingsBGSprite);
	m_pFadeSprite->SetAlpha(0.3f);
	m_pFade->AttachSprite(*m_pFadeSprite);

	return S_OK;
}

void CSettingsScene::Start()
{
	// Load current settings
	m_mouseSensitivity = CSettings::GetMouseSensitivity();
	m_volume = CSettings::GetVolume();

	// Store original values for cancel
	m_originalMouseSensitivity = m_mouseSensitivity;
	m_originalVolume = m_volume;

	m_selectedOption = OPTION_MOUSE_SENSITIVITY;
}

void CSettingsScene::Update()
{
	UpdateSettings();
	CScene::Update();
}

void CSettingsScene::UpdateSettings()
{
	// Get mouse position for click selection
	POINT mousePos = m_mousePos;

	// Handle mouse clicks for selection
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		float optionStartY = 200.0f;
		float optionHeight = 60.0f;
		float optionStartX = 100.0f;
		float optionWidth = 600.0f;

		// Check if clicking on sensitivity option
		if (mousePos.x >= optionStartX && mousePos.x <= optionStartX + optionWidth &&
			mousePos.y >= optionStartY && mousePos.y <= optionStartY + optionHeight)
		{
			m_selectedOption = OPTION_MOUSE_SENSITIVITY;
			return;
		}

		// Check if clicking on volume option
		float volumeY = optionStartY + optionHeight + 20.0f;
		if (mousePos.x >= optionStartX && mousePos.x <= optionStartX + optionWidth &&
			mousePos.y >= volumeY && mousePos.y <= volumeY + optionHeight)
		{
			m_selectedOption = OPTION_VOLUME;
			return;
		}

		// Check if clicking on apply button
		float applyY = volumeY + optionHeight + 40.0f;
		if (mousePos.x >= optionStartX && mousePos.x <= optionStartX + optionWidth &&
			mousePos.y >= applyY && mousePos.y <= applyY + optionHeight)
		{
			ApplySettings();
			return;
		}

		// Check if clicking on cancel button
		float cancelY = applyY + optionHeight + 20.0f;
		if (mousePos.x >= optionStartX && mousePos.x <= optionStartX + optionWidth &&
			mousePos.y >= cancelY && mousePos.y <= cancelY + optionHeight)
		{
			CancelSettings();
			return;
		}
	}

	// Handle keyboard input
	static bool upKeyPressed = false;
	static bool downKeyPressed = false;
	static bool leftKeyPressed = false;
	static bool rightKeyPressed = false;

	// Navigate up/down between options (always)
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (!upKeyPressed)
		{
			// Always navigate to previous option
			m_selectedOption = (m_selectedOption - 1 + OPTION_COUNT) % OPTION_COUNT;
			CSoundManager::PlaySE(CSoundManager::SE_ItemGet);
			upKeyPressed = true;
		}
	}
	else
	{
		upKeyPressed = false;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (!downKeyPressed)
		{
			// Always navigate to next option
			m_selectedOption = (m_selectedOption + 1) % OPTION_COUNT;
			CSoundManager::PlaySE(CSoundManager::SE_ItemGet);
			downKeyPressed = true;
		}
	}
	else
	{
		downKeyPressed = false;
	}

	// Adjust values with left/right arrow keys
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		if (!leftKeyPressed)
		{
			// If sensitivity selected, decrease value
			if (m_selectedOption == OPTION_MOUSE_SENSITIVITY)
			{
				m_mouseSensitivity = max(0.f, m_mouseSensitivity - 0.005f);
				CSoundManager::PlaySE(CSoundManager::SE_ItemGet);
			}
			// If volume selected, decrease value
			else if (m_selectedOption == OPTION_VOLUME)
			{
				m_volume = max(0, m_volume - 10);
				CSoundManager::PlaySE(CSoundManager::SE_ItemGet);
			}
			leftKeyPressed = true;
		}
	}
	else
	{
		leftKeyPressed = false;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		if (!rightKeyPressed)
		{
			// If sensitivity selected, increase value
			if (m_selectedOption == OPTION_MOUSE_SENSITIVITY)
			{
				m_mouseSensitivity = min(0.1f, m_mouseSensitivity + 0.005f);
				CSoundManager::PlaySE(CSoundManager::SE_ItemGet);
			}
			// If volume selected, increase value
			else if (m_selectedOption == OPTION_VOLUME)
			{
				m_volume = min(1000, m_volume + 10);
				CSoundManager::PlaySE(CSoundManager::SE_ItemGet);
			}
			rightKeyPressed = true;
		}
	}
	else
	{
		rightKeyPressed = false;
	}

	// Handle selection
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (m_selectedOption == OPTION_APPLY)
		{
			ApplySettings();
		}
		else if (m_selectedOption == OPTION_CANCEL)
		{
			CancelSettings();
		}
	}

	// Handle escape key to cancel
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		CancelSettings();
	}
}

void CSettingsScene::ApplySettings()
{
	// Update CSettings first
	CSettings::SetConfigData(m_mouseSensitivity, m_volume);

	// Save settings using CScene method
	SaveSettings(m_mouseSensitivity, static_cast<float>(m_volume));

	// Update sound manager
	CSoundManager::SetGlobalVolume(m_volume);

	// Return to previous scene (menu)
	m_pManager->ChangeScene("MENU");
}

void CSettingsScene::CancelSettings()
{
	// Restore original settings
	m_mouseSensitivity = m_originalMouseSensitivity;
	m_volume = m_originalVolume;

	// Return to previous scene (menu)
	m_pManager->ChangeScene("MENU");
}

void CSettingsScene::Draw()
{
	m_pDx11->SetDepth(false);

	// Draw background
	m_pSettingsBG->Draw();
	m_pFade->Draw();

	TCHAR textBuffer[256];
	D3DXVECTOR4 color;

	// Title
	m_pSettingsFont->SetColor(1.0f, 1.0f, 1.0f);
	m_pSettingsFont->Render(_T("SETTINGS"), WND_W / 2 - 120, 50, 60.0f);

	float startY = 200.0f;
	float optionSpacing = 80.0f;

	// Mouse Sensitivity Option
	color = (m_selectedOption == OPTION_MOUSE_SENSITIVITY) ? m_selectedColor : m_normalColor;
	m_pSettingsFont->SetColor(color.x, color.y, color.z);
	_stprintf_s(textBuffer, _T("MOUSE SENSITIVITY: %.2f"), m_mouseSensitivity / 0.1f);
	m_pSettingsFont->Render(textBuffer, 150, startY, 32.0f);

	// Volume Option
	color = (m_selectedOption == OPTION_VOLUME) ? m_selectedColor : m_normalColor;
	m_pSettingsFont->SetColor(color.x, color.y, color.z);
	_stprintf_s(textBuffer, _T("VOLUME: %.2f%%"), static_cast<float > (m_volume) / 1000.0 * 100.0);
	m_pSettingsFont->Render(textBuffer, 150, startY + optionSpacing, 32.0f);

	// Apply Button
	float applyY = startY + (optionSpacing * 2) + 40.0f;
	color = (m_selectedOption == OPTION_APPLY) ? m_selectedColor : m_normalColor;
	m_pSettingsFont->SetColor(color.x, color.y, color.z);
	m_pSettingsFont->Render(_T("[ APPLY ]"), 150, applyY, 36.0f);

	// Cancel Button
	float cancelY = applyY + 70.0f;
	color = (m_selectedOption == OPTION_CANCEL) ? m_selectedColor : m_normalColor;
	m_pSettingsFont->SetColor(color.x, color.y, color.z);
	m_pSettingsFont->Render(_T("[ CANCEL ]"), 150, cancelY, 36.0f);

	// Instructions
	m_pSettingsFont->SetColor(0.6f, 0.6f, 0.6f);
	m_pSettingsFont->Render(_T("UP/DOWN: Navigate | LEFT/RIGHT: Adjust | ENTER: Select | ESC: Cancel"), 50, WND_H - 100, 18.0f);

	m_pDx11->SetDepth(true);
}

void CSettingsScene::Release()
{
	SAFE_DELETE(m_pSettingsFont);
	SAFE_DELETE(m_pSettingsBG);
	SAFE_DELETE(m_pSettingsBGSprite);
	SAFE_DELETE(m_pFade);
	SAFE_DELETE(m_pFadeSprite);
}
