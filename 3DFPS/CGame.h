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


class CGame :
    public CScene
{
public:

	struct GOAL
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 size;
		bool isReached;
	};

	struct COLLISION_TRIGGER
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 size;
		std::vector<int> blockedPathIndices;
		bool isTriggered;
		bool blockBehindPlayer;
	};

	struct BULLET_IMPACT {
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
		bool isEnemyHit;
		float lifeTime;
	};


	CGame(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& m_pManager);
	virtual ~CGame();

	virtual void Create() override;
	virtual HRESULT LoadData() override;
	virtual void Release() override;
	virtual void Start() override;
	virtual void Draw() override;
	virtual void Update() override;


	void Restart();


private:

	int NextBullet();
	int NextEnemyShot();
	void HandleWeaponPos();
	void HandleWeapon();
	void IsShotHit(RAY& shotRay, float& hitDist, D3DXVECTOR3& hitPos, D3DXVECTOR3& normal, CGame::BULLET_IMPACT& impact);

	bool HandleCollision(CCharacter* objA, CCharacter* objB, float& distance, bool doubleCollision = true);
	void HandlePlayerEnemyCollision();
	void HandleEnemyEnemyCollision();
	void HandleEnemySpawning();

	void HandleEnemyShooting();

	//void HandleWallCollisions(CCharacter* character);

	void SetupTriggers();
	void CheckTriggers();
	bool IsPlayerInTriggerArea(const COLLISION_TRIGGER& trigger);

	void SetupGoal();
	void CheckGoal();
	bool IsPlayerInTriggerArea(const GOAL& trigger);

	void SetupBlockedPath();
	void DrawEnemyShots();
	void HandleEnemyShotLoadAnim(CRobo* pEnemy);
	void HandlePlayerDashEffect();

	//void HandlePlayerBulletCollision();
	//void HandleEnemyBulletCollision();
	//void HandlePlayerItemCollision();
	//void HandleEnemyAI();
	//void HandleEnemyItemDrop(CAnimEnemy* enemy);
	//void HandleBulletImpacts(float deltaTime);
	//void HandlePlayerDeath();
	//void HandleStageProgression();

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
		CSprite2D* m_pPainSprite;
		CUIObject* m_pPainUI;
	
	
	//--------------------
	//---SCENE OBJECTS---
	//--------------------
	//GROUND
		CStaticMesh* m_pGroundMesh;
		CStaticMeshObject* m_pGround;
		
	//STAGE
		CStaticMesh* m_pBaseStageMesh;
		CStaticMesh* m_pBridStageMesh;
		CStage* m_pStage;
	//WALL COLLIDER
		CStaticMesh* m_pWallColliderMesh;
	//GOAL
		GOAL m_Goal;
	//LIGHTNING
		CSprite3D* m_pLightningSprite;
		CBlockedPath* m_pLightning;
		std::vector<CBlockedPath*> m_pBlockedPathList;
	//TRIGGERS
		std::vector<COLLISION_TRIGGER> m_CollisionTriggerList;

	//TIMER
		float m_stageTimer = 0.0f;
		float m_accumulatedTime = 0.0f;
	//----------------
	//-----ENEMY------
	//----------------
	CStaticMesh* m_pEnemyMesh;
	CBoss* m_pEnemy;
	
	CStaticMesh* m_pSpiderMesh;
	CStaticMesh* m_pRoboMesh;
	CStaticMesh* m_pBossMesh;

	CSkinMesh* m_pSpiderSkinMesh;
	CSkinMesh* m_pRoboSkinMesh;
	CSkinMesh* m_pBossSkinMesh;
	CAnimEnemy* m_pBossEnemy;
	std::vector<CAnimEnemy*> m_pBossShotList;
	std::vector<CAnimEnemy*> m_pEnemyList;
	std::vector<CShot*> m_pEnemyShotList;
	int m_enemyShotIndex = 0;
	int m_bossShotIndex = 0;


	//----------------
	//-----PLAYER-----
	//----------------
	CPlayer* m_pPlayer;
	CStaticMesh* m_pPistolMesh;
	CStaticMesh* m_pShotgunMesh;

	CStaticMeshObject* m_pPlayerWeapon;

	int m_bulletIndex = 0;

	::EsHandle m_shotHandle = -1;
	CStaticMesh* m_pBulletMesh;
	std::vector<CShot*> m_pBulletList;
	std::vector<BULLET_IMPACT> m_bulletImpactList;
	CDecal* m_pShotDecalSprite;
	CDecal* m_pEnemyHitDecalSprite;

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

	int currStage = 0;
	CStaticMesh* m_pSphereMesh;

#if _DEBUG
	//DEBUG
	CRay* debugRay;
	CStaticMesh* debugShotMesh;
	std::vector<D3DXVECTOR3> debugShotMark;
	CROSSRAY m_prevCrossRay;
	CRay* m_pCrossRay[4];
	CRay* m_pPrevCrossRay[4];
	CRay* m_pPlayerRayY;
	CRay* debugShotRay;
	std::vector<D3DXVECTOR3> debugHitShotList;
#endif // _DEBUG
};

