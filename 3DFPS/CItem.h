#pragma once
#include "CStaticMeshObject.h"
#include "CPlayer.h"

class CItem :
    public CStaticMeshObject
{
public:

    CItem();
	~CItem();

	virtual void Update() = 0;
	virtual void Draw(SCENE_DATA& sceneData);
	
	virtual void ActivateEffect() = 0;
	virtual void DeactivateEffect() = 0;

	virtual void OnPickup(CPlayer& player) = 0;

private:



};

