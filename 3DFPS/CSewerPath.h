#pragma once
#include"CStaticMeshObject.h"
#include "CCharacter.h"

class CSewerPath
	: public CStaticMeshObject
{
public:

	enum PathType {
		LINE = 0,
		TURN,
		T_JUNCTION,
		CROSS,
		END
	};

public:
	CSewerPath();
	virtual ~CSewerPath() override;
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;

	void SetPlayer(CCharacter& player) { m_pPlayer = &player; }

	void SetType(int type) { m_Type = type; }
	int GetType() const { return m_Type; }
	Pair GetMazeCoords() const { return m_MazeCoords; }
	void SetMazeCoords(int x, int y) { m_MazeCoords.x = x; m_MazeCoords.y = y; }

private:
	int m_Type; // 0: Line, 1: Turn, 2: T-Junction, 3: Cross, 4: End
	Pair m_MazeCoords;
	CCharacter* m_pPlayer;
};