#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

class CDataReader
{


public :

	struct LevelData
	{
		int levelID = 0;
		D3DXVECTOR3 playerStartPos = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 goalPos = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 goalScale = D3DXVECTOR3(1.f, 1.f, 1.f);
		float timer = 180.f;
	};

	struct TriggerData
	{
		int levelID = 0;
		int triggerID = 0;
		D3DXVECTOR3 position = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 scale = D3DXVECTOR3(1.f, 1.f, 1.f);
		std::vector<int> blockedPathIndices;
	};

	struct BlockedPathData
	{
		int levelID = 0;
		int pathID = 0;
		D3DXVECTOR3 position = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 scale = D3DXVECTOR3(1.f, 1.f, 1.f);
	};

	struct EnemySpawnData
	{
		int levelID = 0;
		int groupID = 0;
		int spawnID = 0;
		D3DXVECTOR3 position = D3DXVECTOR3(0.f, 0.f, 0.f);
		int enemyType;
	};

	struct EnemyGroupData
	{
		int levelID = 0;
		int groupID = 0;
		int enemyCount = 0;
	};

	struct SettingsData
	{
		float mouseSensitivity = 1.f;
		int volume = 500;
	};	

	bool LoadData(const std::wstring& dataDirectory);

	//bool LoadPlayerData(const std::wstring& filename);

	const LevelData* GetLevelData(int levelID) const;
	const std::vector<LevelData>& GetAllLevels() const { return m_LevelData; }
	int GetLevelCount() const { return static_cast<int>( m_LevelData.size()); }

	std::vector<TriggerData> GetTriggersForLevel(int levelID) const;
	std::vector<BlockedPathData> GetBlockedPathsForLevel(int levelID) const;
	std::vector<EnemySpawnData> GetEnemySpawnsForLevel(int levelID) const;
	std::vector<EnemySpawnData> GetEnemySpawnsForGroup(int levelID, int groupID) const;
	std::vector<EnemyGroupData> GetEnemyGroupsForLevel(int levelID) const;

	SettingsData GetSettingsData() const { return m_SettingsData; }
	void SetSettingsData(const SettingsData& settings);

private:

	bool LoadLevelData(const std::wstring& filename);
	bool LoadTriggerData(const std::wstring& filename);
	bool LoadBlockedPathData(const std::wstring& filename);
	bool LoadEnemySpawnData(const std::wstring& filename);
	bool LoadEnemyGroupData(const std::wstring& filename);
	bool LoadSettingsData(const std::wstring& filename);
	void Clear();
	std::vector<std::string> ParseCSVLine(const std::string& line);
	std::vector<int> ParseIntList(const std::string& str, char delimiter = ';');
	float ParseFloat(const std::string& str);
	int ParseInt(const std::string& str);
	std::string Trim(const std::string& str);

private:
	std::vector<LevelData>			m_LevelData;
	std::vector<TriggerData>		m_TriggersData;
	std::vector<BlockedPathData>	m_BPathData;
	std::vector<EnemySpawnData>		m_EnemyData;
	std::vector<EnemyGroupData>		m_EnemyGroupsData;
	SettingsData					m_SettingsData;

};

