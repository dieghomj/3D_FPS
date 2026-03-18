#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CUIObject.h"

/********************************************************************************
*	Settings Scene Class.
**/
class CSettingsScene : public CScene
{
private:
	enum SETTINGS_OPTION
	{
		OPTION_MOUSE_SENSITIVITY,
		OPTION_VOLUME,
		OPTION_APPLY,
		OPTION_CANCEL,
		OPTION_COUNT
	};

	enum SLIDER_STATE
	{
		SLIDER_MOUSE_SENS,
		SLIDER_VOLUME,
		SLIDER_COUNT
	};

public:
	CSettingsScene(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager);
	~CSettingsScene();

	void Create() override;
	void Release() override;
	HRESULT LoadData() override;
	void Start() override;
	void Update() override;
	void Draw() override;

	// Allow free mouse movement in menus
	bool ShouldLockMouse() const override { return false; }

private:
	void UpdateSettings();
	void DrawSettings();
	void ApplySettings();
	void CancelSettings();

	// Font for menu text
	CFont* m_pSettingsFont;
	CUIObject* m_pSettingsBG;
	CSprite2D* m_pSettingsBGSprite;
	CUIObject* m_pFade;
	CSprite2D* m_pFadeSprite;

	// Settings values
	float m_mouseSensitivity;
	int m_volume;

	// Original values (for cancel)
	float m_originalMouseSensitivity;
	int m_originalVolume;

	// Selected option
	int m_selectedOption;

	// Colors
	D3DXVECTOR4 m_normalColor;
	D3DXVECTOR4 m_selectedColor;
};
