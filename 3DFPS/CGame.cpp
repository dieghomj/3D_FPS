#include "CGame.h"
#include "CSpider.h"
#include "CLaserShot.h"
#include "CBoss.h"

constexpr int PROJECTILE_COUNT_MAX = 32;
constexpr int PLAYER_AMMO_MAX = 999;
constexpr int ENEMY_COUNT_MAX = 64;
constexpr int ENEMY_COUNT_PER_ROOM = 4;
constexpr float ENEMY_SHOT_SPEED = 1.8f;
constexpr int STAGE_TIMER = 2 * 60; // minutes

const D3DXVECTOR3  PLAYER_STARTPOS[4] = {
		D3DXVECTOR3(0.f, 25.f, -95.f),
		D3DXVECTOR3(0.f, 25.f, -35.f),
		D3DXVECTOR3(0.f, 25.f, -35.f),
		D3DXVECTOR3(0.f, 25.f, -95.f),
};

const std::vector<CLevel::COLLISION_TRIGGER> COLLISION_TRIGGER_LIST[LEVEL_COUNT] = {
	
	{
		{ D3DXVECTOR3(0.f, 5.f, 250.f), D3DXVECTOR3(100.f, 50.f, 10.f), {0, 1}, false, false },
	},
	
	{	
		{ D3DXVECTOR3(0.f, 5.f, 120.f), D3DXVECTOR3(100.f, 50.f, 10.f), {0, 1}, false, false },
		{ D3DXVECTOR3(0.f, 42.6f, 290.f), D3DXVECTOR3(100.f, 50.f, 10.f), {2}, false, false },
		{ D3DXVECTOR3(0.f, 42.6f, 578.f), D3DXVECTOR3(100.f, 50.f, 10.f), {3,4}, false, false },
	},

	{
		{ D3DXVECTOR3(0.f, 1.f, 5.f),	 D3DXVECTOR3(500.f, 500.f, 500.f), {}, false, false },
	}

};


const std::vector<CGame::BLOCKED_PATH_TRANS> BLOCKEDPATH_LIST[LEVEL_COUNT] =
{

	{
		{ D3DXVECTOR3(0.f, 9.5f, 219.f), D3DXVECTOR3(12.f, 5.f, 7.f) },
		{ D3DXVECTOR3(0.f, 9.5f, 336.f), D3DXVECTOR3(12.f, 5.f, 7.f) },
	},

	{
		{ D3DXVECTOR3(0.f, 9.5f, 88.f), D3DXVECTOR3(12.f, 5.f, 7.f) },
		{ D3DXVECTOR3(0.f, 9.5f, 175.f), D3DXVECTOR3(18.f, 5.f, 7.f) },
		{ D3DXVECTOR3(0.f, 42.6f, 563.f), D3DXVECTOR3(18.f, 5.f, 7.f) },
		{ D3DXVECTOR3(0.f, 42.6f, 563.f), D3DXVECTOR3(18.f, 5.f, 7.f) },
		{ D3DXVECTOR3(0.f, 42.6f, 767.f), D3DXVECTOR3(18.f, 5.f, 7.f) },
	},

	{
	},

};

const CLevel::GOAL LevelGoals[LEVEL_COUNT] = {
	{ D3DXVECTOR3(0.f, 8.f, 400.f), D3DXVECTOR3(10.f, 10.f, 10.f), false },
	{ D3DXVECTOR3(0.f, 8.f, 920.f), D3DXVECTOR3(100.f, 100.f, 10.f), false },
	{ D3DXVECTOR3(0.f, 8.f, 920.f), D3DXVECTOR3(100.f, 100.f, 10.f), false },
};

const std::vector<D3DXVECTOR3> ENEMY_STARTPOS[LEVEL_COUNT] = {
	//LEVEL 1
	{
		D3DXVECTOR3( 39.f,		10.f,	231.f),
		D3DXVECTOR3( 5.3f,		10.f,	231.f),
		D3DXVECTOR3( -32.8f,	10.f,	231.f),
		D3DXVECTOR3( 10.8f,		10.f,	320.f),
		D3DXVECTOR3( -5.6f,		10.f,	320.f),
		D3DXVECTOR3( -39.f,		10.f,	320.f),
		D3DXVECTOR3( 5.3f,		10.f,	320.f),
		D3DXVECTOR3( 32.8f,		10.f,	320.f),
	},
	//LEVEL 2
	{	
		//FIRST AREA
		D3DXVECTOR3(-47.f,	48.f,	108.f),
		D3DXVECTOR3( 47.f,	68.f,	159.8f),
		D3DXVECTOR3(-18.f,	12.f,	200.f),
		D3DXVECTOR3( 18.f,	12.f,	200.f),

		//SECOND AREA
		D3DXVECTOR3(-188.6,	42.6f,	495.f),
		D3DXVECTOR3(-188.6,	42.6f,	436.f),
		D3DXVECTOR3(-188.6,	42.6f,	408.f),
		D3DXVECTOR3(-188.6,	42.6f,	350.f),

		D3DXVECTOR3( 188.6,	42.6f,	495.f),
		D3DXVECTOR3( 188.6,	42.6f,	436.f),
		D3DXVECTOR3( 188.6,	42.6f,	408.f),
		D3DXVECTOR3( 188.6,	42.6f,	350.f),
		
		//THIRD AREA
		D3DXVECTOR3( 128.6,	42.6f,	700.f),
		D3DXVECTOR3( 128.6,	42.6f,	636.f),
		D3DXVECTOR3( 128.6,	42.6f,	608.f),
		D3DXVECTOR3( 128.6,	42.6f,	650.f),
		
		D3DXVECTOR3(-128.6,	42.6f,	700.f),
		D3DXVECTOR3(-128.6,	42.6f,	636.f),
		D3DXVECTOR3(-128.6,	42.6f,	608.f),
		D3DXVECTOR3(-128.6,	42.6f,	650.f),
	},

	//BOSS LEVEL
	{
		D3DXVECTOR3( 0.f,	15.f,	1.f),
	}
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
	
	, m_pBoss(nullptr)
	, m_pBossEnemy(nullptr)
	, m_pEnemyPool()
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
	, m_pDebugColliderRender(nullptr)
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
	m_pPainSprite = new CSprite2D();
	m_pPainUI = new CUIObject();

	m_pSkybox = new CSkybox();

	m_pLevelController = new CLevelController();

	m_pGroundMesh = new CStaticMesh();
	m_pGround = new CStaticMeshObject();

	for (int i = 0; i < 3; i++)
	{
		m_pStageMeshes.push_back(new CStaticMesh());
	}

	m_pTestStageMesh = new CStaticMesh();
	m_pStageMesh = new CStaticMesh();
	m_pStage = new CStage();

	m_pCubeMesh = new CStaticMesh();
	m_pLightningSprite = new CSprite3D();
	m_pLightning = new CBlockedPath();

	for (int i = 0; i < LEVEL_COUNT; ++i)
	{
		for (int j = 0; j < BLOCKEDPATH_LIST[i].size(); ++j)
		{
			CBlockedPath* pBlockedPath = new CBlockedPath();
			m_pBlockedPathList[i].push_back(pBlockedPath);
		}
	}

	m_pEnemyMesh = new CStaticMesh();
	m_pEnemyShotMesh = new CStaticMesh();
	m_pSpiderMesh = new CStaticMesh();
	m_pRoboMesh = new CStaticMesh();
	m_pBossMesh = new CStaticMesh();
	m_pSpiderSkinMesh = new CSkinMesh();
	m_pRoboSkinMesh = new CSkinMesh();
	m_pBossSkinMesh = new CSkinMesh();
	m_pBossEnemy = new CBoss();
	m_pBoss = new CBoss();

	m_pEnemyPool.reserve(ENEMY_COUNT_MAX);
	m_pBossShotList.reserve(ENEMY_COUNT_MAX);
	
	// Get form csv later
	m_EnemyGroups [0] = {

		{
			8,
			false,
			false,
			7.f,
			{new CSpider, new CSpider, new CSpider, new CSpider, new CSpider, new CSpider, new CSpider, new CSpider,} // enemies will be spawned later
		},
	};

	m_EnemyGroups[1] = {

		{
			4,
			false,
			false,
			7.f,
			{new CRobo, new CRobo, new CSpider, new CSpider}
		},
		{
			8,
			false,
			false,
			7.f,
			{new CRobo, new CRobo, new CRobo, new CRobo, new CRobo, new CRobo, new CRobo, new CRobo}
		},

		{
			8,
			false,
			false,
			7.f,
			{new CRobo, new CRobo, new CRobo, new CRobo, new CRobo, new CRobo, new CRobo, new CRobo}
		},

	};

	m_EnemyGroups[2] = {

		{
			1,
			false,
			false,
			7.f,
			{new CBoss} 
		},
	};



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

	m_pSphereMesh = new CStaticMesh();


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
	debugRay = new CRay();
	debugShotRay = new CRay();
	m_pDebugColliderRender = new CDebugColliderRender();
#endif // DEBUG
}

