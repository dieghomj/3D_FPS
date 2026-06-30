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
	void SavePrevPlayerPos();
	virtual void Draw(SCENE_DATA& sceneData) override;

    void SetPlayer(CPlayer& player) { 
        m_pPlayer = &player; 
    };

	// 敵の衝突処理用メソッドを追加
	void SetEnemyList(std::vector<CAnimEnemy*> enemyList) {
		m_pEnemyPool = enemyList;
	}

	void UpdateEnemyCollisions();

	void RestartPlayerPosition(D3DXVECTOR3 playerPos) {
		
		m_prevPlayerPos = playerPos;

	}


protected:


	void HandleWallCollisions();
	
	void HandleWallCollisions(CROSSRAY* cross);
	
	void HandleFloorCollisions();
	
	void HandleCeilingCollisions();
	
	void HandleStepUp();

	void HandleSweptCollisions(const D3DXVECTOR3& frameStartPos);

	void HandleEnemyWallCollisions(CAnimEnemy* pEnemy);
	void HandleEnemyFloorCollisions(CAnimEnemy* pEnemy);
	void HandleEnemyStepUp(CAnimEnemy* pEnemy);
	D3DXVECTOR3 CalculateWallPushback(CROSSRAY* pCrossRay);
   
protected:

	CPlayer* m_pPlayer;
	D3DXVECTOR3 m_prevPlayerPos;
private:
	std::vector<CAnimEnemy*> m_pEnemyPool;

#if _DEBUG
public:

	std::vector<D3DXVECTOR3> debugPlayerPath;
	RAY						 debugSweptRay;
	bool					 debugSweptHit = false;

#endif // 0

};

