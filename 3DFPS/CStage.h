#pragma once
#include "CStaticMeshObject.h"
#include "CPlayer.h"
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

#if _DEBUG
public:

	std::vector<D3DXVECTOR3> debugPlayerPath;
	RAY						 debugSweptRay;
	bool					 debugSweptHit = false;

#endif // 0
private:

	void HandleWallCollisions();
	void HandleFloorCollisions();
	void HandleCeilingCollisions();
	void HandleStepUp();
	void HandleSweptCollisions();
   
private:
	CPlayer* m_pPlayer;
	D3DXVECTOR3 m_prevPlayerPos;
};