HRESULT CGame::LoadData()
{
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

	// Initialize debug collider renderer
	if (m_pDebugColliderRender)
	{
		if (FAILED(m_pDebugColliderRender->Init(*m_pDx11)))
		{
			return E_FAIL;
		}
	}

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
	//Deleteオブジェクト解放.
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pStaminaBarSprite);
	SAFE_DELETE(m_pStaminaBarUI);
	SAFE_DELETE(m_pCrossHairSprite);
	SAFE_DELETE(m_pCrossHairUI);
	SAFE_DELETE(m_pHealthBarSprite);
	SAFE_DELETE(m_pHealthBarUI);
	SAFE_DELETE(m_pPainSprite);
	SAFE_DELETE(m_pPainUI);

	SAFE_DELETE(m_pSkybox);
	SAFE_DELETE(m_pGround);
	SAFE_DELETE(m_pGroundMesh);
	SAFE_DELETE(m_pStage);
	SAFE_DELETE(m_pTestStageMesh);
	SAFE_DELETE(m_pStageMesh);
	SAFE_DELETE(m_pCubeMesh);
	SAFE_DELETE(m_pLightningSprite);
	SAFE_DELETE(m_pLightning);
	for (auto pBlockedPath : m_pBlockedPathList[CGameStats::LevelSelection])
	{
		SAFE_DELETE(pBlockedPath);
	}
	SAFE_DELETE(m_pEnemyMesh);
	SAFE_DELETE(m_pSpiderMesh);
	SAFE_DELETE(m_pBossMesh);
	SAFE_DELETE(m_pSpiderSkinMesh);
	SAFE_DELETE(m_pRoboSkinMesh);
	SAFE_DELETE(m_pBossSkinMesh);
	SAFE_DELETE(m_pBossEnemy);
	SAFE_DELETE(m_pBoss);
	for (auto pEnemy : m_pEnemyPool)
	{
		SAFE_DELETE(pEnemy);
	}
	for (auto pEnemyShot : m_pEnemyShotList)
	{
		SAFE_DELETE(pEnemyShot);
	}
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_pBulletLaserSprite);
	SAFE_DELETE(m_pBulletLaser);
	SAFE_DELETE(m_pPistolMesh);
	SAFE_DELETE(m_pShotgunMesh);


#if _DEBUG
	SAFE_DELETE(m_pDebugColliderRender);
#endif

}

void CGame::Start()
{
	//ライト設定
	float lightIntensity	= 1.055f;
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
	InitPlayer();
	InitLevelController();
	InitEnemy();
	CSoundManager::PlayLoop(CSoundManager::BGM_Game);

	m_pPlayer->InitPlayer();
	m_pPlayer->SetPosition(PLAYER_STARTPOS[CGameStats::LevelSelection]);
	m_pLevelController->Restart();
	m_pStage->RestartPlayerPosition(PLAYER_STARTPOS[CGameStats::LevelSelection]);

	m_accumulatedTime = 0.0f;
	m_enemyKillCount = 0;
	m_comboCount = 0;
	m_deathCount = 0;
	m_stageTimer = STAGE_TIMER;

	CSoundManager::GetInstance()->CreateVoicePool(CSoundManager::SE_PistolShot1, 3, m_hWnd, 200);
	CSoundManager::GetInstance()->CreateVoicePool(CSoundManager::SE_ShotgunShot, 3, m_hWnd, 100);
	CSoundManager::GetInstance()->CreateVoicePool(CSoundManager::SE_Step2, 3,		m_hWnd, 200);
	CSoundManager::GetInstance()->CreateVoicePool(CSoundManager::SE_Step3, 3,		m_hWnd, 200);
	CSoundManager::GetInstance()->CreateVoicePool(CSoundManager::SE_PlayerHit, 1,	m_hWnd, 200);
	
	CGameStats::Reset();
}

static bool hasRestarted = false;
static bool needToRestart = false;

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
		needToRestart = true;
		CSoundManager::Stop(CSoundManager::BGM_Game);
		CSoundManager::PlayLoop(CSoundManager::BGM_Restart);

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			CSoundManager::Stop(CSoundManager::BGM_Restart);
			hasRestarted = true;
			needToRestart = false;
			Restart();
		}
		return;
	}

	if (m_pLevelController->GetCurrentLevel()->IsCleared())
	{
		GameClear();
		return;
	}

	HandleGameOver();

	//if (m_enemyKillCount >= ENEMY_COUNT_PER_ROOM)
	//{
	//	for (auto pBlockedPath : m_pBlockedPathList)
	//	{
	//		if (pBlockedPath->IsActive() == true)
	//		{
	//			pBlockedPath->SetActive(false);
	//		}
	//	}
	//	return;
	//}

	//if (m_pBlockedPathList[0]->IsActive() == true)
	//{
	//	for (int i = 0; i < ENEMY_COUNT_PER_ROOM; i++)
	//	{
	//		if (m_pEnemyPool[i]->IsActive() == false)
	//			m_pEnemyPool[i]->SpawnAt(ENEMY_STARTPOS[i]);
	//	}
	//}
	 
	
	//m_pGround->Update();

	//for (auto pBossShot : m_pBossShotList)
	//{
	//	if (pBossShot->IsActive())
	//	{
	//		pBossShot->Update();
	//		pBossShot->SetPlayerPos(m_pPlayer->GetPosition());
	//	}
	//}

	//m_pEnemyPool[1]->Update();
	//m_pBoss->RotateAnim(m_pTime->GetFixedDeltaTime(), D3DXToRadian(30.f));
	//m_pBoss->UpDownAnim(m_pTime->GetTotalTime(), 0.02f, 0.005f);

	m_pPlayer->Update();
	m_pPlayer->UpdateCollider();
	
	m_pLevelController->Update();

	float mSense = 0.f;
	if (m_pPlayer->IsDashing())
	{
		mSense = 0.045f;
	}
	else
	{
		mSense = 0.0f;
	}
	if (m_pPlayer->IsJumping())
	{
		mSense -= 0.05f;
	}

	m_pCameraController->FirstPersonCamera(m_pPlayer, m_mouseDelta, m_mouseSense - mSense);
	m_pCamera->Update();
	m_pCameraController->Update(0);

	HandleWeapon();

	//m_pBoss->Update();
	//m_pBoss->SetPlayerPos(m_pPlayer->GetPosition());

	HandleEnemySpawning();

	for (auto pEnemy : m_pEnemyPool)
	{
		pEnemy->Update();
		pEnemy->UpdateCollider();
		pEnemy->SetPlayerPos(m_pPlayer->GetPosition());
	}

	HandlePlayerEnemyCollision();
	HandleEnemyEnemyCollision();
	HandleEnemyShooting();
	HandleEnemyGroupCleared();

	// Boss level clear: wait for boss death animation to finish, then go to result
	if (CGameStats::LevelSelection == 2)
	{
		for (auto pEnemy : m_pEnemyPool)
		{
			if (!pEnemy || typeid(*pEnemy) != typeid(CBoss))
				continue;

			CBoss* pBoss = dynamic_cast<CBoss*>(pEnemy);
			if (pBoss && pBoss->GetState() == CAnimEnemy::Dead)
			{
				GameClear();
				return;
			}
		}
	}

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
			m_pPlayer->ApplyKnockback(pEnemyShot->GetPosition(), 3.5f);
			pEnemyShot->SetDisplay(false);
		}
	}

	HandlePlayerDashEffect();

	D3DXVECTOR3 playerVel = m_pPlayer->GetVelocity();
	playerVel.y = 0.f;
	float playerVelLen = D3DXVec3Length(&playerVel);

	static float stepTimer = 0.f;

	if (playerVelLen > 0.2f && m_pPlayer->IsGrounded() && !m_pPlayer->IsSliding() && !m_pPlayer->IsDashing())
	{
		if (stepTimer <= 0.f)
		{
			stepTimer = 800.f - (playerVelLen * 100.f); // 速度に応じて足音の間隔を変える
			if (stepTimer < 350.f)
			{
				stepTimer = 350.f; // 最小間隔
			}
			float randChoice = distFloat(m_RandomGen);
			if (randChoice < 0.5f)
				CSoundManager::PlaySEPoly(CSoundManager::SE_Step3);
			else
				CSoundManager::PlaySEPoly(CSoundManager::SE_Step2);
		}
		else
		{
			stepTimer -= FPS;
		}
	}
	
	static float fovY = D3DX_PI / 4.0f;
	if (m_pPlayer->IsDashing() && playerVelLen > 0.1f)
	{
		// FOV拡大
		float targetFovY = D3DX_PI / 3.0f; // 
		fovY += (targetFovY - fovY) * FPS / 1000.f; // スムーズに拡大
	}
	else
	{
		// FOV通常戻し
		float normalFovY = D3DX_PI / 4.0f;
		fovY += (normalFovY - fovY) * FPS / 1000.f * 1.5; // スムーズに戻す
	}

	m_pCamera->SetFieldOfView(fovY);

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

	if (GetAsyncKeyState('R') & 0x8000)
	{
		Restart();
	}

