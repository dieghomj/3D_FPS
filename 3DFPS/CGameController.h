#pragma once
#include "CSceneManager.h"

class CPlayerController;
class CStageController;
class CEnemyController;
class CItemController;
class CWeaponController;
class CCollisionController;

class CGameController
{

	public:
	CGameController();
	~CGameController();
	void Create(CSceneManager& sceneManager);
	void Init();
	HRESULT LoadData();
	void Update();
	void Draw();

};

