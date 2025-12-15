#include "CGame.h"


CGame::CGame(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& m_pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, m_pManager)
	, m_pFont(nullptr)
	, m_pStaminaBarSprite(nullptr)
	, m_pStaminaBarUI(nullptr)
	, m_pCrossHairSprite(nullptr)
	, m_pCrossHairUI(nullptr)

	, m_pGround(nullptr)
	, m_pGroundMesh(nullptr)
	, m_pStage(nullptr)
	, m_pBaseStageMesh(nullptr)
	, m_pBridStageMesh(nullptr)
	
	, m_pEnemy(nullptr)
	, m_pEnemyMesh(nullptr)
	
	, m_pPlayer(nullptr)
	, m_pCrossRay()
	, m_pPrevCrossRay()
	, m_prevCrossRay()
	, dashHandle(-1)

	, m_pItemList()
	, m_pItemMesh(nullptr)
	, m_pHealthItem(nullptr)
	, m_pAmmoItemMesh(nullptr)
	, m_pHealthItemMesh(nullptr)
	, m_pDashItemMesh(nullptr)

{
}

CGame::~CGame()
{
}

void CGame::Create()
{
	m_pCamera = new CCamera();
	m_pCameraController = new CCameraController(m_pCamera);

	m_pFont = new CFont();
	
	m_pStaminaBarSprite = new CSprite2D();
	m_pStaminaBarUI = new CUIObject();
	m_pCrossHairSprite = new CSprite2D();
	m_pCrossHairUI = new CUIObject();
	m_pHealthBarSprite = new CSprite2D();
	m_pHealthBarUI = new CUIObject();

	m_pGroundMesh = new CStaticMesh();
	m_pGround = new CStaticMeshObject();
	m_pBaseStageMesh = new CStaticMesh();
	m_pBridStageMesh = new CStaticMesh();
	m_pStage = new CStage();

	m_pEnemyMesh = new CStaticMesh();
	m_pEnemy = new CAnimCharacter();

	m_pPlayer = new CPlayer();

	m_pPistolMesh = new CStaticMesh();
	m_pShotgunMesh = new CStaticMesh();
	m_pPlayerWeapon = new CStaticMeshObject();

	for ( int i = 0; i < 4; ++i )
	{
		m_pCrossRay[i] = new CRay();
	}
	
	for (int i = 0; i < 4; ++i)
	{
		m_pPrevCrossRay[i] = new CRay();
	}


	m_pHealthItemMesh = new CStaticMesh();
	m_pHealthItem = new CHealthItem();


	m_pItemMesh = new CStaticMesh();

	for (int i = 0; i < 10; ++i)
	{
		CItem* pItem = new CHealthItem();
		m_pItemList.push_back(pItem);
	}

	CEffect::GetInstance()->Create(
		m_pDx11->GetDevice(),
		m_pDx11->GetContext());

}