#endif
	CScene::Update();
}

void CGame::GameClear()
{
	SaveStats();
	if (CGameStats::LevelSelection + 1 < LEVEL_COUNT)
		CGameStats::UnlockedLevel[CGameStats::LevelSelection + 1] = true;
	CSoundManager::Stop(CSoundManager::BGM_Game);
	CSoundManager::PlaySE(CSoundManager::SE_Clear);
	m_pManager->ChangeScene("RESULT");
}

void CGame::HandleGameOver()
{
	if (m_stageTimer <= 0.f)
	{
		// Time's up - handle game over
		SaveStats();
		CSoundManager::Stop(CSoundManager::BGM_Game);
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
	CSoundManager::Stop(CSoundManager::BGM_Game);
	CSoundManager::PlayLoop(CSoundManager::BGM_Game);
	
	m_pPlayer->InitPlayer();
	m_pPlayer->SetPosition(PLAYER_STARTPOS[CGameStats::LevelSelection]);
	m_pStage->RestartPlayerPosition(PLAYER_STARTPOS[CGameStats::LevelSelection]);
	m_pLevelController->Restart();
	
	InitEnemy();

	m_enemyKillCount = 0;
	m_comboCount = 0;
	m_deathCount++;
	m_highestCombo = max(m_highestCombo, m_comboCount);
	m_comboCount = 0;

#if _DEBUG
	debugHitShotList.clear();
#endif
}


void CGame::Draw()
{
	m_pCamera->Draw(m_SceneInfo);

	if(needToRestart)
	{
		m_pFont->SetColor(1.f, 1.0, 1.f);
		TCHAR textOnScreen[256] = _T("PRESS [ MOUSE BUTTON ] FOR RESTART");
		m_pFont->Render(textOnScreen, WND_W / 2 - 200, WND_H / 2, 32.f);
		return;
	}

	// Render skybox first (before other objects)
	if (m_pSkybox)
	{
		m_pSkybox->Render(m_SceneInfo.mView, m_SceneInfo.mProj, m_SceneInfo.Camera.vPosition);
	}

	//m_pStage->Draw(m_SceneInfo);

	m_pLevelController->Draw(m_SceneInfo);

	m_pPlayerWeapon->Draw(m_SceneInfo);

	if (0)
	{
	m_pBoss->RenderStatic(m_SceneInfo);
	}

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
			pRobo->Draw(m_SceneInfo);
		}
	}

	for(auto pEnemy : m_pEnemyPool)
	{
		if (typeid(*pEnemy) == typeid(CRobo))
		{
			HandleEnemyShotLoadAnim(dynamic_cast<CRobo*>(pEnemy));
		}

		if (typeid(*pEnemy) == typeid(CBoss))
		{
			pEnemy->RenderStatic(m_SceneInfo);
		}
		else
		{
			pEnemy->Draw(m_SceneInfo);

		}
	}
	
	DrawEnemyShots();

	CEffect::GetInstance()->Draw(m_SceneInfo);

	D3DXMATRIX& mView = m_SceneInfo.mView;
	D3DXMATRIX& mProj = m_SceneInfo.mProj;
	LIGHT globalLight = m_SceneInfo.Light;
	D3DXVECTOR3 camPos = m_SceneInfo.Camera.vPosition;
	FOG fog = m_SceneInfo.Fog;
	SPOT_LIGHT* pSpotLightArray = m_SceneInfo.pSpotLightArray;
	int lightCount = m_SceneInfo.SpotLightNum;

	DrawDecals(mView, mProj);

	//ライトニングエフェクト、パターンアニメーション.（塞がれた道用）
	m_pLightningSprite->SetPatternNo(0,(int)(m_pTime->GetTotalTime() * 0.01f) % 11);

	for (auto pBlockedPath : m_pBlockedPathList[CGameStats::LevelSelection])
	{
		pBlockedPath->Draw(m_SceneInfo);
		pBlockedPath->UpdateCollider();
	}
	
	DrawUI();

	m_pFont->SetColor(1.f, 1.0, 1.f);
	TCHAR textOnScreen[256] = _T("");
	float seconds = static_cast<int>(m_stageTimer) % 60;
	float minutes = static_cast<int>(m_stageTimer) / 60;
	_stprintf_s(textOnScreen, _T("TIME: %02.f:%02.f"), minutes, seconds);
	m_pFont->Render(textOnScreen, WND_W - 32 * 9, 40, 32.f);
	_stprintf_s(textOnScreen, _T("COMBO: %d "), m_comboCount);
	
	const float comboAnimationDuration = 500.f;
	static int prevCombo = 0;
	static float timer = comboAnimationDuration;
	static float fontSize = 32.f;

	if(prevCombo != m_comboCount)
	{
		timer = 0.f;
		prevCombo = m_comboCount;
		//CSoundManager::PlaySE(CSoundManager::SE_ComboIncrease);
	}
	else {

		if (timer < comboAnimationDuration)
		{
			timer += 16.666f;
			fontSize += 1.f;
			m_pFont->SetColor(1.f, 0.f, 0.f);

		}
		else
		{
			if(fontSize > 32.f)
				fontSize-= 32.f - fontSize * 0.5 * 0.166;
			if (fontSize < 32.f)
				fontSize = 32.f;
			m_pFont->SetColor(1.f, 1.0, 1.f);
		}
	}

	if(m_comboCount != 0)
		m_pFont->Render(textOnScreen, WND_W - 49 * 9, WND_H/2 - 50.f, fontSize);


