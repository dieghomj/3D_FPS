#pragma once

#include "CItem.h"

class CHealthItem :
	public CItem
{
	public:
	CHealthItem();
	virtual ~CHealthItem() override;
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;
	virtual void ActivateEffect() override;
	virtual void DeactivateEffect() override;
	virtual void OnPickup(CPlayer& player) override;
};
