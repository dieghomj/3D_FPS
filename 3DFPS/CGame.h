#pragma once
#include "CScene.h"
#include "CUIObject.h"
#include "CAnimCharacter.h"
#include "CPlayer.h"
#include "CStage.h"
#include "CFont.h"
#include "CItem.h"
#include "CHealthItem.h"
#include "CEffect.h"


class CGame :
    public CScene
{
public:
	CGame(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& m_pManager);
	virtual ~CGame();

	virtual void Create() override;
	virtual HRESULT LoadData() override;
	virtual void Release() override;
	virtual void Start() override;
	virtual void Draw() override;
	virtual void Update() override;

	void HandleWeaponPosition();

private:

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


	//----------------
	//-----ENEMY------
	//----------------
	CStaticMesh* m_pEnemyMesh;
	CAnimCharacter* m_pEnemy;

	//----------------
	//-----PLAYER-----
	//----------------
	CPlayer* m_pPlayer;
	CStaticMesh* m_pPistolMesh;
	CStaticMesh* m_pShotgunMesh;
	CStaticMeshObject* m_pPlayerWeapon;

	::EsHandle dashHandle = -1;
	
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

	//DEBUG
	
	CRay* debugRay;
	CStaticMesh* debugPathMesh;
	CROSSRAY m_prevCrossRay;
	CRay* m_pCrossRay[4];
	CRay* m_pPrevCrossRay[4];
	CRay* m_pPlayerRayY;

};

