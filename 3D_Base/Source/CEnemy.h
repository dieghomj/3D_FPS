#pragma once

#include "CCharacter.h"

/**************************************************
*	エネミークラス
**/
class CEnemy
	: public CCharacter
{
public:
	CEnemy();
	virtual ~CEnemy();

	virtual void Update() override;

protected:

};
