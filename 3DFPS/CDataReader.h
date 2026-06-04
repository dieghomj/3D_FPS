#pragma once
#include <vector>
#include <string>
#include "CLevel.h"		// CLevel::GOAL, CLevel::COLLISION_TRIGGER, D3DXVECTOR3

//==================================================================
// CDataReader
//   Loads level data from CSV files under Data\CSV\.
//   Each loader skips the header row and ignores blank lines.
//   List loaders bucket rows by their LEVEL_ID column into out[level].
//==================================================================
class CDataReader
{
public:

	// Per-level scalar data (one row per level).
	struct LEVEL_INFO
	{
		int          id       = 0;
		D3DXVECTOR3  startPos = D3DXVECTOR3(0.f, 0.f, 0.f);
		CLevel::GOAL goal     = {};
		float        timer    = 0.f;
	};

	// Blocked-path transform (one row per blocked path).
	struct BLOCKED_PATH
	{
		D3DXVECTOR3 position = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 scale    = D3DXVECTOR3(1.f, 1.f, 1.f);
	};

	// Enemy archetype. CGame maps each value to a concrete enemy class.
	enum ENEMY_TYPE
	{
		ENEMY_SPIDER,
		ENEMY_ROBO,
		ENEMY_BOSS,
	};

	// One spawn group (cleared together). Holds the ordered enemy types.
	struct ENEMY_GROUP_DEF
	{
		std::vector<ENEMY_TYPE> enemies;
	};

	// levelData.csv  -> start position, goal, timer (appended in file order).
	static bool LoadLevelData(const char* filePath, std::vector<LEVEL_INFO>& out);

	// enemySpawns.csv  -> spawn positions bucketed by LEVEL_ID.
	static bool LoadEnemySpawns(const char* filePath, std::vector<D3DXVECTOR3>* out, int levelCount);

	// blockedPaths.csv -> blocked-path transforms bucketed by LEVEL_ID.
	static bool LoadBlockedPaths(const char* filePath, std::vector<BLOCKED_PATH>* out, int levelCount);

	// triggers.csv     -> collision triggers bucketed by LEVEL_ID.
	static bool LoadTriggers(const char* filePath, std::vector<CLevel::COLLISION_TRIGGER>* out, int levelCount);

	// enemyGroups.csv  -> spawn groups bucketed by LEVEL_ID (indexed by GROUP column).
	static bool LoadEnemyGroups(const char* filePath, std::vector<ENEMY_GROUP_DEF>* out, int levelCount);

private:

	// Split a line into fields, preserving empty fields (unlike strtok).
	static std::vector<std::string> SplitLine(const std::string& line, char delim);
};
