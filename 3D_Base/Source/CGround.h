#pragma once

#include "CStaticMeshObject.h"
#include "CCharacter.h"

/*********************************************************
*	地面クラス
**/
class CGround
	: public CStaticMeshObject
{
public:
	CGround();
	virtual ~CGround();

	virtual void Update() override;

	//プレイヤーを設定
	void SetPlayer(CCharacter& pPlayer) { m_pPlayer = &pPlayer; }

protected:
	CCharacter* m_pPlayer;
};