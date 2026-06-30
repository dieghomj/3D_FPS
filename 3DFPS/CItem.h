#pragma once
#include "CStaticMeshObject.h"
#include "CPlayer.h"

class CItem :
    public CStaticMeshObject
{
public:

    CItem();
	virtual ~CItem() override;

	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData);
	
	virtual void ActivateEffect() = 0;
	virtual void DeactivateEffect() = 0;

	virtual void OnPickup(CPlayer& player) = 0;

private:

	float m_UseTimer;
	float m_UseTimerMax;

};

