#include "stdafx.h"
#include "CGameOver.h"

CGameOver::CGameOver(CDirectX9& dx9, CDirectX11& dx11, HWND hWnd, CTime& time, CSceneManager& manager)
	: CScene(dx9, dx11, hWnd, time, manager)
	, m_Font(nullptr) 
	, m_pGameOverSprite(nullptr)
	, m_pGameOverUI(nullptr)
{
}

CGameOver::~CGameOver()
{
	SAFE_DELETE(m_Font);
}

void CGameOver::Release()
{
}

void CGameOver::Create()
{
	m_Font = new CFont();
	m_pGameOverSprite = new CSprite2D();
	m_pGameOverUI = new CUIObject();
}

HRESULT CGameOver::LoadData()
{
	CSprite2D::SPRITE_STATE spriteState = {
		WND_W, WND_H,
		860, 640,
		860, 640
	};

	if (FAILED(m_pGameOverSprite->Init(*m_pDx11, _T("Data\\Texture\\GameOver.png"), spriteState)))
	{
		return E_FAIL;
	}

	m_pGameOverUI->AttachSprite(*m_pGameOverSprite);

	return m_Font->Init(*m_pDx11);
}

void CGameOver::Start()
{
	m_pDx11->SetDepth(false);
}

void CGameOver::Update()
{
	CScene::Update();

	m_pGameOverUI->Update();

	// Press Enter to continue to result scene
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		m_pManager->ChangeScene("RESULT");
	}
	// Press Escape to quit to title if needed (optional)
}

void CGameOver::Draw()
{
	m_pDx11->SetDepth(false);
	
	m_pGameOverUI->Draw();
	
	m_Font->SetColor(1.f, 0.2f, 0.2f);
	m_Font->SetAlpha(1.f);

	m_Font->Render(_T("GAME OVER"), WND_W / 2 - 200.f, WND_H / 2 - 80.f, 80.f);
	m_Font->SetColor(1.f, 1.f, 1.f);
	m_Font->Render(_T("Press ENTER to see your result"), WND_W / 2 - 270.f, WND_H / 2 + 20.f, 32.f);

}

