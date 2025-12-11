#pragma once
#include "CScene.h"
#include "CUIObject.h"
#include "CAnimCharacter.h"
#include "CPlayer.h"
#include "CStage.h"
#include "CFont.h"

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
	//
	
	
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
	CRay* m_pCrossRay[4];
	CRay* m_pPrevCrossRay[4];
	CROSSRAY m_prevCrossRay;
};