#if _DEBUG

	// Draw debug colliders for enemies and other objects
	DrawDebugColliders();

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

	TCHAR buff[256] = _T("");

	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pCrossRay[dir]->Render(
			m_SceneInfo.mView, m_SceneInfo.mProj, m_pPlayer->GetCrossRay().Ray[dir]);
		D3DXVECTOR3 rayPos = m_pPlayer->GetCrossRay().Ray[dir].Position;

		_stprintf_s(buff, _T("RayY Pos: (%.2f, %.2f, %.2f)"), rayPos.x, rayPos.y, rayPos.z);
		m_pFont->Render(buff, WND_W - 500, dir * 50, 18.f);
	}

	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pPrevCrossRay[dir]->Render(
			m_SceneInfo.mView, m_SceneInfo.mProj, m_prevCrossRay.Ray[dir], D3DXVECTOR4(1.0f, 0.f, 0.f, 1.f));
	}

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

void CGame::DrawUI()
{
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

	static float painAlpha = 0.f;
	float painObjectiveAlpha = 0.f;
	float painSpeed = 0.f;
	float painIncreaseSpeed = 3.0f;
	float painDecaySpeed = 0.5f;
	if(m_pPlayer->IsDamaged() && !m_pPlayer->IsDashing())
	{
		painObjectiveAlpha += 1.7f;
		painSpeed = painIncreaseSpeed;
	}
	else
	{
		painObjectiveAlpha = 0.0f;
		painSpeed = painDecaySpeed;
	}

	painAlpha += (painObjectiveAlpha - painAlpha) * painSpeed * FPS / 1000.f;
	if(needToRestart )
	{
		painAlpha = 0.0f;
	}
	m_pPainSprite->SetAlpha(painAlpha);
	m_pPainUI->Draw();

	m_pDx11->SetDepth(true);
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

		//if (CEffect::IsPlaying(enemyShotEffectHandles[i]))
		//{
		//	CEffect::SetLocation(enemyShotEffectHandles[i], m_pEnemyShotList[i]->GetPosition());
		//}
		//else
		//{
		//	enemyShotEffectHandles[i] = CEffect::Play(CEffect::MagmaEffect, m_pEnemyShotList[i]->GetPosition());
		//}
		m_pEnemyShotList[i]->Draw(m_SceneInfo);
	}
}

void CGame::HandleEnemyShotLoadAnim(CRobo* pEnemy)
{
	//RANGED ENEMY SHOT LOAD ANIMATION
	if (!pEnemy->IsShot() && pEnemy->IsActive())
	{
		float scale = pEnemy->GetAttackCD() * 0.5f;
		D3DXVECTOR3 vScale = D3DXVECTOR3(scale, scale, scale);
		D3DXVECTOR3 vPos = pEnemy->GetPosition() + D3DXVECTOR3(0.f, 4.5f, 0.f) - pEnemy->GetForward() * 3.5f;

		m_pEnemyShotMesh->SetScale(vScale);
		m_pEnemyShotMesh->SetPosition(vPos);

		D3DXMATRIX& mView = m_SceneInfo.mView;
		D3DXMATRIX& mProj = m_SceneInfo.mProj;
		LIGHT globalLight = m_SceneInfo.Light;
		D3DXVECTOR3 camPos = m_SceneInfo.Camera.vPosition;
		FOG fog = m_SceneInfo.Fog;
		SPOT_LIGHT* pSpotLightArray = m_SceneInfo.pSpotLightArray;
		int lightCount = m_SceneInfo.SpotLightNum;


		m_pEnemyShotMesh->Render(mView, mProj, globalLight, camPos, fog, pSpotLightArray, lightCount);
	}
}
#pragma region Player Weapon Methods

