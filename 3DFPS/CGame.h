#pragma once
#include <random>
#include "CScene.h"
#include "CUIObject.h"
#include "CAnimEnemy.h"
#include "CPlayer.h"
#include "CStage.h"
#include "CFont.h"
#include "CItem.h"
#include "CHealthItem.h"
#include "CEffect.h"
#include "CShot.h"
#include "CDecal.h"
#include "CRobo.h"
#include "CBoss.h"
#include "CBlockedPath.h"	
#include "CSkybox.h"
#include "CLevelController.h"
#include "CLevel.h"
#include "CDebugColliderRender.h"


class CGame :
    public CScene
{
public:

	struct BULLET_IMPACT {

		D3DXVECTOR3 position = D3DXVECTOR3(0.f,0.f,0.f);
		D3DXVECTOR3 normal = D3DXVECTOR3(0.f, 1.f, 0.f);
		bool isEnemyHit = false;
		float lifeTime = 1.f;

	};

	struct ENEMY_GROUP {
		int count = 0;
		bool isSpawned = false;
		bool isCleared = false;
		std::vector<CAnimEnemy*> enemies;
	};

	CGame(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& m_pManager);
	virtual ~CGame();

	virtual void Create() override;
	virtual HRESULT LoadData() override;
	virtual void Release() override;
	virtual void Start() override;
	void InitLevelController();
	virtual void Draw() override;
	void DrawUI();
	void DrawDecals(D3DXMATRIX& mView, D3DXMATRIX& mProj);
	virtual void Update() override;


	void GameClear();

	void HandleGameOver();
	bool CheckRestartStatus();
	void Restart();


private:

	HRESULT LoadPlayerAsset();
	HRESULT LoadEnemiesMesh();
	HRESULT LoadStageMesh();
	HRESULT LoadUtilityMesh();
	HRESULT LoadSpriteAssets();
	HRESULT LoadUIAssets();
	HRESULT LoadSceneAssets();
	void InitPlayer();
	void InitEnemy();
	void InitStage();
	void InitSpriteAssets();
	void InitUI();
	void InitScene(float fovY, float aspect, float zn, float zf, float lightIntensity, const D3DXVECTOR3& lightDir, bool fog);

	int NextBullet();
	int NextEnemyShot();
	void HandleWeaponPos();
	void HandleWeapon();
	void IsShotHit(RAY& shotRay, float& hitDist, D3DXVECTOR3& hitPos, D3DXVECTOR3& normal, CGame::BULLET_IMPACT& impact);

	void HandleWallCollisions(CStaticMeshObject* pMeshObj, CCharacter* pChara);
	bool HandleCubeCollisions(CStaticMeshObject* pMeshObj, CCharacter* pChara);
	bool HandleCollision(CCharacter* objA, CCharacter* objB, float& distance, bool doubleCollision = true);
	bool HandleColliderCollision(CStaticMeshObject* pObjA, CCharacter* pChara, bool resolveForChara = true);
	void HandlePlayerEnemyCollision();
	void HandleEnemyEnemyCollision();
	void HandleEnemySpawning();
	void HandleEnemyShooting();
	void HandleEnemyGroupCleared();
	void HandleDynamicFov(float playerVelLen, float& fovY);
	void HandleStepSE(float playerVelLen, float& stepTimer);


	//void HandleWallCollisions(CCharacter* character);
	void LoadLevelDataFromCSV();
	void SetupBlockedPath();

	void DrawEnemyShots();
	void HandleEnemyShotLoadAnim(CRobo* pEnemy);
	void HandlePlayerDashEffect();

	void SaveStats();



private:

	std::mt19937 m_RandomGen;
	std::uniform_real_distribution<float> m_AngleDist;
	std::uniform_real_distribution<float> distFloat;

	//STATS

	CGameStats m_GameStats;
	int m_enemyKillCount = 0;
	int m_deathCount = 0;
	int m_comboCount = 0;
	int m_highestCombo = 0;

	//----------------
	//----UI----------
	//----------------
	//FONT
		CFont* m_pFont;
	//STAMINAR BAR
		CSprite2D* m_pStaminaBarSprite;
		CUIObject* m_pStaminaBarUI;
	//CROSSHAIR
		CSprite2D* m_pCrossHairSprite;
		CUIObject* m_pCrossHairUI;
	//HEALTH BAR
		CSprite2D* m_pHealthBarSprite;
		CUIObject* m_pHealthBarUI;
	//HURT EFFECT
		CSprite2D* m_pPainSprite;
		CUIObject* m_pPainUI;
	//DASH EFFECT
		CSprite2D* m_pDashEffectSprite;
		CUIObject* m_pDashEffectUI;

	//--------------------
	//----LEVEL---
	//--------------------
		CLevelController* m_pLevelController;

	//--------------------
	//---SCENE OBJECTS---
	//--------------------
	//SKYBOX
		CSkybox* m_pSkybox;
	//GROUND
		CStaticMesh* m_pGroundMesh;
		CStaticMeshObject* m_pGround;
		
	//STAGE
		CStaticMesh* m_pTestStageMesh;
		CStaticMesh* m_pStageMesh;
		std::vector<CStaticMesh*> m_pStageMeshes;
		CStage* m_pStage;
	//WALL COLLIDER
		CStaticMesh* m_pCubeMesh;
	//GOAL
		CLevel::GOAL m_Goal;
	//BLOCKED PATH
		//Sprite
		CSprite3D* m_pLightningSprite;
		CBlockedPath* m_pLightning;
		//List
		std::vector<CBlockedPath*> m_pBlockedPathList[LEVEL_COUNT];
	//TRIGGERS
		std::vector<CLevel::COLLISION_TRIGGER> m_CollisionTriggerList;

	//TIMER
		float m_stageTimer = 0.0f;
		float m_accumulatedTime = 0.0f;
	//----------------
	//-----ENEMY------
	//----------------

		std::vector<ENEMY_GROUP> m_EnemyGroups[LEVEL_COUNT];

		CStaticMesh* m_pEnemyMesh;
		CBoss* m_pBoss;
	
		CStaticMesh* m_pSpiderMesh;
		CStaticMesh* m_pRoboMesh;
		CStaticMesh* m_pBossMesh;

		CStaticMesh* m_pEnemyShotMesh;

		CSkinMesh* m_pSpiderSkinMesh;
		CSkinMesh* m_pRoboSkinMesh;
		CSkinMesh* m_pBossSkinMesh;
		CAnimEnemy* m_pBossEnemy;
		std::vector<CAnimEnemy*> m_pBossShotList;
		std::vector<CAnimEnemy*> m_pEnemyPool;
		std::vector<CShot*> m_pEnemyShotList;
		int m_enemyShotIndex = 0;
		int m_bossShotIndex = 0;


	//----------------
	//-----PLAYER-----
	//----------------
		CPlayer* m_pPlayer;
		//WEAPON
		CStaticMesh* m_pPistolMesh;
		CStaticMesh* m_pShotgunMesh;
		CStaticMeshObject* m_pPlayerWeapon;

		//SHOT
		int m_bulletIndex = 0;
		::EsHandle m_shotHandle = -1;
		CStaticMesh* m_pBulletMesh;
		CSprite3D* m_pBulletLaserSprite;
		CSpriteObject* m_pBulletLaser;
		std::vector<CShot*> m_pBulletList;
		std::vector<BULLET_IMPACT> m_bulletImpactList;
		CDecal* m_pShotDecalSprite;
		CDecal* m_pEnemyHitDecalSprite;

	//----------------
	//EFFECTS HANDLERS
	//----------------
		::EsHandle explosionHandle = -1;
		::EsHandle dashHandle = -1;
		::EsHandle enemyHitHandle = -1;
		::EsHandle enemyDeathHandle = -1;
		::EsHandle enemyShotLoadHandle = -1;
		std::vector<::EsHandle> enemyShotEffectHandles;
		::EsHandle itemPickupHandle = -1;
		::EsHandle healthPickupHandle = -1;
		::EsHandle ammoPickupHandle = -1;
		::EsHandle playerDeathHandle = -1;
		::EsHandle bossDeathHandle = -1;
	
	//----------------
	//---ITEMS--------
	//----------------	

		CStaticMesh* m_pItemMesh;
		std::vector<CItem*> m_pItemList;

		CHealthItem* m_pHealthItem;

		CStaticMesh* m_pAmmoItemMesh;
		CStaticMesh* m_pHealthItemMesh;
		CStaticMesh* m_pDashItemMesh;

	//----------------
	//---UTILITY------
	//----------------
		int currStage = 0;
		CStaticMesh* m_pSphereMesh;

#if _DEBUG
	void DrawDebugColliders();
	CRay* debugRay;
	CStaticMesh* debugShotMesh;
	std::vector<D3DXVECTOR3> debugShotMark;
	CROSSRAY m_prevCrossRay;
	CRay* m_pCrossRay[4];
	CRay* m_pPrevCrossRay[4];
	CRay* m_pPlayerRayY;
	CRay* debugShotRay;
	std::vector<D3DXVECTOR3> debugHitShotList;
	CDebugColliderRender* m_pDebugColliderRender;
#endif // _DEBUG
};

