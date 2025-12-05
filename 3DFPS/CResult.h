#pragma once
#include "CScene.h"
#include "CFont.h"
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

private:
	CFont* m_Font;
};