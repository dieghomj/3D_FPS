#pragma once
#include <vector>
#include <string>
#include "CLevel.h"		// CLevel::GOAL, CLevel::COLLISION_TRIGGER, D3DXVECTOR3

//==================================================================
// CDataReader
//   Data\CSV\ 配下のCSVファイルからレベルデータを読み込む。
//   各ローダーはヘッダー行をスキップし、空行を無視する。
//   リスト系ローダーはLEVEL_ID列ごとに行をout[level]へ振り分ける。
//==================================================================
class CDataReader
{
public:

	// レベルごとのスカラーデータ（レベル1つにつき1行）
	struct LEVEL_INFO
	{
		int          id       = 0;
		D3DXVECTOR3  startPos = D3DXVECTOR3(0.f, 0.f, 0.f);
		CLevel::GOAL goal     = {};
		float        timer    = 0.f;
	};

	// 通行止めパスのトランスフォーム（通行止め1つにつき1行）
	struct BLOCKED_PATH
	{
		D3DXVECTOR3 position = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 scale    = D3DXVECTOR3(1.f, 1.f, 1.f);
	};

	// 敵のアーキタイプ。CGameが各値を具体的な敵クラスに対応付ける
	enum ENEMY_TYPE
	{
		ENEMY_SPIDER,
		ENEMY_ROBO,
		ENEMY_BOSS,
	};

	// 1つのスポーングループ（まとめて撃破される）。順序付けされた敵種別を保持する
	struct ENEMY_GROUP_DEF
	{
		std::vector<ENEMY_TYPE> enemies;
	};

	// levelData.csv  -> 開始位置・ゴール・タイマー（ファイル順に追加）
	static bool LoadLevelData(const char* filePath, std::vector<LEVEL_INFO>& out);

	// enemySpawns.csv  -> LEVEL_IDごとに振り分けたスポーン位置
	static bool LoadEnemySpawns(const char* filePath, std::vector<D3DXVECTOR3>* out, int levelCount);

	// blockedPaths.csv -> LEVEL_IDごとに振り分けた通行止めパスのトランスフォーム
	static bool LoadBlockedPaths(const char* filePath, std::vector<BLOCKED_PATH>* out, int levelCount);

	// triggers.csv     -> LEVEL_IDごとに振り分けた衝突トリガー
	static bool LoadTriggers(const char* filePath, std::vector<CLevel::COLLISION_TRIGGER>* out, int levelCount);

	// enemyGroups.csv  -> LEVEL_IDごとに振り分けたスポーングループ（GROUP列でインデックス）
	static bool LoadEnemyGroups(const char* filePath, std::vector<ENEMY_GROUP_DEF>* out, int levelCount);

private:

	// 行をフィールドに分割する。strtokと違い空フィールドも保持する
	static std::vector<std::string> SplitLine(const std::string& line, char delim);
};
