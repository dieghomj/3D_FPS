#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CUIObject.h"

class CGameOver : public CScene
{
public:
	CGameOver(CDirectX9& dx9, CDirectX11& dx11, HWND hWnd, CTime& time, CSceneManager& manager);

	virtual ~CGameOver();

	void Release() override;

	void Create() override;

	HRESULT LoadData() override;

	void Start() override;

	void Update() override;

	void Draw() override;

	// ゲームオーバー画面ではマウスの自由な移動を許可.
	bool ShouldLockMouse() const override { return false; }

private:
	enum GAMEOVER_OPTION
	{
		GAMEOVER_OPTION_RETRY,
		GAMEOVER_OPTION_MAIN_MENU,
		GAMEOVER_OPTION_COUNT
	};

	CFont* m_Font;

	// 背景.
	CUIObject* m_pBG;
	CSprite2D* m_pBGSprite;
	float m_BGScrollOffset;
	float m_BGScrollSpeed;

	// カーソル.
	CUIObject* m_pCursor;
	CSprite2D* m_pCursorSprite;

	// フェード.
	CUIObject* m_pFade;
	CSprite2D* m_pFadeSprite;
	bool m_IsFading;
	float m_FadeAlpha;
	float m_FadeSpeed;

	// ナビゲーション.
	int m_SelectedOption;
	bool m_GoToRetry;
};