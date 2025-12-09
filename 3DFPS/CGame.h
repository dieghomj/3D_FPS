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

	CFont* m_pFont;

	CStaticMesh* m_pGroundMesh;
	CStaticMeshObject* m_pGround;

	CStaticMesh* m_pBaseStageMesh;
	CStaticMesh* m_pBridStageMesh;
	CStage* m_pStage;

	CStaticMesh* m_pEnemyMesh;
	CAnimCharacter* m_pEnemy;
	CPlayer* m_pPlayer;

	CRay* m_pCrossRay[4];
	CRay* m_pPrevCrossRay[4];
	CROSSRAY m_prevCrossRay;
};

