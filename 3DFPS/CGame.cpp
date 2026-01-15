#include "CGame.h"
#include "CSpider.h"

constexpr int PROJECTILE_COUNT_MAX = 32;
constexpr int PLAYER_AMMO_MAX = 999;
constexpr int ENEMY_COUNT_MAX = 64;
constexpr int ENEMY_COUNT_PER_ROOM = 4;
constexpr float ENEMY_SHOT_SPEED = 1.2f;
constexpr int STAGE_TIMER = 2 * 60; // minutes
const D3DXVECTOR3  PLAYER_STARTPOS = D3DXVECTOR3(0.f, 25.f, -25.f);

const D3DXVECTOR3 enemyStartPos[4] = {
	D3DXVECTOR3(-13.f,8.f,200.f),
	D3DXVECTOR3(13.f,8.f,200.f),
	D3DXVECTOR3(-13.f,8.f,235.f),
	D3DXVECTOR3(13.f,8.f,235.f),
};

const D3DXVECTOR3 itemPos[2] =
{
	D3DXVECTOR3(0.f, 1.45f, 75.9f),
	D3DXVECTOR3(0.f, 1.45f, 150.9f),
};

bool wasStuck = false;
int stuckFrameCount = 0;

CGame::CGame(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& m_pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, m_pManager)
	, m_RandomGen(std::random_device{}())
	, m_AngleDist(0.0f, 360.0f)
	, distFloat(0.0f, 1.0f)
	, m_pFont(nullptr)
	, m_pStaminaBarSprite(nullptr)
	, m_pStaminaBarUI(nullptr)
	, m_pCrossHairSprite(nullptr)
	, m_pCrossHairUI(nullptr)
	, m_pHealthBarSprite(nullptr)
	, m_pHealthBarUI(nullptr)
	, m_pPainSprite(nullptr)
	, m_pPainUI(nullptr)

	, m_pSkybox(nullptr)
	, m_pGround(nullptr)
	, m_pGroundMesh(nullptr)
	, m_pStage(nullptr)
	, m_pTestStageMesh(nullptr)
	, m_pStageMesh(nullptr)
	
	, m_pEnemy(nullptr)
	, m_pBossEnemy(nullptr)
	, m_pEnemyList()
	, m_pSpiderSkinMesh(nullptr)
	, m_pRoboSkinMesh(nullptr)
	, m_pBossSkinMesh(nullptr)
	, m_pEnemyMesh(nullptr)
	
	, m_pPlayer(nullptr)
	
	, m_pPistolMesh(nullptr)
	, m_pPlayerWeapon(nullptr)
	, m_pBulletMesh(nullptr)
	, m_pBulletList()
	, m_pShotDecalSprite(nullptr)

	, dashHandle(-1)

	, m_pItemList()
	, m_pItemMesh(nullptr)
	, m_pHealthItem(nullptr)
	, m_pAmmoItemMesh(nullptr)
	, m_pHealthItemMesh(nullptr)
	, m_pDashItemMesh(nullptr)

	, m_pSphereMesh(nullptr)

	, currStage(0)
#if _DEBUG
	, m_pPlayerRayY(nullptr)
	, m_pCrossRay()
	, m_pPrevCrossRay()
	, m_prevCrossRay()
	, debugRay(nullptr)
	, debugShotRay(nullptr)
	, debugShotMark()
	, debugHitShotList()
	, debugShotMesh(nullptr)
#endif
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

	m_pSkybox = new CSkybox();

	m_pGroundMesh = new CStaticMesh();
	m_pGround = new CStaticMeshObject();
	m_pTestStageMesh = new CStaticMesh();
	m_pStageMesh = new CStaticMesh();
	m_pStage = new CStage();

	m_pCubeMesh = new CStaticMesh();
	m_pLightningSprite = new CSprite3D();
	m_pLightning = new CBlockedPath();
	m_pBlockedPathList.reserve(32);

	for (int i = 0; i < 32; ++i)
	{
		CBlockedPath* pBlockedPath = new CBlockedPath();
		m_pBlockedPathList.push_back(pBlockedPath);
	}

	m_pEnemyMesh = new CStaticMesh();
	m_pSpiderMesh = new CStaticMesh();
	m_pBossMesh = new CStaticMesh();
	m_pSpiderSkinMesh = new CSkinMesh();
	m_pRoboSkinMesh = new CSkinMesh();
	m_pBossSkinMesh = new CSkinMesh();
	m_pBossEnemy = new CBoss();
	m_pEnemy = new CBoss();

	m_pEnemyList.reserve(ENEMY_COUNT_MAX);
	m_pBossShotList.reserve(ENEMY_COUNT_MAX);
	
	for( int i= 0; i < ENEMY_COUNT_MAX; i++)
	{
		float rnd = distFloat(m_RandomGen); 
		if (rnd > 0.5f)
		{
			CAnimEnemy* pEnemy = new CSpider();
			pEnemy->SetActive(false);
			m_pBossShotList.push_back(pEnemy);
			continue;
		}
		else
		{
			CAnimEnemy* pEnemy = new CRobo();
			pEnemy->SetActive(false);
			m_pBossShotList.push_back(pEnemy);
			continue;
		}
	}

	for (int i = 0; i < ENEMY_COUNT_MAX; i++)
	{
		CAnimEnemy* pEnemy = new CSpider();
		pEnemy->SetActive(false);
		m_pEnemyList.push_back(pEnemy);
	}

	for (int i = 0; i < PROJECTILE_COUNT_MAX; i++)
	{
		CShot* pProjectile = new CShot();
		m_pEnemyShotList.push_back(pProjectile);
		enemyShotEffectHandles.push_back(-1);
	}

	m_pPlayer = new CPlayer();

	m_pBulletLaserSprite = new CSprite3D();
	m_pBulletLaser = new CSpriteObject();
	m_pPistolMesh = new CStaticMesh();
	m_pShotgunMesh = new CStaticMesh();
	m_pPlayerWeapon = new CStaticMeshObject();
	m_pBulletMesh = new CStaticMesh();
	for (int i = 0; i < PLAYER_AMMO_MAX; ++i)
	{
		CShot* pBullet = new CShot();
		m_pBulletList.push_back(pBullet);
	}

	m_pShotDecalSprite = new CDecal();
	m_pEnemyHitDecalSprite = new CDecal();

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

#ifdef _DEBUG

	for (int i = 0; i < 4; ++i)
	{
		m_pCrossRay[i] = new CRay();
	}

	for (int i = 0; i < 4; ++i)
	{
		m_pPrevCrossRay[i] = new CRay();
	}
	m_pPlayerRayY = new CRay();
	m_pSphereMesh = new CStaticMesh();
	debugRay = new CRay();
	debugShotRay = new CRay();
#endif // DEBUG
}

