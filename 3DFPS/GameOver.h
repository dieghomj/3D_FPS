#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CUIObject.h"

class CGameOverScene : public CScene
{
public:
	CGameOverScene(CDirectX9& dx9, CDirectX11& dx11, HWND hWnd, CTime& time, CSceneManager& manager);

	virtual ~CGameOverScene();

	void Release() override;

	void Create() override;

	HRESULT LoadData() override;

	void Start() override;

	void Update() override;

	void Draw() override;

private:
	CFont* m_Font;

	CSprite2D* m_pGameOverSprite;
	CUIObject* m_pGameOverUI;
	
};