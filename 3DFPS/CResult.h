#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CUIObject.h"
#include "CGameStats.h"

class CResultScene : public CScene
{
public:
	CResultScene(CDirectX9& dx9, CDirectX11& dx11, HWND hWnd, CTime& time, CSceneManager& manager);

	virtual ~CResultScene();

	void Release() override;

	void Create() override;

	HRESULT LoadData() override;

	void Start() override;

	void Update() override;

	void Draw() override;

	// Allow free mouse movement in result screen
	bool ShouldLockMouse() const override { return false; }

private:
	enum RESULT_OPTION
	{
		RESULT_OPTION_LEVEL_SELECT,
		RESULT_OPTION_MAIN_MENU,
		RESULT_OPTION_COUNT
	};

	CFont* m_Font;

	// Background
	CUIObject* m_pBG;
	CSprite2D* m_pBGSprite;
	float m_BGScrollOffset;
	float m_BGScrollSpeed;

	// Fade
	CUIObject* m_pFade;
	CSprite2D* m_pFadeSprite;
	bool m_IsFading;
	float m_FadeAlpha;
	float m_FadeSpeed;

	// Navigation
	int m_SelectedOption;
	bool m_GoToLevelSelect;
};