HRESULT CGame::LoadData()
{
	HRESULT hr = S_OK;
	
	if (FAILED(LoadSceneAssets()))
	{
		return E_FAIL;
	}

	if(FAILED(LoadUIAssets()))
	{
		return E_FAIL;
	}

	if(FAILED(LoadSpriteAssets()))
	{
		return E_FAIL;
	}
	
	if(FAILED(LoadUtilityMesh()))
	{
		return E_FAIL;
	}
	
	if(FAILED(LoadStageMesh()))
	{
		return E_FAIL;
	}
	
	if(FAILED(LoadEnemiesMesh()))
	{
		return E_FAIL;
	}
	
	if(FAILED(LoadPlayerAsset()))
	{
		return E_FAIL;
	}
	


//デバッグ用
#if _DEBUG

	if (FAILED(m_pItemMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Ammo\\AMMO.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pHealthItemMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Wall\\WallCol.x")))
	{
		return E_FAIL;
	}

	m_pHealthItem->AttachMesh(*m_pHealthItemMesh);

	m_pHealthItem->SetPosition(2.0f, 2.0f, 2.0f);

	CROSSRAY crossRay = m_pPlayer->GetCrossRay();
	for (int i = 0; i < 4; ++i)
	{
		auto ray = crossRay.Ray[i];
		if (FAILED(m_pCrossRay[i]->Init(*m_pDx11, ray)))
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

	RAY rayY = m_pPlayer->GetRayY();
	m_pPlayerRayY->Init(*m_pDx11, rayY);

	debugRay->Init(*m_pDx11, m_pStage->debugSweptRay);

	RAY shotRay;
	shotRay.Position = m_pCamera->GetPosition();
	shotRay.Axis = D3DXVECTOR3(0.f, 0.f, 1.f);
	D3DXVec3Normalize(&shotRay.Axis, &shotRay.Axis);
	shotRay.Length = 100.f;
	shotRay.RotationY = 0.f;
	debugShotRay->Init(*m_pDx11, shotRay);

#endif

	return S_OK;
}

void CGame::Release()
{
}

void CGame::Start()
{
	//ライト設定
	float lightIntensity	= 1.05f;
	D3DXVECTOR3 lightDir	= D3DXVECTOR3(0.2f, -1.0f, 0.5f);
	
	//フォグ設定
	bool fog				= false;

	//カメラ設定.
	float fovY				= D3DX_PI / 4.0f;
	float aspect			= static_cast<float>(WND_W) / static_cast<float>(WND_H);
	float zn				= 0.1f, 
		  zf				= 1000.0f;

	//シーン初期化.
	InitScene(fovY, aspect, zn, zf, lightIntensity, lightDir, fog);

	InitUI();
	InitSpriteAssets();
	InitStage();
	InitEnemy();
	InitPlayer();

	m_pPlayer->InitPlayer();
	m_pPlayer->SetPosition( PLAYER_STARTPOS );
	m_pStage->RestartPlayerPosition(PLAYER_STARTPOS);

	m_accumulatedTime = 0.0f;
	m_enemyKillCount = 0;
	m_comboCount = 0;
	m_stageTimer = STAGE_TIMER;

	CGameStats::Reset();
}

void CGame::InitScene(float fovY, float aspect, float zn, float zf, float lightIntensity, const D3DXVECTOR3& lightDir, bool fog)
{
	m_pCamera->SetPerspective(fovY,
		aspect,
		zn, zf);
	m_GlobalLight.fIntensity = lightIntensity;
	m_GlobalLight.vDirection = lightDir;
	m_Fog.Enable = fog;
}

void CGame::Update()
{
	m_accumulatedTime += 0.016f;
	if (1.0f - m_accumulatedTime <= 1e-5)
	{
		m_stageTimer -= 1.0f;
		m_accumulatedTime = 0.0f;
	}
	
	if (CheckRestartStatus())
	{
		Restart();
		return;
	}

	HandleGameOver();

	HandleBlockedPath();
	//m_pGround->Update();

	m_pEnemy->Update();
	m_pEnemy->SetPlayerPos(m_pPlayer->GetPosition());

	for (auto pEnemy : m_pEnemyList)
	{
		pEnemy->Update();
		pEnemy->SetPlayerPos(m_pPlayer->GetPosition());
	}

	for (auto pBossShot : m_pBossShotList)
	{
		if (pBossShot->IsActive())
		{
			pBossShot->Update();
			pBossShot->SetPlayerPos(m_pPlayer->GetPosition());
		}
	}

	//m_pEnemyList[1]->Update();
	//m_pEnemy->RotateAnim(m_pTime->GetFixedDeltaTime(), D3DXToRadian(30.f));
	//m_pEnemy->UpDownAnim(m_pTime->GetTotalTime(), 0.02f, 0.005f);

	float mSense = 0.f;
	if (m_pPlayer->IsDashing())
	{
		mSense = 0.045f; // Lower sensitivity when dashing
	}
	else
	{
		mSense = 0.0f; // Normal sensitivity
	}

	if (m_pPlayer->IsJumping())
	{
		mSense -= 0.05f;
	}

	m_pPlayer->Update();
	m_pPlayer->UpdateCollider();
	if (wasStuck)
	{
		// Skip updating the stage to prevent collision issues
		stuckFrameCount++;
		if (stuckFrameCount > 1)
		{
			wasStuck = false;
			stuckFrameCount = 0;
		}
	}
	else
	{

		CheckTriggers();
		CheckGoal();
		for (auto pBlockedPath : m_pBlockedPathList)
		{
			if (pBlockedPath->IsActive() == false)
			{
				continue;
			}
			pBlockedPath->Update();
			pBlockedPath->UpdateCollider();
			pBlockedPath->HandleWallCollisions(m_pPlayer);
		}
		m_pStage->Update();

	}

	m_pCameraController->FirstPersonCamera(m_pPlayer, m_mouseDelta, m_mouseSense - mSense);
	m_pCamera->Update();
	m_pCameraController->Update(0);

	HandleWeapon();

	HandlePlayerEnemyCollision();
	HandleEnemyEnemyCollision();
	HandleEnemyShooting();

	for(auto pBullet : m_pBulletList)
	{
		pBullet->Update();
	}

	for (auto pEnemyShot : m_pEnemyShotList)
	{
		pEnemyShot->Update();
		pEnemyShot->UpdateCollider();
		if (pEnemyShot->IsHit(m_pPlayer->GetCollider(), m_pPlayer->GetRadius()))
		{
			m_pPlayer->ApplyDamage(20.f);
			pEnemyShot->SetDisplay(false);
		}
	}

	D3DXVECTOR3 playerVel = m_pPlayer->GetVelocity();
	float playerVelLen = D3DXVec3Length(&playerVel);

	HandlePlayerDashEffect();

	m_pHealthItem->Update();
	m_pHealthItem->UpDownAnim(m_pTime->GetTotalTime(), 0.005f, 0.009);



#if _DEBUG

	if (GetAsyncKeyState('1'))
	{
		if (currStage != 1)
		{
			m_pStage->DetachMesh();
			m_pStage->AttachMesh(*m_pStageMesh);
			m_pStage->SetScale(1.0f);
			currStage = 1;
		}
	}

	if (GetAsyncKeyState('2'))
	{
		if (currStage != 0)
		{
			m_pStage->DetachMesh();
			m_pStage->AttachMesh(*m_pTestStageMesh);
			currStage = 0;
			m_pStage->SetScale(1.f);
		}
	}

	if (GetAsyncKeyState('R'))
	{
		Restart();
	}

#endif
	CScene::Update();
}

void CGame::HandleBlockedPath()
{
	if (m_enemyKillCount >= ENEMY_COUNT_PER_ROOM)
	{
		for (auto pBlockedPath : m_pBlockedPathList)
		{
			if (pBlockedPath->IsActive() == true)
			{
				pBlockedPath->SetActive(false);
			}
		}
		return;
	}

	if (m_pBlockedPathList[0]->IsActive() == true)
	{
		for (int i = 0; i < ENEMY_COUNT_PER_ROOM; i++)
		{
			if (m_pEnemyList[i]->IsActive() == false)
				m_pEnemyList[i]->SpawnAt(enemyStartPos[i]);
		}
	}
}

void CGame::HandleGameOver()
{
	if (m_stageTimer <= 0.f)
	{
		// Time's up - handle game over
		SaveStats();
		m_pManager->ChangeScene("GAMEOVER");
	}
}

bool CGame::CheckRestartStatus()
{
	return m_pPlayer->IsAlive() == false || m_pPlayer->GetPosition().y <= -50.f;
}

void CGame::HandlePlayerDashEffect()
{
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
			CEffect::SetRotation(dashHandle, D3DXVECTOR3(0.f, 1.f, 0.f), /*D3DXToRadian(180)*/ +angleY);
			CEffect::SetSpeed(dashHandle, 1.0f);
		}
		else
		{
			// Keep the effect attached and rotated with the current forward
			CEffect::SetLocation(dashHandle, playerPos);
			CEffect::SetRotation(dashHandle, D3DXVECTOR3(0.f, 1.f, 0.f), /*D3DXToRadian(180)*/ +angleY);
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
}

void CGame::Restart()
{

	for (int i = 0; i < 4; i++)
	{
		m_pEnemyList[i]->InitEnemy();
		m_pEnemyList[i]->SetPosition(enemyStartPos[i]);
		m_pEnemyList[i]->SetActive(false);
	}

	m_pPlayer->InitPlayer();
	m_pPlayer->SetPosition(PLAYER_STARTPOS);
	m_pStage->RestartPlayerPosition(PLAYER_STARTPOS);

	//SetupBlockedPath();
	//SetupGoal();
	//SetupTriggers();

	m_enemyKillCount = 0;
	m_comboCount = 0;
	CGameStats::DeathCounter++;
	m_highestCombo = max(m_highestCombo, m_comboCount);
	m_comboCount = 0;

	for (auto blockedPath : m_pBlockedPathList)
	{
		blockedPath->SetActive(false);
	}
	
	for (auto& trigger : m_CollisionTriggerList)
	{
		trigger.isTriggered = false;
	}

#if _DEBUG
	debugHitShotList.clear();
#endif
}


void CGame::Draw()
{
	m_pCamera->Draw(m_SceneInfo);

	// Render skybox first (before other objects)
	if (m_pSkybox)
	{
		m_pSkybox->Render(m_SceneInfo.mView, m_SceneInfo.mProj, m_SceneInfo.Camera.vPosition);
	}

	m_pStage->Draw(m_SceneInfo);

	m_pPlayerWeapon->Draw(m_SceneInfo);

	m_pEnemy->RenderStatic(m_SceneInfo);
	//HandleEnemyShotLoadAnim();

	for ( auto pBossShot : m_pBossShotList)
	{
		if(typeid(*pBossShot) == typeid(CSpider))
		{
			CSpider* pSpider = dynamic_cast<CSpider*>(pBossShot);
			pSpider->Draw(m_SceneInfo);
		}
		else if (typeid(*pBossShot) == typeid(CRobo))
		{
			CRobo* pRobo = dynamic_cast<CRobo*>(pBossShot);
			pRobo->RenderStatic(m_SceneInfo);
		}
	}

	for (auto pEnemyShot : m_pEnemyShotList)
	{
		pEnemyShot->Draw(m_SceneInfo);
	}

	for(auto pEnemy : m_pEnemyList)
	{
		pEnemy->Draw(m_SceneInfo);
		
	}
	

	DrawEnemyShots();

	m_pHealthItem->Draw(m_SceneInfo);

	CEffect::GetInstance()->Draw(m_SceneInfo);

	D3DXMATRIX& mView = m_SceneInfo.mView;
	D3DXMATRIX& mProj = m_SceneInfo.mProj;
	LIGHT globalLight = m_SceneInfo.Light;
	D3DXVECTOR3 camPos = m_SceneInfo.Camera.vPosition;
	FOG fog = m_SceneInfo.Fog;
	SPOT_LIGHT* pSpotLightArray = m_SceneInfo.pSpotLightArray;
	int lightCount = m_SceneInfo.SpotLightNum;


	DrawDecals(mView, mProj);


	m_pLightningSprite->SetPatternNo(0,(int)(m_pTime->GetTotalTime() * 0.01f) % 11);
	m_pBlockedPathList[0]->UpdateCollider();
	m_pBlockedPathList[0]->Draw(m_SceneInfo);

	m_pBlockedPathList[1]->UpdateCollider();
	m_pBlockedPathList[1]->Draw(m_SceneInfo);
	
	m_pDx11->SetDepth(false);
	m_pHealthBarUI->SetAlpha(0.5f);
	m_pHealthBarSprite->SetFillPercent(1.f, true);
	m_pHealthBarUI->Draw();

	m_pHealthBarUI->SetAlpha(1.f);

	m_pHealthBarSprite->SetFillPercent(m_pPlayer->GetHealth() / 100.f, true);
	m_pHealthBarUI->Draw();

	m_pStaminaBarSprite->SetFillPercent(m_pPlayer->GetDashTimer() / 3.f, true);
	m_pStaminaBarUI->Draw();

	m_pCrossHairUI->Draw();

	m_pDx11->SetDepth(true);



#if _DEBUG

	//DEBUG

	auto playerPath = m_pStage->debugPlayerPath;

	for(auto point : playerPath)
	{
		m_pSphereMesh->SetPosition(point + D3DXVECTOR3(0.f,-m_pPlayer->GetHeight(), 0.f));
		m_pSphereMesh->SetScale(D3DXVECTOR3(0.05f, 0.05f, 0.05f));
		D3DXVECTOR3 dir = point - m_pPlayer->GetPosition();
		float dist = D3DXVec3Length(&dir);
		if(dist <= 3.f)
		{
			m_pSphereMesh->SetScale(dist/ 3.f * 0.05f);
		}
		m_pSphereMesh->Render(mView, mProj, globalLight, camPos, fog, pSpotLightArray, lightCount);
	}

	for (auto hitPos : debugHitShotList)
	{
		
		m_pSphereMesh->SetPosition(hitPos);
		m_pSphereMesh->SetScale(D3DXVECTOR3(0.02f, 0.02f, 0.02f));
		//m_pSphereMesh->Render(mView, mProj, globalLight, camPos, fog, pSpotLightArray, lightCount);

	}

	D3DXVECTOR4 color = D3DXVECTOR4(1.f, 0.f, 0.f, 1.f);

	if(m_pStage->debugSweptHit)
	{
		color = D3DXVECTOR4(0.f, 0.f, 1.f, 1.f);
	}

	debugRay->Render(
		m_SceneInfo.mView, m_SceneInfo.mProj, m_pStage->debugSweptRay, color);

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
	}

#endif

	TCHAR buff[256] = _T("");
	float seconds = static_cast<int>(m_stageTimer) % 60;
	float minutes = static_cast<int>(m_stageTimer) / 60;
	_stprintf_s(buff, _T("TIME: %02.f:%02.f"), minutes, seconds );
	m_pFont->Render(buff, WND_W - 32*9, 40, 32.f);


#if _DEBUG

	m_pFont->Render(_T("3D FPS Sample"), 10, 10, 12.f);
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

	groundText[0] = m_pPlayer->IsShot() ? 'T' : 'F';
	_stprintf_s(buff, _T("Player Is Shot: %s"), groundText);
	m_pFont->Render(buff, 10, 200, 18.f);

	_stprintf_s(buff, _T("Camera Pos: (%.2f, %.2f, %.2f)"), m_pCamera->GetPosition().x, m_pCamera->GetPosition().y, m_pCamera->GetPosition().z);
	m_pFont->Render(buff, 10, 230, 18.f);

	if (!debugHitShotList.empty())
	{
		_stprintf_s(buff, _T("Shot Ray Axis: (%.2f, %.2f, %.2f) "),
			debugHitShotList[debugHitShotList.size() - 1].x,
			debugHitShotList[debugHitShotList.size() - 1].y,
			debugHitShotList[debugHitShotList.size() - 1].z);
		m_pFont->Render(buff, 10, 260, 18.f);
	}

	switch (m_pPlayer->GetCurrentWeapon())
	{

	case 0:
		_stprintf_s(buff, _T("Current Weapon: Pistol"));
		break;
	case 1:
		_stprintf_s(buff, _T("Current Weapon: Shotgun"));
		break;
	default:
		_stprintf_s(buff, _T("Current Weapon: Empty"));
		break;
	}

	m_pFont->Render(buff, 10, 300, 18.f);

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

	m_pFont->Render(buff, 10, 360, 18.f);

#endif

}

void CGame::DrawDecals(D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	int count = 2;
	for (auto& mark : m_bulletImpactList)
	{
		if (mark.lifeTime <= 0)
		{
			m_bulletImpactList.erase(m_bulletImpactList.begin());
			continue;
		}

		mark.lifeTime -= FPS;

		if (mark.isEnemyHit)
		{
			D3DXVECTOR3 decalPos = mark.position + mark.normal * count * 0.0001f;
			m_pEnemyHitDecalSprite->SetPosition(decalPos);
			m_pEnemyHitDecalSprite->SetRotationFromNormal(mark.normal);
			m_pEnemyHitDecalSprite->SetScale(D3DXVECTOR3(0.003f, 0.003f, 0.003f));
			m_pEnemyHitDecalSprite->RenderDecal(mView, mProj, mark.normal);

			count++;
			continue;

		}

		D3DXVECTOR3 decalPos = mark.position + mark.normal * count * 0.0001f;
		m_pShotDecalSprite->SetPosition(decalPos);
		m_pShotDecalSprite->SetRotationFromNormal(mark.normal);
		m_pShotDecalSprite->SetScale(D3DXVECTOR3(0.015f, 0.015f, 0.015f));
		m_pShotDecalSprite->RenderDecal(mView, mProj, mark.normal);

		count++;
	}
}

void CGame::DrawEnemyShots()
{
	for (int i = 0; i < PROJECTILE_COUNT_MAX; i++)
	{
		m_pEnemyShotList[i]->UpdateCollider();

		if (!m_pEnemyShotList[i]->IsDisplay())
		{
			CEffect::Stop(enemyShotEffectHandles[i]);
			continue;
		}

		if (CEffect::IsPlaying(enemyShotEffectHandles[i]))
		{
			CEffect::SetLocation(enemyShotEffectHandles[i], m_pEnemyShotList[i]->GetPosition());
		}
		else
		{
			enemyShotEffectHandles[i] = CEffect::Play(CEffect::MagmaEffect, m_pEnemyShotList[i]->GetPosition());
		}
	}
}

void CGame::HandleEnemyShotLoadAnim(CRobo* pEnemy)
{


	//RANGED ENEMY SHOT LOAD ANIMATION
	/*if (!pEnemy->IsShot())
	{
		float scale = pEnemy->GetAttackCD() * 0.5f;
		D3DXVECTOR3 vScale = D3DXVECTOR3(scale, scale, scale);
		D3DXVECTOR3 vPos = pEnemy->GetPosition() + D3DXVECTOR3(0.f, 2.5f, 0.f) - pEnemy->GetForward() * 3.5f;

		if (CEffect::IsPlaying(enemyShotLoadHandle))
		{
			CEffect::SetScale(enemyShotLoadHandle, vScale);
			CEffect::SetLocation(enemyShotLoadHandle, vPos);
		}
		else
		{
			enemyShotLoadHandle = CEffect::Play(CEffect::MagmaEffect, vPos);
			CEffect::SetScale(enemyShotLoadHandle, vScale);
		}

	}
	else
	{
		CEffect::Stop(enemyShotLoadHandle);
	}*/
}

void CGame::HandleWeapon()
{
	HandleWeaponPos();

	D3DXVECTOR3 camForward = m_pCamera->GetForward();
	D3DXVECTOR3 bulletDir = camForward;
	D3DXVec3Normalize(&bulletDir, &bulletDir);
	D3DXVECTOR3 shotEffPos = m_pPlayerWeapon->GetPosition() + bulletDir * 0.25f + D3DXVECTOR3(0.f, 0.1f, 0.f);
	CEffect::SetLocation(m_shotHandle, shotEffPos);
	BULLET_IMPACT impact;

	impact.lifeTime = FPS * 1000; // 5 seconds

	if (m_pPlayer->IsShot())
	{
		int currWeapon = m_pPlayer->GetCurrentWeapon();

		RAY shotRay;
		shotRay.Position = m_pPlayer->GetPosition();;
	
		shotRay.Axis = camForward;
		shotRay.Length = 100.f;
		shotRay.RotationY = 0;

		float hitDist = 0.f;
		D3DXVECTOR3 hitPos = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 normal = D3DXVECTOR3(0.f, 0.f, 0.f);

		const int PELLET_COUNT = 8;
		const float SPREAD_ANGLE = D3DXToRadian(5.0f);
		switch (currWeapon)
		{
		case 0: // Pistol
			IsShotHit(shotRay, hitDist, hitPos, normal, impact);
			m_pBulletList[NextBullet()]->Reload(m_pPlayerWeapon->GetPosition(), camForward, m_pCamera->GetYaw());
			break;
		
		case 1: // Shotgun

			for (int i = 0; i < PELLET_COUNT; ++i)
			{
				// Generate random spread
				float randomX = ((rand() % 200 - 100) / 100.0f) * SPREAD_ANGLE;
				float randomY = ((rand() % 200 - 100) / 100.0f) * SPREAD_ANGLE;

				// Apply rotation to camera forward vector
				D3DXVECTOR3 spreadDir = camForward;
				D3DXMATRIX rotMatrix;
				D3DXMatrixRotationYawPitchRoll(&rotMatrix, randomX, randomY, 0.0f);
				D3DXVec3TransformNormal(&spreadDir, &camForward, &rotMatrix);
				D3DXVec3Normalize(&spreadDir, &spreadDir);

				// Create ray for each pellet
				RAY pelletRay;
				pelletRay.Position = m_pPlayer->GetPosition();
				pelletRay.Axis = spreadDir;
				pelletRay.Length = 100.f;
				pelletRay.RotationY = 0;
				float hitDist = 0.f;
				D3DXVECTOR3 hitPos = D3DXVECTOR3(0.f, 0.f, 0.f);
				D3DXVECTOR3 normal = D3DXVECTOR3(0.f, 0.f, 0.f);

				// Check for hits
				IsShotHit(pelletRay, hitDist, hitPos, normal, impact);
				// Spawn bullet visual for each pellet
				m_pBulletList[NextBullet()]->Reload(m_pPlayerWeapon->GetPosition(), spreadDir, m_pCamera->GetYaw());
			}

			break;

		default:
			break;
		}
		
		m_shotHandle = CEffect::Play(CEffect::PistolShotEffect, shotEffPos);
		CEffect::SetSpeed(m_shotHandle, 2.5f);
		CEffect::SetRotation(m_shotHandle, D3DXVECTOR3(0.f, 1.f, 0.f), m_pCamera->GetYaw());
		CEffect::SetScale(m_shotHandle, D3DXVECTOR3(0.025f, 0.025, 0.025));

	
	}
}



void CGame::HandleWeaponPos()
{
	D3DXVECTOR3 weaponPos = m_pCamera->GetPosition();
	D3DXVECTOR3 camForward, camRight, camUp;
	camForward = m_pCamera->GetForward();
	camRight = m_pCamera->GetRight();
	camUp = m_pCamera->GetUp();

	D3DXVECTOR3 localOffset;

	D3DXVECTOR3 pistolOffset =
		camRight * 0.25f +   // move to the right
		camUp * -0.25f +  // move a bit down
		camForward * 0.65f;     // move a bit forward

	D3DXVECTOR3 shotgunOffset =
		camRight * 0.4f +   // move to the right
		camUp * -0.25f +  // move a bit down
		camForward * 0.65f;     // move a bit forward
	D3DXMATRIX gunRecoilRot, gunOffset, gunScale;

	switch (m_pPlayer->GetCurrentWeapon())
	{
		case 0: // Pistol
			m_pPlayerWeapon->AttachMesh(*m_pPistolMesh);

			D3DXMatrixRotationY(&gunOffset, D3DXToRadian(180.f));
			D3DXMatrixRotationX(&gunRecoilRot, D3DXToRadian(0.f));
			D3DXMatrixScaling(&gunScale,1.f, 1.f, 1.f);
			localOffset = pistolOffset;
		break;
		case 1: // Shotgun
			m_pPlayerWeapon->AttachMesh(*m_pShotgunMesh);
		default:
			D3DXMatrixRotationY(&gunOffset, D3DXToRadian(0.f));
			D3DXMatrixRotationX(&gunRecoilRot, D3DXToRadian(0.f));
			D3DXMatrixScaling(&gunScale, 0.15f, 0.15f, 0.15f);
			localOffset = shotgunOffset;
		break;
	}

	weaponPos += localOffset;

	D3DXVECTOR3 playerVel = m_pPlayer->GetVelocity();
	float playerVelLen = D3DXVec3Length(&playerVel);


	if (!m_pPlayer->CanShoot())
	{
		D3DXMATRIX recoilRot;
		weaponPos += -camForward * 0.08f; // Recoil effect
		weaponPos += camUp * 0.04f * 0.5f; // Slight upward kick
		D3DXMatrixRotationX(&recoilRot, D3DXToRadian(-9.5f));
		gunRecoilRot += recoilRot * gunRecoilRot;
		D3DXVECTOR3 prevPos = m_pPlayerWeapon->GetPosition();
		m_pPlayerWeapon->VibrateAnim(0.016f, 0.035f, 0.8f);
		D3DXVECTOR3 diff = m_pPlayerWeapon->GetPosition() - prevPos;
		weaponPos += diff;
	}
	else
	{
		if (playerVelLen > 0.2f)
		{

			//Add effects to gun when moving here!!!
			weaponPos += camUp * 0.005f * sinf(m_pTime->GetTotalTime() * 0.009); // Bobbing effect when moving

		}
		else
		{
			weaponPos += camUp * 0.005f * sinf(m_pTime->GetTotalTime() * 0.005); // Bobbing effect when moving
		}
	}

	D3DXMATRIX weaponWorld;
	// left-handed, row-major: Right, Up, Forward, Position
	weaponWorld._11 = camRight.x;   weaponWorld._12 = camRight.y;   weaponWorld._13 = camRight.z;   weaponWorld._14 = 0.f;
	weaponWorld._21 = camUp.x;      weaponWorld._22 = camUp.y;      weaponWorld._23 = camUp.z;      weaponWorld._24 = 0.f;
	weaponWorld._31 = camForward.x; weaponWorld._32 = camForward.y; weaponWorld._33 = camForward.z; weaponWorld._34 = 0.f;
	weaponWorld._41 = weaponPos.x;  weaponWorld._42 = weaponPos.y;  weaponWorld._43 = weaponPos.z;  weaponWorld._44 = 1.f;

	weaponWorld = gunScale * gunOffset * gunRecoilRot * weaponWorld;

	m_pPlayerWeapon->SetPosition(weaponPos);
	m_pPlayerWeapon->SetRotation(D3DXVECTOR3(0.f, D3DXToRadian(180.f), 0.f));

	m_pPlayerWeapon->SetWorldMatrix(weaponWorld);
}

void CGame::IsShotHit(RAY& shotRay, float& hitDist, D3DXVECTOR3& hitPos, D3DXVECTOR3& normal, CGame::BULLET_IMPACT& impact)
{

	for (auto& enemy : m_pEnemyList)
	{
		if (enemy->IsDead() || !enemy->IsActive()) continue;
		if (enemy->IsHitForRay(shotRay, &hitDist, &hitPos, &normal))
		{
			impact.position = hitPos;
			impact.normal = normal;
			impact.isEnemyHit = true;
			enemy->ApplyDamage(5.f);
			if (enemy->IsDead())
			{
				//CEffect::Play(CEffect::EnemyDeathEffect, enemy->GetPosition());
				m_enemyKillCount++;
				m_comboCount++;
			}
			enemyHitHandle = CEffect::Play(CEffect::HitEffect, hitPos);
			CEffect::SetSpeed(enemyHitHandle, 2.0f);
			CEffect::SetScale(enemyHitHandle, D3DXVECTOR3(0.5f, 0.5f, 0.5f));
			//debugHitShotList.push_back(hitPos);
			//m_bulletImpactList.push_back(impact);
			return;
		}
	}

	//if (m_pEnemy->IsHitForRay(shotRay, &hitDist, &hitPos, &normal))
	//{
	//	impact.position = hitPos;
	//	impact.normal = normal;
	//	impact.isEnemyHit = true;
	//	//debugHitShotList.push_back(hitPos);
	//	//m_bulletImpactList.push_back(impact);
	//}
	if (m_pStage->IsHitForRay(shotRay, &hitDist, &hitPos, &normal))
	{
		impact.position = hitPos;
		impact.normal = normal;
		impact.isEnemyHit = false;
		// Hit detected
		//debugHitShotList.push_back(hitPos);
		m_bulletImpactList.push_back(impact);
	}
	
}

void CGame::HandlePlayerEnemyCollision()
{

	for (auto pEnemy : m_pEnemyList)
	{
		if (!pEnemy || pEnemy->IsDead() || !pEnemy->IsActive()) continue;

		D3DXVECTOR3 enemyPos = pEnemy->GetPosition();
		float enemyRadius = pEnemy->GetRadius();
		float distance = 0.f;
		HandleCollision(m_pPlayer, pEnemy, distance, false);

		if (pEnemy->GetState() == CAnimEnemy::Attacking && distance <= enemyRadius + 0.85f)
		{
			m_pPlayer->ApplyDamage(5.5f);

			m_highestCombo = max(m_highestCombo, m_comboCount);
			m_comboCount = 0;
		}
	}

}

void CGame::HandleEnemyEnemyCollision()
{
	size_t enemyCount = m_pEnemyList.size();
	float distance = 0.f;

	for (size_t i = 0; i < enemyCount; ++i)
	{
		if (!m_pEnemyList[i] || m_pEnemyList[i]->IsDead()) continue;

		for (size_t j = i + 1; j < enemyCount; ++j)
		{
			if (!m_pEnemyList[j] || m_pEnemyList[j]->IsDead()) continue;

			auto& enemyA = m_pEnemyList[i];
			auto& enemyB = m_pEnemyList[j];
			HandleCollision(enemyA, enemyB, distance);
		}
	}

}

void CGame::HandleEnemySpawning()
{

}

void CGame::HandleEnemyShooting()
{

	//BOSS MINION ENEMY SHOOTING
	float playerHeight = m_pPlayer->GetHeight();
	D3DXVECTOR3 dirToPlayer = m_pPlayer->GetPosition() + D3DXVECTOR3(0.f, -playerHeight * 0.5f, 0.f) - m_pEnemy->GetPosition();
	D3DXVec3Normalize(&dirToPlayer, &dirToPlayer);
	if (m_pEnemy->IsShot())
	{
		if(m_bossShotIndex > m_pBossShotList.size()-1)
		{
			m_bossShotIndex = 0;
		}
		auto minion = m_pBossShotList[m_bossShotIndex++];
		if (minion->IsActive() && !minion->IsDead())
			return;
		minion->InitEnemy();
		minion->SpawnAt(
			m_pEnemy->GetPosition() + D3DXVECTOR3(0.f, 2.f, 0.f));
		minion->SetPlayerPos(m_pPlayer->GetPosition());
		minion->LaunchAtPlayer(0.9f);
		
	}
	
	//RANGED ENEMY SHOOTING
	/*float playerHeight = m_pPlayer->GetHeight();
	D3DXVECTOR3 dirToPlayer = m_pPlayer->GetPosition() + D3DXVECTOR3(0.f,-playerHeight * 0.5f, 0.f) - m_pEnemy->GetPosition();
	D3DXVec3Normalize(&dirToPlayer, &dirToPlayer);

	if (m_pEnemy->IsShot())
	{
		m_pEnemyShotList[NextEnemyShot()]->Reload(
			m_pEnemy->GetPosition() + D3DXVECTOR3(0.f, 2.f, 0.f),
			dirToPlayer,
			m_pEnemy->GetRotation().y);
	}

	for (auto pEnemy : m_pEnemyList)
	{
		
		if(pEnemy->IsActive() && !pEnemy->IsDead() && pEnemy->IsShot())
		{
			m_pEnemyShotList[NextEnemyShot()]->Reload(
				pEnemy->GetPosition() + D3DXVECTOR3(0.f, 2.f, 0.f),
				dirToPlayer,
				pEnemy->GetRotation().y);
		}*/


	//}

}

void CGame::SetupBlockedPath()
{
	m_pBlockedPathList[0]->SetScale(12.f, 5.f, 3.f);
	m_pBlockedPathList[1]->SetScale(12.f, 5.f, 3.f);
	m_pBlockedPathList[0]->SetPosition(D3DXVECTOR3(0.f, 7.5f, 175.f));
	m_pBlockedPathList[1]->SetPosition(D3DXVECTOR3(0.f, 7.5f, 245.f));
}

void CGame::SetupTriggers()
{
	COLLISION_TRIGGER trigger1;
	trigger1.position = D3DXVECTOR3(0.f, 0.f, 180.f);
	trigger1.size = D3DXVECTOR3(100.f, 100.f, 1.f); // 10x10x4 box
	trigger1.blockedPathIndices.push_back(0); // Block the first blocked path
	trigger1.blockedPathIndices.push_back(1); // Block the second blocked path
	trigger1.isTriggered = false;
	trigger1.blockBehindPlayer = false;
	m_CollisionTriggerList.push_back(trigger1);
}

void CGame::CheckTriggers()
{

	for (auto& trigger : m_CollisionTriggerList)
	{
		if (trigger.isTriggered)
			continue;

		if (IsPlayerInTriggerArea(trigger))
		{
			trigger.isTriggered = true;

			for (auto index : trigger.blockedPathIndices)
			{
				if (index >= 0 && index < m_pBlockedPathList.size())
				{
					m_pBlockedPathList[index]->SetActive(true);

					if (trigger.blockBehindPlayer)
					{
						D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
						D3DXVECTOR3 pathPos = m_pBlockedPathList[index]->GetPosition();
						if (pathPos.z < playerPos.z)
						{
							m_pBlockedPathList[index]->SetActive(false);
						}
					}

				}
			}
		}
	}

}

bool CGame::IsPlayerInTriggerArea(const COLLISION_TRIGGER& trigger)
{

	D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();

	// AABB collision check
	return (playerPos.x >= trigger.position.x - trigger.size.x &&
		playerPos.x <= trigger.position.x + trigger.size.x &&
		playerPos.y >= trigger.position.y - trigger.size.y &&
		playerPos.y <= trigger.position.y + trigger.size.y &&
		playerPos.z >= trigger.position.z - trigger.size.z &&
		playerPos.z <= trigger.position.z + trigger.size.z);

	return false;

	return false;
}

bool CGame::IsPlayerInTriggerArea(const GOAL& trigger)
{
	D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();

	// AABB collision check
	return (playerPos.x >= trigger.position.x - trigger.size.x &&
		playerPos.x <= trigger.position.x + trigger.size.x &&
		playerPos.y >= trigger.position.y - trigger.size.y &&
		playerPos.y <= trigger.position.y + trigger.size.y &&
		playerPos.z >= trigger.position.z - trigger.size.z &&
		playerPos.z <= trigger.position.z + trigger.size.z);

	return false;
}


void CGame::SetupGoal()
{

	m_Goal.position = D3DXVECTOR3(0.f, 0.f, 300.f);
	m_Goal.size = D3DXVECTOR3(10.f, 100.f, 10.f); // 10x10x4 box

}

void CGame::CheckGoal()
{

	if (IsPlayerInTriggerArea(m_Goal))
	{
		// Player reached the goal
		SaveStats();
		m_pManager->ChangeScene("RESULT");
	}

}


int CGame::NextBullet()
{
	if(m_bulletIndex >= PLAYER_AMMO_MAX - 1)
	{
		m_bulletIndex = 0;
		return m_bulletIndex;
	}
	else
	{
		return m_bulletIndex++;
	}

}

int CGame::NextEnemyShot()
{
	if (m_enemyShotIndex >= PROJECTILE_COUNT_MAX - 1)
	{
		m_enemyShotIndex = 0;
		return m_enemyShotIndex;
	}
	else
	{
		return m_enemyShotIndex++;
	}
}


bool CGame::HandleCollision(CCharacter* charaA, CCharacter* charaB, float& distance, bool doubleCollision)
{

	D3DXVECTOR3 posA = charaA->GetPosition();
	D3DXVECTOR3 posB = charaB->GetPosition();
	float radiusA = charaA->GetRadius();
	float radiusB = charaB->GetRadius();

	// XZ平面での距離計算（Y軸を無視）
	D3DXVECTOR3 diff = posA - posB;
	diff.y = 0.0f;
	distance = D3DXVec3Length(&diff);
	float minDist = radiusA + radiusB;

	// 衝突検出
	if (distance < minDist && distance > 0.001f)
	{
		// 押し出しベクトルを計算
		D3DXVECTOR3 pushDir;
		D3DXVec3Normalize(&pushDir, &diff);

		float overlap = minDist - distance;

		// プレイヤーを押し出す（敵は動かない、または重量に応じて分配）
		D3DXVECTOR3 correction = pushDir * overlap * 0.5f;

		D3DXVECTOR3 newPos = posA + correction;
		newPos.y = posA.y; // Y座標を保持

		D3DXVECTOR3 newEnemyPos = posB - correction * 1.5f; // 敵も少し押し出す

		charaB->SetPosition(newEnemyPos);
		if (doubleCollision)
		{
			charaA->SetPosition(newPos);
		}
		return true;
	}

	return false;

}

void CGame::SaveStats()
{
	CGameStats::EnemiesKilled = m_enemyKillCount;
	CGameStats::HighestCombo = m_highestCombo;
	CGameStats::RemainingTime = static_cast<int>(m_stageTimer);
	CGameStats::ComputeScore();
}

HRESULT CGame::LoadSceneAssets()
{
	if (FAILED(m_pFont->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	// Load skybox cubemap texture
	if (FAILED(m_pSkybox->Init(*m_pDx11, L"Data\\Texture\\Skybox\\skybox.dds")))
	{
		return E_FAIL;
	}

	if (FAILED(CEffect::GetInstance()->LoadData()))
	{
		return E_FAIL;
	}

	if (FAILED(LoadUtilityMesh())) {
		return E_FAIL;
	}

	return S_OK;
}
HRESULT CGame::LoadUIAssets()
{
	CSprite2D::SPRITE_STATE healthBarState = {};
	healthBarState.Disp = { 350.f, 45.6f };
	healthBarState.Base = { 361.f, 79.f };
	healthBarState.Stride = { 361.f, 79.f };

	CSprite2D::SPRITE_STATE crossHairState = {};
	crossHairState.Disp = { 32.f, 32.f };
	crossHairState.Base = { 512.f, 512.f };
	crossHairState.Stride = { 512.f, 512.f };

	CSprite2D::SPRITE_STATE staminaBarState = {};
	staminaBarState.Disp = { 300.f, 45.6f };
	staminaBarState.Base = { 128.f, 43.0f };
	staminaBarState.Stride = { 128.f, 43.0f };

	if (FAILED(m_pHealthBarSprite->Init(*m_pDx11, L"Data\\Texture\\Health.png", healthBarState)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pStaminaBarSprite->Init(*m_pDx11, L"Data\\Texture\\dash.png", staminaBarState)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pCrossHairSprite->Init(*m_pDx11, L"Data\\Texture\\Cross.png", crossHairState)))
	{
		return E_FAIL;
	}

	m_pHealthBarUI->AttachSprite(*m_pHealthBarSprite);
	m_pStaminaBarUI->AttachSprite(*m_pStaminaBarSprite);
	m_pCrossHairUI->AttachSprite(*m_pCrossHairSprite);

	return S_OK;
}
void CGame::InitUI()
{
	m_pHealthBarUI->SetPosition(D3DXVECTOR3(20.0f, WND_H - 150.0f, 0.0f));
	m_pStaminaBarUI->SetPosition(D3DXVECTOR3(20.0f, WND_H - 100.0f, 0.0f));
	m_pCrossHairUI->SetPosition(D3DXVECTOR3((WND_W / 2) - 16.0f, (WND_H / 2) - 16.0f, 0.0f));
	m_pCrossHairUI->SetAlpha(0.7f);
}
HRESULT CGame::LoadSpriteAssets()
{
	CSprite3D::SPRITE_STATE lightningState = {};
	lightningState.Disp = { 2.5f, 0.5f };
	lightningState.Base = { 1024.f, 1892.f };
	lightningState.Stride = { 1024.f, 171.f };

	CSprite3D::SPRITE_STATE bulletLaserState = {};
	bulletLaserState.Disp = { 1.5f, 5.5f };
	bulletLaserState.Base = { 2048.f, 2048.f };
	bulletLaserState.Stride = { 512.f, 682.f };

	CSprite3D::SPRITE_STATE bulletHoleState = {};
	bulletHoleState.Disp = { 32.f,32.f };
	bulletHoleState.Base = { 512.f,512.f };
	bulletHoleState.Stride = { 512.f,512.f };

	if (FAILED(m_pLightningSprite->Init(*m_pDx11, L"Data\\Texture\\lightning.png", lightningState)))
	{
	}

	if (FAILED(m_pShotDecalSprite->Init(*m_pDx11, L"Data\\Texture\\bullet_hole.png", bulletHoleState)))
	{
	}

	if (FAILED(m_pEnemyHitDecalSprite->Init(*m_pDx11, L"Data\\Texture\\blood.png", bulletHoleState)))
	{
	}

	if (FAILED(m_pBulletLaserSprite->Init(*m_pDx11, L"Data\\Texture\\laser.png", bulletLaserState)))
	{
	}

	m_pBulletLaser->AttachSprite(*m_pBulletLaserSprite);
	m_pLightning->AttachSprite(*m_pLightningSprite);

	return S_OK;
}
void CGame::InitSpriteAssets()
{
	m_pBulletLaserSprite->SetBillboard(true);
	m_pLightning->AttachMesh(*m_pCubeMesh);
	m_pLightning->SetScale(3.f);
	m_pLightning->SetScale(10.f, 5.f, 3.f);
	m_pLightning->CreateCollider(CCollider::COLLIDER_SHAPE_CUBE);
}
HRESULT CGame::LoadUtilityMesh()
{
	if (FAILED(m_pGroundMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Ground\\ground.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pCubeMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Wall\\WallCol.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pSphereMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Collision\\Sphere.x")))
	{
		return E_FAIL;
	}

	return S_OK;
}
HRESULT CGame::LoadStageMesh()
{
	if (FAILED(m_pTestStageMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Stage\\stage.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pStageMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Stage\\Level003\\Level003.x")))
	{
		return E_FAIL;
	}

	m_pGround->AttachMesh(*m_pGroundMesh);

	m_pStage->AttachMesh(*m_pStageMesh);

	return S_OK;
}
void CGame::InitStage()
{
	m_pStage->SetScale(0.8f);
	m_pStage->SetPlayer(*m_pPlayer);
	m_pStage->SetEnemyList(m_pEnemyList);

	for (auto pBlockedPath : m_pBlockedPathList)
	{
		pBlockedPath->AttachMesh(*m_pCubeMesh);
		pBlockedPath->CreateCollider(CCollider::COLLIDER_SHAPE_CUBE);
		pBlockedPath->SetActive(false);
		pBlockedPath->SetPosition(0.f, -50.f, 0.f);
		pBlockedPath->SetRotation(0.f, 0.f, 0.f);
		pBlockedPath->AttachSprite(*m_pLightningSprite);
		pBlockedPath->SetScale(3.f);
	}

	SetupBlockedPath();
	SetupGoal();
	SetupTriggers();

}
HRESULT CGame::LoadEnemiesMesh()
{

	if (FAILED(m_pEnemyMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\Pigman\\Pigman.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pSpiderSkinMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\zako2\\zako2.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pSpiderMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\zako2\\zako2.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pRoboSkinMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\robo\\Robo.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pBossMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Enemy\\BrainRobot\\brain-robot.x")))
	{
		return E_FAIL;
	}

	m_pEnemy->AttachMesh(*m_pBossMesh);
	m_pEnemy->CreateCollider(CCollider::COLLIDER_SHAPE_CUBE);

	for (auto pEnemy : m_pEnemyList)
	{
		pEnemy->AttachMesh(*m_pSpiderMesh);
		pEnemy->AttachSkinMesh(*m_pSpiderSkinMesh);
	}

	for (auto pBossShot : m_pBossShotList)
	{
		pBossShot->SetActive(false);
		if (typeid(*pBossShot) == typeid(CSpider))
		{
			pBossShot = dynamic_cast<CSpider*>(pBossShot);
			pBossShot->AttachMesh(*m_pSpiderMesh);
			pBossShot->AttachSkinMesh(*m_pSpiderSkinMesh);
		}
		else if (typeid(*pBossShot) == typeid(CRobo))
		{
			pBossShot = dynamic_cast<CRobo*>(pBossShot);
			pBossShot->AttachMesh(*m_pSphereMesh);
		}
	}

	for (auto pEnemyShot : m_pEnemyShotList)
	{
		pEnemyShot->AttachMesh(*m_pSphereMesh);
		pEnemyShot->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);
	}

	return S_OK;
}
void CGame::InitEnemy()
{
	m_pBossMesh->SetRotation(D3DXVECTOR3(0.f, D3DXToRadian(180.f), 0.f));
	m_pEnemy->SetScale(4.9f);
	m_pEnemy->SetPlayerPos(m_pPlayer->GetPosition());

	for (auto pEnemy : m_pEnemyList)
	{
		pEnemy->SetActive(false);
		pEnemy->SetScale(2.2f);
	}

	for (auto pBossShot : m_pBossShotList)
	{
		pBossShot->SetActive(false);
		if (typeid(*pBossShot) == typeid(CSpider))
		{
			pBossShot = dynamic_cast<CSpider*>(pBossShot);
			pBossShot->SetScale(2.2f);
		}
		else if (typeid(*pBossShot) == typeid(CRobo))
		{
			pBossShot = dynamic_cast<CRobo*>(pBossShot);
			pBossShot->SetScale(2.2f);
		}
	}

	for (auto pEnemyShot : m_pEnemyShotList)
	{
		pEnemyShot->SetScale(1.5f);
		pEnemyShot->SetMoveSpeed(ENEMY_SHOT_SPEED);
	}

	m_pEnemy->SetPosition(D3DXVECTOR3(0.f, 15.f, 10.f));

	for (int i = 0; i < 4; i++)
	{
		m_pEnemyList[i]->InitEnemy();
		m_pEnemyList[i]->SetPosition(enemyStartPos[i]);
		m_pEnemyList[i]->SetActive(false);
	}

}
HRESULT CGame::LoadPlayerAsset()
{
	if (FAILED(m_pPistolMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Weapons\\gun3.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShotgunMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Weapons\\Shotgun\\shotgun.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pBulletMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Weapons\\Bullet\\bullet.x")))
	{
		return E_FAIL;
	}

	m_pPlayer->AttachMesh(*m_pSphereMesh);
	m_pPlayer->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);
	m_pPlayerWeapon->AttachMesh(*m_pPistolMesh);
	for (auto pBullet : m_pBulletList)
	{
		pBullet->AttachMesh(*m_pBulletMesh);
	}

	return S_OK;
}
void CGame::InitPlayer()
{
	m_pPlayer->SetScale(1.5f);
	m_pPlayer->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);
	m_pPlayerWeapon->SetRotation(D3DXVECTOR3(0.f, D3DXToRadian(180.f), 0.f));
	for (auto pBullet : m_pBulletList)
	{
		pBullet->SetScale(1.5f);
		pBullet->SetMoveSpeed(6.90f);
	}
}
