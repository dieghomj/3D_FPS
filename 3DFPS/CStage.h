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
   
private:
	CPlayer* m_pPlayer;
	CROSSRAY m_prevCrossRay;
	D3DXVECTOR3 m_prevPlayerPos;
};