void CGame::HandleWeapon()
{
	HandleWeaponPos();
	int currWeapon = m_pPlayer->GetCurrentWeapon();
	D3DXVECTOR3 camForward = m_pCamera->GetForward();
	D3DXVECTOR3 bulletDir = camForward;
	D3DXVec3Normalize(&bulletDir, &bulletDir);
	D3DXVECTOR3 shotEffPos;
	D3DXVECTOR3 pistolMuzzleOffset = bulletDir * 0.25f + D3DXVECTOR3(0.f, 0.1f, 0.f);
	D3DXVECTOR3 shotgunMuzzleOffset = bulletDir * 0.7f + D3DXVECTOR3(0.f, 0.1f, 0.f);
	//m_pPlayerWeapon->GetPosition() + bulletDir * 0.25f + D3DXVECTOR3(0.f, 0.1f, 0.f)
	BULLET_IMPACT impact;

	impact.lifeTime = FPS * 1000; // 5 seconds

	if (m_pPlayer->IsShot())
	{

		RAY shotRay;
		shotRay.Position = m_pPlayer->GetPosition();;
	
		shotRay.Axis = camForward;
		shotRay.Length = 500.f;
		shotRay.RotationY = 0;

		float hitDist = 0.f;
		D3DXVECTOR3 hitPos = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 normal = D3DXVECTOR3(0.f, 0.f, 0.f);

		const int PELLET_COUNT = 4;
		const float SPREAD_ANGLE = D3DXToRadian(10.0f);
		switch (currWeapon)
		{
		case 0: // Pistol
			shotEffPos = m_pPlayerWeapon->GetPosition() + pistolMuzzleOffset;
			CSoundManager::PlaySEPoly(CSoundManager::SE_PistolShot1);
			IsShotHit(shotRay, hitDist, hitPos, normal, impact);
			m_pBulletList[NextBullet()]->Reload(m_pPlayerWeapon->GetPosition(), camForward, m_pCamera->GetYaw());
			break;
		
		case 1: // Shotgun
			CSoundManager::PlaySEPoly(CSoundManager::SE_ShotgunShot);
			shotEffPos = m_pPlayerWeapon->GetPosition() + shotgunMuzzleOffset;
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
				pelletRay.Length = 90.f;
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
		}
		m_shotHandle = CEffect::Play(CEffect::PistolShotEffect, shotEffPos);
		CEffect::SetSpeed(m_shotHandle, 5.5f);
		CEffect::SetRotation(m_shotHandle, D3DXVECTOR3(0.f, 1.f, 0.f), m_pCamera->GetYaw());
		CEffect::SetScale(m_shotHandle, D3DXVECTOR3(0.025f, 0.025, 0.025));
	}

	switch (currWeapon)
	{
		case 0: // Pistol
			CEffect::SetLocation(m_shotHandle, m_pPlayerWeapon->GetPosition() + pistolMuzzleOffset);
		case 1: // Shotgun
			CEffect::SetLocation(m_shotHandle, m_pPlayerWeapon->GetPosition() + shotgunMuzzleOffset);
		break;
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
	float minDistance = FLT_MAX;
	CAnimEnemy* hitEnemy = nullptr;

	// Broad-phase: use collider box (preferred) or radius to cull before precise hit
	D3DXVECTOR3 rayDir = shotRay.Axis;
	D3DXVec3Normalize(&rayDir, &rayDir);
	float rayLen = shotRay.Length;

	auto RayIntersectsAABB = [](const D3DXVECTOR3& origin, const D3DXVECTOR3& dir, float maxLen,
		const D3DXVECTOR3& bmin, const D3DXVECTOR3& bmax, float& outT) -> bool
	{
		float tmin = 0.0f;
		float tmax = maxLen;
		const float EPS = 1e-6f;

		for (int axis = 0; axis < 3; ++axis)
		{
			float o = (&origin.x)[axis];
			float d = (&dir.x)[axis];
			float minv = (&bmin.x)[axis];
			float maxv = (&bmax.x)[axis];

			if (fabsf(d) < EPS)
			{
				if (o < minv || o > maxv) return false;
			}
			else
			{
				float invD = 1.0f / d;
				float t1 = (minv - o) * invD;
				float t2 = (maxv - o) * invD;
				if (t1 > t2) std::swap(t1, t2);
				tmin = max(tmin, t1);
				tmax = min(tmax, t2);
				if (tmax < tmin) return false;
			}
		}
		outT = tmin;
		return tmin <= maxLen && tmax >= 0.0f;
	};

	for (auto& enemy : m_pEnemyPool)
	{
		if (enemy->IsDead() || !enemy->IsActive()) continue;

		bool broadPhaseHit = true;
		bool usedColliderHit = false;
		float colliderHitDist = 0.0f;
		D3DXVECTOR3 colliderHitPos(0,0,0);
		D3DXVECTOR3 colliderNormal(0,1,0);
		if (CCollider* col = enemy->GetCollider())
		{
			if (col->GetShape() == CCollider::COLLIDER_SHAPE_CUBE)
			{
				CBoundingCube* box = col->GetBBox();
				if (box)
				{
					D3DXVECTOR3 bmin = box->GetMin();
					D3DXVECTOR3 bmax = box->GetMax();
					if (RayIntersectsAABB(shotRay.Position, rayDir, rayLen, bmin, bmax, colliderHitDist))
					{
						broadPhaseHit = true;
						colliderHitPos = shotRay.Position + rayDir * colliderHitDist;
						// Approximate normal by which face is closest
						D3DXVECTOR3 center = (bmin + bmax) * 0.5f;
						D3DXVECTOR3 local = colliderHitPos - center;
						D3DXVECTOR3 halfExtents = (bmax - bmin) * 0.5f;
						float dx = fabsf(local.x) - halfExtents.x;
						float dy = fabsf(local.y) - halfExtents.y;
						float dz = fabsf(local.z) - halfExtents.z;
						if (dx > dy && dx > dz)
							colliderNormal = D3DXVECTOR3((local.x > 0) ? 1.f : -1.f, 0.f, 0.f);
						else if (dy > dz)
							colliderNormal = D3DXVECTOR3(0.f, (local.y > 0) ? 1.f : -1.f, 0.f);
						else
							colliderNormal = D3DXVECTOR3(0.f, 0.f, (local.z > 0) ? 1.f : -1.f);

						usedColliderHit = true;
					}
					else
					{
						broadPhaseHit = false;
					}
				}
			}
			else if (col->GetShape() == CCollider::COLLIDER_SHAPE_SPHERE)
			{
				CBoundingSphere* sph = col->GetBSphere();
				if (sph)
				{
					D3DXVECTOR3 toCenter = sph->GetPosition() - shotRay.Position;
					float r = sph->GetRadius();
					float t = D3DXVec3Dot(&toCenter, &rayDir);
					if (t < -r || t > rayLen + r)
						broadPhaseHit = false;
					else
					{
						D3DXVECTOR3 closest = shotRay.Position + rayDir * t;
						D3DXVECTOR3 diff = sph->GetPosition() - closest;
						broadPhaseHit = (D3DXVec3LengthSq(&diff) <= r * r);
					}
				}
			}
		}

		if (!broadPhaseHit)
			continue;

		// Use collider hit if we already have it, otherwise fall back to mesh ray test
		if (usedColliderHit)
		{
			hitDist = colliderHitDist;
			hitPos = colliderHitPos;
			normal = colliderNormal;
		}
		else if (!enemy->IsHitForRay(shotRay, &hitDist, &hitPos, &normal))
		{
			continue;
		}

		if (hitDist < minDistance)
		{
			minDistance = hitDist;
			hitEnemy = enemy;
		}
	}

	if (minDistance != FLT_MAX && hitEnemy != nullptr)
	{
		impact.position = hitPos;
		impact.normal = normal;
		impact.isEnemyHit = true;
		hitEnemy->ApplyDamage(5.f);
		if (hitEnemy->IsDead())
		{
			m_enemyKillCount++;
			m_comboCount++;
		}
		int currWeapon = m_pPlayer->GetCurrentWeapon();
		switch (currWeapon)
		{
		case 0: // Pistol
			enemyHitHandle = CEffect::Play(CEffect::PistolShotEffect, hitPos);
			CEffect::SetRotation(enemyHitHandle, D3DXVECTOR3(0.f, 1.f, 0.f), m_pCamera->GetYaw());
			break;
		case 1: // Shotgun
			enemyHitHandle = CEffect::Play(CEffect::ExplosionEffect, hitPos);
			CEffect::SetRotation(enemyHitHandle, D3DXVECTOR3(0.f, 1.f, 0.f), m_pCamera->GetYaw());
			break;
		}
		CEffect::SetSpeed(enemyHitHandle, 2.0f);
		CEffect::SetScale(enemyHitHandle, D3DXVECTOR3(0.8f, 0.8f, 0.8f));
		return;
	}

	if (m_pLevelController->GetStage()->IsHitForRay(shotRay, &hitDist, &hitPos, &normal))
	{
		impact.position = hitPos;
		impact.normal = normal;
		impact.isEnemyHit = false;
		m_bulletImpactList.push_back(impact);
		enemyHitHandle = CEffect::Play(CEffect::HitEffect, hitPos);
	}
	CEffect::SetSpeed(enemyHitHandle, 2.0f);
	CEffect::SetScale(enemyHitHandle, D3DXVECTOR3(0.8f, 0.8f, 0.8f));
	
}

#pragma endregion

#pragma region Collision Methods
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
		
		// Full separation with slight buffer to prevent repeated collision
		float separationBuffer = 0.02f;
		float totalPush = overlap + separationBuffer;

		if (doubleCollision)
		{
			// Both objects move - split the correction
			D3DXVECTOR3 correctionA = pushDir * (totalPush * 0.5f);
			D3DXVECTOR3 correctionB = -pushDir * (totalPush * 0.5f);

			D3DXVECTOR3 newPosA = posA + correctionA;
			D3DXVECTOR3 newPosB = posB + correctionB;
			newPosA.y = posA.y;
			newPosB.y = posB.y;

			charaA->SetPosition(newPosA);
			charaB->SetPosition(newPosB);

			// Dampen velocity in collision direction
			D3DXVECTOR3 velA = charaA->GetVelocity();
			D3DXVECTOR3 velB = charaB->GetVelocity();
			
			float velDotA = D3DXVec3Dot(&velA, &pushDir);
			float velDotB = D3DXVec3Dot(&velB, &pushDir);
			
			// Remove velocity component pushing into each other
			if (velDotA < 0.f)
			{
				velA -= pushDir * velDotA * 0.8f;
				charaA->SetVelocity(velA);
			}
			if (velDotB > 0.f)
			{
				velB -= pushDir * velDotB * 0.8f;
				charaB->SetVelocity(velB);
			}
		}
		else
		{
			// Only charaB moves (enemy pushed away from player)
			D3DXVECTOR3 correction = -pushDir * totalPush;
			D3DXVECTOR3 newPosB = posB + correction;
			newPosB.y = posB.y;
			charaB->SetPosition(newPosB);

			// Dampen enemy velocity toward player
			D3DXVECTOR3 velB = charaB->GetVelocity();
			float velDot = D3DXVec3Dot(&velB, &pushDir);
			if (velDot > 0.f)
			{
				velB -= pushDir * velDot * 0.9f;
				charaB->SetVelocity(velB);
			}
		}
		return true;
	}

	return false;
}

