#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CUIObject.h"
#include "CSettingsMenu.h"

/********************************************************************************
*	メニューシーンクラス.
**/
class CMenu : public CScene
{
public:
	static bool s_OpenToLevelSelect;

private:
	enum MENU_OPTION
	{
		MENU_OPTION_START,
		MENU_OPTION_SETTINGS,
		MENU_OPTION_EXIT,
		MENU_OPTION_COUNT
	};

	enum LEVEL_OPTION
	{
		LEVEL_1 = 0,
		LEVEL_2,
		LEVEL_3,
		LEVEL_COUNT,

		LEVEL_BACK
	};

	enum MENU_STATE
	{
		STATE_MAIN_MENU,
		STATE_LEVEL_SELECT,
		STATE_SETTINGS,
	};

public:
	CMenu(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager);
	~CMenu();

	void Create() override;
	void Release() override;
	HRESULT LoadData() override;
	void Start() override;
	void Update() override;
	void Draw() override;

	// メニューではマウスを自由に動かせるようにする.
	bool ShouldLockMouse() const override { return false; }

private:
	void UpdateMainMenu();
	void UpdateLevelSelect();
	void UpdateSettings();
	void DrawMainMenu();
	void DrawLevelSelect();
	void DrawSettings();

	// メニューテキスト用フォント.
	CFont* m_pMenuFont;
	CUIObject* m_pMenuBG;
	CUIObject* m_pMenuOption;
	CSprite2D* m_pMenuBGSprite;
	CUIObject* m_pFade;
	CSprite2D* m_pFadeSprite;
	CSprite2D* m_pCursorSprite;
	CUIObject* m_pCursor;

	// メニューの状態.
	MENU_STATE m_MenuState;
	int m_SelectedOption;
	int m_SelectedLevel;
	int m_SelectedDifficulty;

	bool m_IsFading;
	float m_FadeAlpha;
	float m_FadeSpeed;

	// 背景スクロール.
	float m_BGScrollOffset;
	float m_BGScrollSpeed;

	// 共通設定メニューUI.
	CSettingsMenu m_settingsMenu;
};