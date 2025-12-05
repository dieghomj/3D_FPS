#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CUIObject.h"

/********************************************************************************
*	Menu Scene Class.
**/
class CMenu : public CScene
{
public:
	CMenu(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager);
	~CMenu();

	void Create() override;
	void Release() override;
	HRESULT LoadData() override;
	void Start() override;
	void Update() override;
	void Draw() override;

private:
	// Font for menu text
	CFont* m_pMenuFont;
	CUIObject* m_pMenuBG;
	CUIObject* m_pMenuOption;
	CSprite2D* m_pMenuBGSprite;
	CUIObject* m_pFade;
	CSprite2D* m_pFadeSprite;
	
	// Menu state
	int m_SelectedOption;
	const int MENU_OPTION_START = 0;
	const int MENU_OPTION_EXIT = 1;
	bool m_IsFading;
	float m_FadeAlpha;
	float m_FadeSpeed;
};
