#pragma once
#include "CStaticMeshObject.h"
#include "CSprite3D.h"
#include "CPlayer.h"

class CBlockedPath :
    public CStaticMeshObject
{

public:
	CBlockedPath();
	virtual ~CBlockedPath() override;
	void AttachSprite(CSprite3D& pSprite) {
		m_pBlockedPathSprite = &pSprite;
	}
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;
	void HandleCubeCollisions(CPlayer* character);

private:

	CSprite3D* m_pBlockedPathSprite;

};