bool CGame::HandleCubeCollisions(CStaticMeshObject* pMeshObj, CCharacter* pChara)
{
	if (pMeshObj->IsActive() == false)
	{
		return false;
	}

	CCollider* pCollider = pMeshObj->GetCollider();
	if (pCollider == nullptr)
	{
		return false;
	}

	CBoundingCube* pBox = pCollider->GetBBox();
	if (pBox == nullptr)
	{
		return false;
	}

	const D3DXVECTOR3 minBound = pBox->GetMin();
	const D3DXVECTOR3 maxBound = pBox->GetMax();

	const float playerRadius = pChara->GetRadius();
	const float playerHeight = pChara->GetHeight();
	D3DXVECTOR3 playerPos = pChara->GetPosition();
	const float playerFeet = playerPos.y - playerHeight;
	const float playerHead = playerPos.y + playerHeight * 0.3f;

	const bool overlapX = (playerPos.x + playerRadius) > minBound.x && (playerPos.x - playerRadius) < maxBound.x;
	const bool overlapZ = (playerPos.z + playerRadius) > minBound.z && (playerPos.z - playerRadius) < maxBound.z;
	const bool overlapY = playerHead > minBound.y && playerFeet < maxBound.y;

	if (!(overlapX && overlapZ && overlapY))
	{
		return false;
	}

	const float centerX = (minBound.x + maxBound.x) * 0.5f;
	const float centerZ = (minBound.z + maxBound.z) * 0.5f;

	float penetrationX = (playerPos.x >= centerX)
		? maxBound.x - (playerPos.x - playerRadius)
		: (playerPos.x + playerRadius) - minBound.x;
	float penetrationZ = (playerPos.z >= centerZ)
		? maxBound.z - (playerPos.z - playerRadius)
		: (playerPos.z + playerRadius) - minBound.z;

	if (penetrationX <= 0.f && penetrationZ <= 0.f)
	{
		return false;
	}

	D3DXVECTOR3 resolvedPos = playerPos;
	D3DXVECTOR3 playerVelocity = pChara->GetVelocity();

	if (penetrationX < penetrationZ)
	{
		const float pushDir = (playerPos.x >= centerX) ? 1.f : -1.f;
		resolvedPos.x += pushDir * penetrationX;
		playerVelocity.x = 0.f;
	}
	else
	{
		const float pushDir = (playerPos.z >= centerZ) ? 1.f : -1.f;
		resolvedPos.z += pushDir * penetrationZ;
		playerVelocity.z = 0.f;
	}
	
	pChara->SetPosition(resolvedPos);
	pChara->SetVelocity(playerVelocity);

	return true;
}

bool CGame::HandleColliderCollision(CStaticMeshObject* pObjA, CCharacter* pChara, bool resolveForChara)
{
	if (!pObjA || !pChara) return false;
	if (!pObjA->IsActive()) return false;

	CCollider* pColliderA = pObjA->GetCollider();
	CCollider* pColliderB = pChara->GetCollider();

	if (!pColliderA || !pColliderB) return false;

	D3DXVECTOR3 penetration(0, 0, 0);
	
	// コライダー同士の衝突判定
	if (pColliderB->CheckCollision(pColliderA, &penetration))
	{
		if (resolveForChara)
		{
			// キャラクターを押し出す
			D3DXVECTOR3 charaPos = pChara->GetPosition();
			D3DXVECTOR3 charaVel = pChara->GetVelocity();

			// Y方向の押し出しは無視（床の衝突は別処理）
			penetration.y = 0.0f;

			charaPos += penetration;
			pChara->SetPosition(charaPos);

			// 押し出し方向の速度をゼロにする
			if (fabsf(penetration.x) > 0.001f)
			{
				charaVel.x = 0.0f;
			}
			if (fabsf(penetration.z) > 0.001f)
			{
				charaVel.z = 0.0f;
			}
			pChara->SetVelocity(charaVel);
		}
		return true;
	}

	return false;
}

void CGame::HandleWallCollisions(CStaticMeshObject* pMeshObj, CCharacter* pChara)
{
	if (pMeshObj->IsActive() == false)
	{
		return;
	}

	CCollider* pCollider = pMeshObj->GetCollider();
	if (pCollider == nullptr)
	{
		return;
	}

	CBoundingCube* pBox = pCollider->GetBBox();
	if (pBox == nullptr)
	{
		return;
	}

	const D3DXVECTOR3 minBound = pBox->GetMin();
	const D3DXVECTOR3 maxBound = pBox->GetMax();

	const float playerRadius = pChara->GetRadius();
	const float playerHeight = pChara->GetHeight();
	D3DXVECTOR3 playerPos = pChara->GetPosition();
	const float playerFeet = playerPos.y - playerHeight;
	const float playerHead = playerPos.y + playerHeight * 0.3f;

	const bool overlapX = (playerPos.x + playerRadius) > minBound.x && (playerPos.x - playerRadius) < maxBound.x;
	const bool overlapZ = (playerPos.z + playerRadius) > minBound.z && (playerPos.z - playerRadius) < maxBound.z;
	const bool overlapY = playerHead > minBound.y && playerFeet < maxBound.y;

	if (!(overlapX && overlapZ && overlapY))
	{
		return;
	}

	const float centerX = (minBound.x + maxBound.x) * 0.5f;
	const float centerZ = (minBound.z + maxBound.z) * 0.5f;

	float penetrationX = (playerPos.x >= centerX)
		? maxBound.x - (playerPos.x - playerRadius)
		: (playerPos.x + playerRadius) - minBound.x;
	float penetrationZ = (playerPos.z >= centerZ)
		? maxBound.z - (playerPos.z - playerRadius)
		: (playerPos.z + playerRadius) - minBound.z;

	if (penetrationX <= 0.f && penetrationZ <= 0.f)
	{
		return;
	}

	D3DXVECTOR3 resolvedPos = playerPos;
	D3DXVECTOR3 playerVelocity = pChara->GetVelocity();

	if (penetrationX < penetrationZ)
	{
		const float pushDir = (playerPos.x >= centerX) ? 1.f : -1.f;
		resolvedPos.x += pushDir * penetrationX;
		playerVelocity.x = 0.f;
	}
	else
	{
		const float pushDir = (playerPos.z >= centerZ) ? 1.f : -1.f;
		resolvedPos.z += pushDir * penetrationZ;
		playerVelocity.z = 0.f;
	}
	
	pChara->SetPosition(resolvedPos);
	pChara->SetVelocity(playerVelocity);
}


void CGame::HandlePlayerEnemyCollision()
{
	const int MAX_COLLISION_ITERATIONS = 3;
	
	for (int iteration = 0; iteration < MAX_COLLISION_ITERATIONS; ++iteration)
	{
		bool hadCollision = false;
		
		for (auto pEnemy : m_pEnemyPool)
		{
			if (!pEnemy || pEnemy->IsDead() || !pEnemy->IsActive()) continue;

			CCollider* pEnemyCollider = pEnemy->GetCollider();
			float distance = 0.f;
			bool collided = false;

			// コライダーがある場合はコライダーベースの衝突判定を使用
			if (pEnemyCollider)
			{
				collided = HandleColliderCollision(pEnemy, m_pPlayer, true);
			
			// 距離計算（ダメージ判定用）
				D3DXVECTOR3 diff = m_pPlayer->GetPosition() - pEnemy->GetPosition();
				diff.y = 0.0f;
				distance = D3DXVec3Length(&diff);
			}
			else
			{
				// コライダーがない場合は従来の球ベースの衝突判定
				if (typeid(*pEnemy) == typeid(CBoss))
				{
					collided = HandleCubeCollisions(pEnemy, m_pPlayer);
					D3DXVECTOR3 diff = m_pPlayer->GetPosition() - pEnemy->GetPosition();
					diff.y = 0.0f;
					distance = D3DXVec3Length(&diff);
				}
				else
				{
					// Push enemy away from player (doubleCollision = false)
					collided = HandleCollision(m_pPlayer, pEnemy, distance, false);
				}
			}

			if (collided)
			{
				hadCollision = true;
				
				// ダメージ処理 (only on first iteration to avoid multiple damage)
				if (iteration == 0 && ((pEnemy->GetState() == CAnimEnemy::Attacking) || (pEnemy->GetState() == CAnimEnemy::Jumping)))
				{
					float damage = 5.5f;
					if (typeid(*pEnemy) == typeid(CBoss))
					{
						damage = 12.f;
					}
					m_pPlayer->ApplyDamage(damage);
					if(m_pPlayer->IsDamaged())
						CSoundManager::PlaySEPoly(CSoundManager::SE_PlayerHit);
					m_highestCombo = max(m_highestCombo, m_comboCount);
					m_comboCount = 0;
				}
			}
		}
		
		// If no collisions this iteration, we're done
		if (!hadCollision) break;
	}
}

