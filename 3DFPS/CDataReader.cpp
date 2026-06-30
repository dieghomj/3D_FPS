#include "stdafx.h"
#include "CDataReader.h"
#include <fstream>
#include <sstream>

std::vector<std::string> CDataReader::SplitLine(const std::string& line, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(line);
	std::string token;
	while (std::getline(ss, token, delim))
	{
		// Windowsの改行コードが残す末尾のキャリッジリターンを除去する
		if (!token.empty() && token.back() == '\r')
			token.pop_back();
		tokens.push_back(token);
	}
	return tokens;
}

bool CDataReader::LoadLevelData(const char* filePath, std::vector<LEVEL_INFO>& out)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		_ASSERT_EXPR(false, _T("Failed to open levelData.csv"));
		return false;
	}

	std::string line;
	std::getline(file, line); // ヘッダー行

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		std::vector<std::string> c = SplitLine(line, ',');
		if (c.size() < 11)
			continue;

		LEVEL_INFO info;
		info.id             = std::stoi(c[0]);
		info.startPos       = D3DXVECTOR3(std::stof(c[1]), std::stof(c[2]), std::stof(c[3]));
		info.goal.position  = D3DXVECTOR3(std::stof(c[4]), std::stof(c[5]), std::stof(c[6]));
		info.goal.size      = D3DXVECTOR3(std::stof(c[7]), std::stof(c[8]), std::stof(c[9]));
		info.goal.isReached = false;
		info.timer          = std::stof(c[10]);
		out.push_back(info);
	}

	return true;
}

bool CDataReader::LoadEnemySpawns(const char* filePath, std::vector<D3DXVECTOR3>* out, int levelCount)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		_ASSERT_EXPR(false, _T("Failed to open enemySpawns.csv"));
		return false;
	}

	std::string line;
	std::getline(file, line); // ヘッダー行

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		std::vector<std::string> c = SplitLine(line, ',');
		if (c.size() < 4)
			continue;

		int level = std::stoi(c[0]);
		if (level < 0 || level >= levelCount)
			continue;

		out[level].push_back(D3DXVECTOR3(std::stof(c[1]), std::stof(c[2]), std::stof(c[3])));
	}

	return true;
}

bool CDataReader::LoadBlockedPaths(const char* filePath, std::vector<BLOCKED_PATH>* out, int levelCount)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		_ASSERT_EXPR(false, _T("Failed to open blockedPaths.csv"));
		return false;
	}

	std::string line;
	std::getline(file, line); // ヘッダー行

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		std::vector<std::string> c = SplitLine(line, ',');
		if (c.size() < 7)
			continue;

		int level = std::stoi(c[0]);
		if (level < 0 || level >= levelCount)
			continue;

		BLOCKED_PATH path;
		path.position = D3DXVECTOR3(std::stof(c[1]), std::stof(c[2]), std::stof(c[3]));
		path.scale    = D3DXVECTOR3(std::stof(c[4]), std::stof(c[5]), std::stof(c[6]));
		out[level].push_back(path);
	}

	return true;
}

bool CDataReader::LoadTriggers(const char* filePath, std::vector<CLevel::COLLISION_TRIGGER>* out, int levelCount)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		_ASSERT_EXPR(false, _T("Failed to open triggers.csv"));
		return false;
	}

	std::string line;
	std::getline(file, line); // ヘッダー行

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		std::vector<std::string> c = SplitLine(line, ',');
		if (c.size() < 9)
			continue;

		int level = std::stoi(c[0]);
		if (level < 0 || level >= levelCount)
			continue;

		CLevel::COLLISION_TRIGGER trigger;
		trigger.position = D3DXVECTOR3(std::stof(c[1]), std::stof(c[2]), std::stof(c[3]));
		trigger.size     = D3DXVECTOR3(std::stof(c[4]), std::stof(c[5]), std::stof(c[6]));

		// 通行止めパスのインデックス：1つのフィールド内に';'区切りで列挙（空の場合あり）
		for (const std::string& idx : SplitLine(c[7], ';'))
		{
			if (!idx.empty())
				trigger.blockedPathIndices.push_back(std::stoi(idx));
		}

		trigger.blockBehindPlayer = (std::stoi(c[8]) != 0);
		trigger.isTriggered       = false;
		out[level].push_back(trigger);
	}

	return true;
}

bool CDataReader::LoadEnemyGroups(const char* filePath, std::vector<ENEMY_GROUP_DEF>* out, int levelCount)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		_ASSERT_EXPR(false, _T("Failed to open enemyGroups.csv"));
		return false;
	}

	std::string line;
	std::getline(file, line); // ヘッダー行

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		std::vector<std::string> c = SplitLine(line, ',');
		if (c.size() < 4)
			continue;

		int level = std::stoi(c[0]);
		if (level < 0 || level >= levelCount)
			continue;

		int group = std::stoi(c[1]);
		if (group < 0)
			continue;

		// 種別名をアーキタイプに対応付ける。未知の名前はスキップする
		ENEMY_TYPE type;
		if      (c[2] == "SPIDER") type = ENEMY_SPIDER;
		else if (c[2] == "ROBO")   type = ENEMY_ROBO;
		else if (c[2] == "BOSS")   type = ENEMY_BOSS;
		else                       continue;

		int count = std::stoi(c[3]);

		// out[level][group]が存在するようにバケットを拡張してから敵を追加する
		if (group >= static_cast<int>(out[level].size()))
			out[level].resize(group + 1);

		for (int i = 0; i < count; ++i)
			out[level][group].enemies.push_back(type);
	}

	return true;
}