HRESULT CGame::LoadData()
{
	
	if (FAILED(m_pFont->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	CSprite2D::SPRITE_STATE healthBarState = {};
	healthBarState.Disp = { 350.f, 45.6f };
	healthBarState.Base = { 361.f, 79.f };
	healthBarState.Stride = { 361.f, 79.f };

	if(FAILED(m_pHealthBarSprite->Init(*m_pDx11, L"Data\\Texture\\Health.png", healthBarState)))
	{
		return E_FAIL;
	}

	m_pHealthBarUI->AttachSprite(*m_pHealthBarSprite);
	m_pHealthBarUI->SetPosition(D3DXVECTOR3(20.0f, WND_H - 150.0f, 0.0f));

	CSprite2D::SPRITE_STATE staminaBarState = {};
	staminaBarState.Disp = { 300.f, 45.6f };
	staminaBarState.Base = { 128.f, 43.0f };
	staminaBarState.Stride = { 128.f, 43.0f };

	if (FAILED(m_pStaminaBarSprite->Init(*m_pDx11, L"Data\\Texture\\dash.png", staminaBarState)))
	{
		return E_FAIL;
	}

	m_pStaminaBarUI->AttachSprite(*m_pStaminaBarSprite);
	m_pStaminaBarUI->SetPosition(D3DXVECTOR3(20.0f, WND_H - 100.0f, 0.0f));

	CSprite2D::SPRITE_STATE crossHairState = {};
	crossHairState.Disp = { 32.f, 32.f };
	crossHairState.Base = { 32.f, 32.f };
	crossHairState.Stride = { 32.f, 32.f };

	if (FAILED(m_pCrossHairSprite->Init(*m_pDx11, L"Data\\Texture\\Cross.png", crossHairState)))
	{
		return E_FAIL;
	}

	m_pCrossHairUI->AttachSprite(*m_pCrossHairSprite);
	m_pCrossHairUI->SetPosition(D3DXVECTOR3((WND_W / 2) - 16.0f, (WND_H / 2) - 16.0f, 0.0f));
	m_pCrossHairUI->SetAlpha(0.7f);

	if (FAILED(m_pGroundMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Ground\\ground.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pBaseStageMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Stage\\Stage1\\stage1.x")))
	{
		return E_FAIL;
	}


	if (FAILED(m_pBridStageMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Stage\\TestStage\\TestMap.x")))
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

	if (FAILED(m_pPistolMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Weapons\\gun3.x")))
	{
		return E_FAIL;
	}

	m_pPlayerWeapon->AttachMesh(*m_pPistolMesh);
	m_pPlayerWeapon->SetRotation(D3DXVECTOR3(0.f, D3DXToRadian(180.f), 0.f));

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

	if (FAILED(m_pHealthItemMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Health\\HP_Play.x")))
	{
		return E_FAIL;
	}

	m_pHealthItem->AttachMesh(*m_pHealthItemMesh);

	m_pHealthItem->SetPosition(2.0f, 2.0f, 2.0f);

	if (FAILED(CEffect::GetInstance()->LoadData()))
	{
		return E_FAIL;
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

	//m_pPlayerWeapon->SetPosition(0.f, D3DXToRadian(180.f), 0.f);

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
	m_pEnemy->RotateAnim(m_pTime->GetFixedDeltaTime(), D3DXToRadian(1.f));

	float mSense = 0.f;
	if(m_pPlayer->IsDashing())
	{
		mSense = 0.045f; // Lower sensitivity when dashing
	}
	else
	{
		mSense = 0.0f; // Normal sensitivity
	}

	m_pCameraController->FirstPersonCamera(m_pPlayer,m_mouseDelta, m_mouseSense - mSense);
	m_pPlayer->Update();
	HandleWeaponPosition();

	m_prevCrossRay = m_pPlayer->GetCrossRay();
	m_pStage->Update();

	D3DXVECTOR3 playerVel = m_pPlayer->GetVelocity();
	float playerVelLen = D3DXVec3Length(&playerVel);

	if (m_pPlayer->IsDashing())
	{
		D3DXVECTOR3 playerPos = m_pCamera->GetPosition();
		D3DXVECTOR3 forward = m_pPlayer->GetForwardVector();
		//playerPos.y -= 0.02f; // Adjust Y position to be at player's feet
		//playerPos -= forward * 0.3f; // Offset backward a bit
		
		//
		D3DXVec3Normalize(&forward, &forward);

		//
		float angleY = atan2f(forward.x, forward.z);

		if (!CEffect::IsPlaying(dashHandle))
		{
			dashHandle = CEffect::Play(CEffect::DashEffect, playerPos);
			CEffect::SetScale(dashHandle, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
			CEffect::SetRotation(dashHandle, D3DXVECTOR3(0.f, 1.f, 0.f), /*D3DXToRadian(180)*/ + angleY);
			CEffect::SetSpeed(dashHandle, 1.0f);
		}
		else
		{
			// Keep the effect attached and rotated with the current forward
			CEffect::SetLocation(dashHandle, playerPos);
			CEffect::SetRotation(dashHandle, D3DXVECTOR3(0.f, 1.f, 0.f), /*D3DXToRadian(180)*/ + angleY);
		}
	}
	else
	{
		if (CEffect::IsPlaying(dashHandle))
		{
			CEffect::Stop(dashHandle);
			dashHandle = -1;
		}
	}

	m_pHealthItem->Update();


	CScene::Update();
}


void CGame::Draw()
{
	m_pCamera->Draw(m_SceneInfo);
	m_pStage->Draw(m_SceneInfo);

	m_pEnemy->RenderStatic(m_SceneInfo);
	m_pPlayerWeapon->Draw(m_SceneInfo);

	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pCrossRay[dir]->Render(
			m_SceneInfo.mView, m_SceneInfo.mProj, m_pPlayer->GetCrossRay().Ray[dir]);
		D3DXVECTOR3 rayPos = m_pPlayer->GetCrossRay().Ray[dir].Position;
		TCHAR buff[256] = _T("");

		_stprintf_s(buff, _T("RayY Pos: (%.2f, %.2f, %.2f)"), rayPos.x, rayPos.y, rayPos.z);
		m_pFont->Render(buff, WND_W - 500, dir * 50, 18.f);
	}

	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pPrevCrossRay[dir]->Render(
			m_SceneInfo.mView, m_SceneInfo.mProj, m_prevCrossRay.Ray[dir], D3DXVECTOR4(1.0f, 0.f, 0.f, 1.f));
		//D3DXVECTOR3 rayPos = m_pPlayer->GetCrossRay().Ray[dir].Position;
		//TCHAR buff[256] = _T("");

		//_stprintf_s(buff, _T("RayY Pos: (%.2f, %.2f, %.2f)"), rayPos.x, rayPos.y, rayPos.z);
		//m_pFont->Render(buff, WND_W - 500, dir * 50, 32.0f);
	}


	m_pHealthItem->Draw(m_SceneInfo);

	CEffect::GetInstance()->Draw(m_SceneInfo);

	m_pDx11->SetDepth(false);

	m_pHealthBarUI->SetAlpha(0.5f);
	m_pHealthBarSprite->SetFillPercent(1.f, true);
	m_pHealthBarUI->Draw();

	m_pHealthBarUI->SetAlpha(1.f);

	m_pHealthBarSprite->SetFillPercent(m_pPlayer->GetHealth()/100.f, true);
	m_pHealthBarUI->Draw();

	m_pStaminaBarSprite->SetFillPercent(m_pPlayer->GetDashTimer()/3.f, true);
	m_pStaminaBarUI->Draw();

	m_pCrossHairUI->Draw();

	m_pDx11->SetDepth(true);

	m_pFont->Render(_T("3D FPS Sample"), 10, 10, 12.f);
	TCHAR buff[256] = _T("");
	_stprintf_s(buff, _T("FPS: %.2f"), m_pTime->GetFramePerSec());
	m_pFont->Render(buff, 10, 40, 18.f);

	_stprintf_s(buff, _T("Player Pos: (%.2f, %.2f, %.2f)"), m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y, m_pPlayer->GetPosition().z);
	m_pFont->Render(buff, 10, 100, 18.f);

	D3DXVECTOR3 playerVel = m_pPlayer->GetVelocity();
	_stprintf_s(buff, _T("Player Vel: (%.2f, %.2f, %.2f)"), playerVel.x, playerVel.y, playerVel.z);
	m_pFont->Render(buff, 10, 150, 18.f);

	char groundText[20] = "";
	groundText[0] = m_pPlayer->IsGrounded() ? 'T' : 'F';
	_stprintf_s(buff, _T("Player Is Grounded: %s"), groundText);
	m_pFont->Render(buff, 10, 180, 18.f);

	_stprintf_s(buff, _T("Camera Pos: (%.2f, %.2f, %.2f)"), m_pCamera->GetPosition().x, m_pCamera->GetPosition().y, m_pCamera->GetPosition().z);
	m_pFont->Render(buff, 10, 210, 18.f);

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
		case CPlayer::PlayerState::Sliding:
			_stprintf_s(buff, _T("Player State: Sliding"));
			break;
		default:
			_stprintf_s(buff, _T("Player State: Unknown"));
			break;
	}	

	m_pFont->Render(buff, 10, 260, 18.f);

}

void CGame::HandleWeaponPosition()
{
	D3DXVECTOR3 weaponPos = m_pCamera->GetPosition();
	D3DXVECTOR3 weaponRot = D3DXVECTOR3(
		m_pCamera->GetPitch(),
		m_pCamera->GetYaw(),
		0.f
	);

	D3DXVECTOR3 camForward, camRight, camUp;
	camForward = m_pCamera->GetForward();
	camRight = m_pCamera->GetRight();
	camUp = m_pCamera->GetUp();

	D3DXVECTOR3 localOffset =
		camRight * 0.25f +   // move to the right
		camUp * -0.15f +  // move a bit down
		camForward * 0.4f;     // move a bit forward
	weaponPos += localOffset;

	D3DXMATRIX weaponWorld;
	// left-handed, row-major: Right, Up, Forward, Position
	weaponWorld._11 = camRight.x;   weaponWorld._12 = camRight.y;   weaponWorld._13 = camRight.z;   weaponWorld._14 = 0.f;
	weaponWorld._21 = camUp.x;      weaponWorld._22 = camUp.y;      weaponWorld._23 = camUp.z;      weaponWorld._24 = 0.f;
	weaponWorld._31 = camForward.x; weaponWorld._32 = camForward.y; weaponWorld._33 = camForward.z; weaponWorld._34 = 0.f;
	weaponWorld._41 = weaponPos.x;  weaponWorld._42 = weaponPos.y;  weaponWorld._43 = weaponPos.z;  weaponWorld._44 = 1.f;

	D3DXMATRIX gunOffset;
	D3DXMatrixRotationY(&gunOffset, D3DXToRadian(180.f));
	weaponWorld = gunOffset * weaponWorld;

	m_pPlayerWeapon->SetWorldMatrix(weaponWorld);
}