void CGame::HandleEnemyEnemyCollision()
{
	const int MAX_COLLISION_ITERATIONS = 2;
	size_t enemyCount = m_pEnemyPool.size();
	
	for (int iteration = 0; iteration < MAX_COLLISION_ITERATIONS; ++iteration)
	{
		bool hadCollision = false;
		
		for (size_t i = 0; i < enemyCount; ++i)
		{
			if (!m_pEnemyPool[i] || m_pEnemyPool[i]->IsDead()) continue;

		if(typeid(*m_pEnemyPool[i]) == typeid(CBoss))
		{
			continue;
		}

		for (size_t j = i + 1; j < enemyCount; ++j)
		{
			if (!m_pEnemyPool[j] || m_pEnemyPool[j]->IsDead()) continue;
			
			if (typeid(*m_pEnemyPool[j]) == typeid(CBoss))
			{
				continue;
			}

			auto& enemyA = m_pEnemyPool[i];
			auto& enemyB = m_pEnemyPool[j];
			float distance = 0.f;
			if (HandleCollision(enemyA, enemyB, distance, true))
			{
				hadCollision = true;
			}
		}
	}
		
		if (!hadCollision) break;
	}
}
#pragma endregion

void CGame::HandleEnemySpawning()
{

	const float SPIDER_SCALE = 4.5f;
	const float ROBO_SCALE = 0.01f;
	const float BOSS_SCALE = 2.f;

	auto level = m_pLevelController->GetCurrentLevel();
	int currLevelIndex = CGameStats::LevelSelection;
	int triggerCount = level->GetTriggerAreaCount();

	for (int i = 0; i < triggerCount; i++)
	{
		if (level->IsAreaTriggered(i) && !m_EnemyGroups[currLevelIndex][i].isSpawned)
		{
			m_EnemyGroups[currLevelIndex][i].isSpawned = true;
			auto enemyList = m_EnemyGroups[currLevelIndex][i].enemies;
			for(auto enemy : enemyList)
			{
				int poolCount = static_cast<int>(m_pEnemyPool.size());

				if (typeid(*enemy) == typeid(CSpider))
					enemy->SetScale(SPIDER_SCALE);
				if (typeid(*enemy) == typeid(CRobo))
					enemy->SetScale(ROBO_SCALE);
				if (typeid(*enemy) == typeid(CBoss))
				{
					enemy->SetScale(BOSS_SCALE);
					CSoundManager::PlaySE(CSoundManager::SE_BossSE1);
				}
				else 
					CSoundManager::PlaySE(CSoundManager::SE_ItemGet);
				
				enemy->InitEnemy();
				enemy->SpawnAt(ENEMY_STARTPOS[currLevelIndex][poolCount]);
				m_pEnemyPool.push_back(enemy);
			}

			m_pLevelController->GetStage()->SetEnemyList(m_pEnemyPool);
		}
	}
}

void CGame::HandleEnemyShooting()
{
	//BOSS MINION ENEMY SHOOTING
	float playerHeight = m_pPlayer->GetHeight();
	D3DXVECTOR3 dirToPlayer = m_pPlayer->GetPosition() + D3DXVECTOR3(0.f, -playerHeight * 0.5f, 0.f) - m_pBoss->GetPosition();
	D3DXVec3Normalize(&dirToPlayer, &dirToPlayer);

	if (m_pBoss->IsShot())
	{

		
	}
	
	//RANGED ENEMY SHOOTING
	for (auto pEnemy : m_pEnemyPool)
	{
		D3DXVECTOR3 dirToPlayer = m_pPlayer->GetPosition() + D3DXVECTOR3(0.f, -playerHeight * 0.2f, 0.f) - (pEnemy->GetPosition() + D3DXVECTOR3(0.f, 7.f, 0.f));
		D3DXVec3Normalize(&dirToPlayer, &dirToPlayer);

		if (typeid(*pEnemy) == typeid(CRobo))
		{
			if(pEnemy->IsActive() && !pEnemy->IsDead() && pEnemy->IsShot())
			{
				m_pEnemyShotList[NextEnemyShot()]->Reload(
					pEnemy->GetPosition() + D3DXVECTOR3(0.f, 7.f, 0.f),
					dirToPlayer,
					pEnemy->GetRotation().y);
			}
			continue;
		}
		if (typeid(*pEnemy) == typeid(CBoss))
		{
			if (pEnemy->IsActive() && !pEnemy->IsDead() && pEnemy->IsShot())
			{
				if (m_bossShotIndex > m_pBossShotList.size() - 1)
				{
					m_bossShotIndex = 0;
				}
				auto minion = m_pBossShotList[m_bossShotIndex++];
				if (minion->IsActive() && !minion->IsDead())
					continue;
				minion->InitEnemy();
				minion->CreateCollider(CCollider::COLLIDER_SHAPE_CUBE);
				minion->SpawnAt(
					pEnemy->GetPosition() + D3DXVECTOR3(0.f, 2.f, 0.f));
				minion->SetPlayerPos(m_pPlayer->GetPosition());
				minion->LaunchAtPlayer(0.9f);
				m_pEnemyPool.push_back(minion);
			}
			continue;
		}


	}

}

void CGame::HandleEnemyGroupCleared()
{

	for (int i = 0; i < m_EnemyGroups[CGameStats::LevelSelection].size(); i++)
	{
		auto& group = m_EnemyGroups[CGameStats::LevelSelection][i];
		if (!group.isSpawned || group.isCleared)
			continue;
		bool allDead = true;
		for (auto enemy : group.enemies)
		{
			if (enemy && enemy->IsActive() && !enemy->IsDead())
			{
				allDead = false;
				break;
			}
		}

		if (allDead)
		{
			group.isCleared = true;

			auto pathIndices = COLLISION_TRIGGER_LIST[CGameStats::LevelSelection][i].blockedPathIndices;
			
			for (auto index : pathIndices)
			{
				m_pBlockedPathList[CGameStats::LevelSelection][index]->SetActive(false);
			}

		}

	}

}

void CGame::SetupBlockedPath()
{

	int i = 0;
	for (int i = 0; i < BLOCKEDPATH_LIST[CGameStats::LevelSelection].size(); i++)
	{
		auto pBlockedPath = m_pBlockedPathList[CGameStats::LevelSelection][i];
		float scaleX = BLOCKEDPATH_LIST[CGameStats::LevelSelection][i].scale.x;
		float scaleY = BLOCKEDPATH_LIST[CGameStats::LevelSelection][i].scale.y;
		float scaleZ = BLOCKEDPATH_LIST[CGameStats::LevelSelection][i].scale.z;
		D3DXVECTOR3 pos = BLOCKEDPATH_LIST[CGameStats::LevelSelection][i].position;
		pBlockedPath->SetScale( scaleX, scaleY, scaleZ );
		pBlockedPath->SetPosition(pos);
	}

}

