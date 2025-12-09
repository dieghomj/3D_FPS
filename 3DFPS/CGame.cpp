#include "CGame.h"

int currStage = 0;

CGame::CGame(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& m_pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, m_pManager)
	, m_pGround(nullptr)
	, m_pGroundMesh(nullptr)
	, m_pEnemy(nullptr)
	, m_pEnemyMesh(nullptr)
	, m_pPlayer(nullptr)
	, m_pFont(nullptr)
	, m_pStage(nullptr)
	, m_pBaseStageMesh(nullptr)
	, m_pBridStageMesh(nullptr)
	, m_pCrossRay()
{
}

CGame::~CGame()
{
}

void CGame::Create()
{
	m_pCamera = new CCamera();
	m_pCameraController = new CCameraController(m_pCamera);

	m_pEnemyMesh = new CStaticMesh();
	m_pEnemy = new CAnimCharacter();

	m_pGroundMesh = new CStaticMesh();
	m_pGround = new CStaticMeshObject();

	m_pBaseStageMesh = new CStaticMesh();
	m_pBridStageMesh = new CStaticMesh();
	m_pStage = new CStage();

	m_pPlayer = new CPlayer();

	for ( int i = 0; i < 4; ++i )
	{
		m_pCrossRay[i] = new CRay();
	}
	
	for (int i = 0; i < 4; ++i)
	{
		m_pPrevCrossRay[i] = new CRay();
	}

	m_pFont = new CFont();
}

HRESULT CGame::LoadData()
{
	
	if (FAILED(m_pFont->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGroundMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Ground\\ground.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pBaseStageMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Stage\\stage.x")))
	{
		return E_FAIL;
	}


	if (FAILED(m_pBridStageMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Stage\\Bridge\\baseBridge.x")))
	{
		return E_FAIL;
	}
	
	if(FAILED(m_pEnemyMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\Pigman\\Pigman.x")))
	{
		return E_FAIL;
	}

	m_pEnemy->AttachMesh(*m_pEnemyMesh);
	m_pGround->AttachMesh(*m_pGroundMesh);
	m_pStage->AttachMesh(*m_pBaseStageMesh);
	m_pStage->SetPlayer(*m_pPlayer);

	CROSSRAY crossRay = m_pPlayer->GetCrossRay();
	for(int i=0; i<4; ++i)
	{
		auto ray = crossRay.Ray[i];
		if(FAILED(m_pCrossRay[i]->Init(*m_pDx11, ray)))
		{
			return E_FAIL;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		auto ray = crossRay.Ray[i];
		if (FAILED(m_pPrevCrossRay[i]->Init(*m_pDx11, ray)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}


void CGame::Release()
{
}

void CGame::Start()
{
	m_pCamera->SetPosition(0.0f, 2.0f, -5.0f);
	m_pCamera->SetPerspective(D3DX_PI / 4.0f,
		static_cast<float>(WND_W) / static_cast<float>(WND_H),
		0.1f, 1000.0f);
	m_GlobalLight.fIntensity = 1.0f;
	m_GlobalLight.vDirection = D3DXVECTOR3(0.0f, -1.0f, 1.5f);

	m_Fog.Enable = false;

	m_pGround->SetPosition(0.0f, 0.0f, 0.0f);
	m_pEnemy->SetPosition(0.0f, 0.0f, 5.0f);
	m_pPlayer->SetPosition(0.0f, 2.0f, -5.0f);

}

void CGame::Update()
{
	m_pCamera->Update();
	m_pCameraController->Update(0);
	m_pGround->Update();

	if (GetAsyncKeyState('1'))
	{
		if(currStage != 0)
		{
			m_pStage->DetachMesh();
			m_pStage->AttachMesh(*m_pBaseStageMesh);
			currStage = 0;
		}
	}
	if (GetAsyncKeyState('2'))
	{
		if (currStage != 1)
		{
			m_pStage->DetachMesh();
			m_pStage->AttachMesh(*m_pBridStageMesh);
			currStage = 1;
		}
	}

	if (GetAsyncKeyState('R'))
	{
		m_pPlayer->SetPosition(0.0f, 2.0f, -5.0f);
		m_pPlayer->SetFloorY(0.0f);
	}

	m_pEnemy->Update();

	m_pCameraController->FirstPersonCamera(m_pPlayer,m_mouseDelta, m_mouseSense);
	m_prevCrossRay = m_pPlayer->GetCrossRay();
	m_pPlayer->Update();
	m_pStage->Update();
	
	CScene::Update();
}

void CGame::Draw()
{
	m_pCamera->Draw(m_SceneInfo);
	m_pStage->Draw(m_SceneInfo);

	m_pEnemy->RenderStatic(m_SceneInfo);

	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pCrossRay[dir]->Render(
			m_SceneInfo.mView, m_SceneInfo.mProj, m_pPlayer->GetCrossRay().Ray[dir]);
		D3DXVECTOR3 rayPos = m_pPlayer->GetCrossRay().Ray[dir].Position;
		TCHAR buff[256] = _T("");

		_stprintf_s(buff, _T("RayY Pos: (%.2f, %.2f, %.2f)"), rayPos.x, rayPos.y, rayPos.z);
		m_pFont->Render(buff, WND_W - 500, dir * 50, 32.0f);
	}

	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pPrevCrossRay[dir]->Render(
			m_SceneInfo.mView, m_SceneInfo.mProj, m_prevCrossRay.Ray[dir], D3DXVECTOR4(1.0f, 0.f, 0.f, 1.f));
		//D3DXVECTOR3 rayPos = m_pPlayer->GetCrossRay().Ray[dir].Position;
		//TCHAR buff[256] = _T("");

		//_stprintf_s(buff, _T("RayY Pos: (%.2f, %.2f, %.2f)"), rayPos.x, rayPos.y, rayPos.z);
		//m_pFont->Render(buff, WND_W - 500, dir * 50, 32.0f);
	}

	m_pFont->Render(_T("3D FPS Sample"), 10, 10, 24.0f);
	TCHAR buff[256] = _T("");
	_stprintf_s(buff, _T("FPS: %.2f"), m_pTime->GetFramePerSec());
	m_pFont->Render(buff, 10, 40, 32.0f);

	_stprintf_s(buff, _T("Player Pos: (%.2f, %.2f, %.2f)"), m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y, m_pPlayer->GetPosition().z);
	m_pFont->Render(buff, 10, 100, 32.0f);

	

	_stprintf_s(buff, _T("Camera Pos: (%.2f, %.2f, %.2f)"), m_pCamera->GetPosition().x, m_pCamera->GetPosition().y, m_pCamera->GetPosition().z);
	m_pFont->Render(buff, 10, 150, 32.0f);

	switch (m_pPlayer->GetState())
	{
		case CPlayer::PlayerState::Idle:
		_stprintf_s(buff, _T("Player State: Idle"));
		break;
		case CPlayer::PlayerState::Walking:
		_stprintf_s(buff, _T("Player State: Walking"));
		break;
		case CPlayer::PlayerState::Dashing:
			_stprintf_s(buff, _T("Player State: Dashing"));
			break;
		case CPlayer::PlayerState::Jumping:
			_stprintf_s(buff, _T("Player State: Jumping"));
			break;
		case CPlayer::PlayerState::Attacking:
			_stprintf_s(buff, _T("Player State: Attacking"));
			break;
		default:
			_stprintf_s(buff, _T("Player State: Unknown"));
			break;
	}	

	m_pFont->Render(buff, 10, 180, 32.0f);	

}
