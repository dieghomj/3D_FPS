#pragma once
#include "CStaticMeshObject.h"
#include "CPlayer.h"
#include "CAnimEnemy.h"

class CStage :
    public CStaticMeshObject
{

public:
    CStage();
    virtual ~CStage() override;
    virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;

    void SetPlayer(CPlayer& player) { 
        m_pPlayer = &player; 
    };

	// 敵の衝突処理用メソッドを追加
	void SetEnemyList(std::vector<CAnimEnemy*>& enemyList) {
		m_pEnemyList = &enemyList;
	}

	void UpdateEnemyCollisions();


private:

	void HandleWallCollisions();
	
	void HandleWallCollisions(CROSSRAY* cross);
	
	void HandleFloorCollisions();
	
	void HandleCeilingCollisions();
	
	void HandleStepUp();

	void HandleSweptCollisions();

	void HandleEnemyWallCollisions(CAnimEnemy* pEnemy);
	void HandleEnemyFloorCollisions(CAnimEnemy* pEnemy);
	void HandleEnemyStepUp(CAnimEnemy* pEnemy);
   
private:
	CPlayer* m_pPlayer;
	D3DXVECTOR3 m_prevPlayerPos;
	std::vector<CAnimEnemy*>* m_pEnemyList;

#if _DEBUG
public:

	std::vector<D3DXVECTOR3> debugPlayerPath;
	RAY						 debugSweptRay;
	bool					 debugSweptHit = false;

#endif // 0

};