void CGame::SetupTriggers()
{

}
void CGame::SetupGoal()
{

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

void CGame::SaveStats()
{
	CGameStats::EnemiesKilled = m_enemyKillCount;
	if (m_highestCombo <= m_comboCount)
	{
		m_highestCombo = m_comboCount;
	}
	CGameStats::HighestCombo = m_highestCombo;
	CGameStats::RemainingTime = static_cast<int>(m_stageTimer);
	CGameStats::DeathCounter = m_deathCount;
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
void CGame::InitScene(float fovY, float aspect, float zn, float zf, float lightIntensity, const D3DXVECTOR3& lightDir, bool fog)
{
	m_pCamera->SetPerspective(fovY,
		aspect,
		zn, zf);
	m_GlobalLight.fIntensity = lightIntensity;
	m_GlobalLight.vDirection = lightDir;
	m_Fog.Enable = fog;
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
	
	CSprite2D::SPRITE_STATE damageEffectState = {};
	damageEffectState.Disp = { WND_W, WND_H};
	damageEffectState.Base = { 1920.f, 1080.f};
	damageEffectState.Stride = { 1920.f, 1080.f };


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

	if (FAILED(m_pPainSprite->Init(*m_pDx11, L"Data\\Texture\\pain.png", damageEffectState)))
	{
		return E_FAIL;
	}

	m_pPainUI->AttachSprite(*m_pPainSprite);
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

	TCHAR buff[256];
	int count = 1;
	for(auto pMesh : m_pStageMeshes)
	{
		_stprintf_s(buff, _T("Data\\Mesh\\Static\\Stage\\Level00%d\\Level00%d.x"), count, count);
		if (FAILED(pMesh->Init(*m_pDx9, *m_pDx11, buff)))
		{
			return E_FAIL;
		}
		count++;
	}

	m_pGround->AttachMesh(*m_pGroundMesh);
	m_pStage->AttachMesh(*m_pStageMesh);

	return S_OK;
}
void CGame::InitStage()
{
	m_pStage->SetScale(0.8f);
	m_pStage->SetPlayer(*m_pPlayer);
	m_pStage->SetEnemyList(m_pEnemyPool);

	for (auto pBlockedPath : m_pBlockedPathList[CGameStats::LevelSelection])
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

	if (FAILED(m_pSpiderSkinMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\zako2\\zako2.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pSpiderMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\zako2\\zako2.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pRoboMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\zako\\zako.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pRoboSkinMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\zako\\zako.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pBossMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Enemy\\BrainRobot\\brain-robot.x")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pEnemyShotMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Weapons\\Spike\\spike.x")))
	{
		return E_FAIL;
	}

	for (auto& group : m_EnemyGroups[CGameStats::LevelSelection])
	{
		for (auto& pEnemy : group.enemies)
		{
			if (typeid(*pEnemy) == typeid(CSpider))
			{
				pEnemy = dynamic_cast<CSpider*>(pEnemy);
				pEnemy->AttachMesh(*m_pSpiderMesh);
				pEnemy->AttachSkinMesh(*m_pSpiderSkinMesh);
				pEnemy->CreateCollider(CCollider::COLLIDER_SHAPE_CUBE);
			}
			else if (typeid(*pEnemy) == typeid(CRobo))
			{
				pEnemy = dynamic_cast<CRobo*>(pEnemy);
				pEnemy->AttachMesh(*m_pRoboMesh);
				pEnemy->AttachSkinMesh(*m_pRoboSkinMesh);
				pEnemy->CreateCollider(CCollider::COLLIDER_SHAPE_CUBE);
			}
			else if (typeid(*pEnemy) == typeid(CBoss))
			{
				pEnemy = dynamic_cast<CBoss*>(pEnemy);
				pEnemy->AttachMesh(*m_pBossMesh);
				pEnemy->CreateCollider(CCollider::COLLIDER_SHAPE_CUBE);
			}
		}
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
			pBossShot->AttachMesh(*m_pRoboMesh);
			pBossShot->AttachSkinMesh(*m_pRoboSkinMesh);
		}
	}

	for (auto pEnemyShot : m_pEnemyShotList)
	{
		pEnemyShot->AttachMesh(*m_pEnemyShotMesh);
		pEnemyShot->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);
	}

	return S_OK;
}
void CGame::InitEnemy()
{
	const float SPIDER_SCALE = 4.5f;
	const float ROBO_SCALE = 0.01f;
	const float BOSS_SCALE = 2.f;

	//敵の非アクティブ化.
	for (auto pEnemy : m_pEnemyPool)
	{
		pEnemy->InitEnemy();
		pEnemy->SetActive(false);
		pEnemy->SetPosition(0.f, -100.f, -500.f);


	}
	m_pEnemyPool.clear();

	for (auto pBossShot : m_pBossShotList)
	{
		pBossShot->InitEnemy();
		pBossShot->SetActive(false);
		pBossShot->SetPosition(0.f, -100.f, -500.f);
		if (typeid(*pBossShot) == typeid(CSpider))
			pBossShot->SetScale(SPIDER_SCALE);
		if (typeid(*pBossShot) == typeid(CRobo))
			pBossShot->SetScale(ROBO_SCALE);
		if (typeid(*pBossShot) == typeid(CBoss))
			pBossShot->SetScale(BOSS_SCALE);
	}

	for (auto& group : m_EnemyGroups[CGameStats::LevelSelection])
	{
		group.isSpawned = false;
		group.isCleared = false;
	}

	m_pBossMesh->SetRotation(D3DXVECTOR3(0.f, D3DXToRadian(180.f), 0.f));

	for (auto pEnemyShot : m_pEnemyShotList)
	{
		pEnemyShot->SetScale(1.5f);
		pEnemyShot->SetMoveSpeed(ENEMY_SHOT_SPEED);
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

void CGame::InitLevelController()
{
	m_pLevelController->Init();
	m_pLevelController->SetPlayer(m_pPlayer);
	for (int i = 0; i < 3; i++)
	{
		m_pLevelController->SetCurrentLevel(i);
		auto pLevel = m_pLevelController->GetCurrentLevel();
		pLevel->SetStage(m_pStageMeshes[i]);
		pLevel->SetGoal(LevelGoals[i]);

		switch (i)
		{
		default:
			pLevel->SetStartPosition(PLAYER_STARTPOS[CGameStats::LevelSelection]);
			pLevel->SetTriggerAreas(COLLISION_TRIGGER_LIST[CGameStats::LevelSelection]);
			pLevel->SetBlockedPathList(m_pBlockedPathList[CGameStats::LevelSelection]);
			break;
		}

	}

	m_pLevelController->SetCurrentLevel(CGameStats::LevelSelection);
}

#if _DEBUG
void CGame::DrawDebugColliders()
{
	if (!m_pDebugColliderRender)
		return;

	D3DXMATRIX& mView = m_SceneInfo.mView;
	D3DXMATRIX& mProj = m_SceneInfo.mProj;

	// Draw player collider
	if (m_pPlayer && m_pPlayer->GetCollider())
	{
		CCollider* pCollider = m_pPlayer->GetCollider();
		CCollider::COLLIDER_SHAPE shape = pCollider->GetShape();
		m_pDebugColliderRender->DrawCollider(*m_pDx11, mView, mProj, shape, *pCollider);
	}

	// Draw enemy colliders
	for (auto pEnemy : m_pEnemyPool)
	{
		if (!pEnemy || !pEnemy->IsActive() || pEnemy->IsDead())
			continue;

		CCollider* pCollider = pEnemy->GetCollider();
		if (!pCollider)
			continue;

		CCollider::COLLIDER_SHAPE shape = pCollider->GetShape();
		m_pDebugColliderRender->DrawCollider(*m_pDx11, mView, mProj, shape, *pCollider);
	}

	// Draw blocked path colliders
	for (auto pBlockedPath : m_pBlockedPathList[CGameStats::LevelSelection])
	{
		if (!pBlockedPath || !pBlockedPath->IsActive())
			continue;

		CCollider* pCollider = pBlockedPath->GetCollider();
		if (!pCollider)
			continue;

		CCollider::COLLIDER_SHAPE shape = pCollider->GetShape();
		m_pDebugColliderRender->DrawCollider(*m_pDx11, mView, mProj, shape, *pCollider);
	}

	// Draw enemy shot colliders
	for (auto pEnemyShot : m_pEnemyShotList)
	{
		if (!pEnemyShot || !pEnemyShot->IsDisplay())
			continue;

		CCollider* pCollider = pEnemyShot->GetCollider();
		if (!pCollider)
			continue;

		CCollider::COLLIDER_SHAPE shape = pCollider->GetShape();
		m_pDebugColliderRender->DrawCollider(*m_pDx11, mView, mProj, shape, *pCollider);
	}
}
#endif
