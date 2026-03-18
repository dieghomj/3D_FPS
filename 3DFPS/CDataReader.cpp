#include "stdafx.h"
#include "CDataReader.h"

constexpr static int MAX_LINE_LENGTH = 256;

bool CDataReader::LoadData(const std::wstring& dataDirectory)
{
	Clear();

	bool success = true;

	success &= LoadLevelData(dataDirectory + L"\\levelData.csv");
	success &= LoadTriggerData(dataDirectory + L"\\triggerData.csv");
	success &= LoadBlockedPathData(dataDirectory + L"\\bPathData.csv");
	success &= LoadEnemySpawnData(dataDirectory + L"\\enemyData.csv");
	success &= LoadEnemyGroupData(dataDirectory + L"\\enemyGroupData.csv");
	success &= LoadSettingsData(dataDirectory + L"\\settingsData.csv");

	return success;
}

bool CDataReader::LoadLevelData(const std::wstring& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		return false;
	}

	std::string line;
	std::getline(file, line);

	while (std::getline(file, line))
	{
		if (line.empty()) continue;

		auto tokens = ParseCSVLine(line);
		if (tokens.size() < 11) continue;

		LevelData data;
		data.levelID = ParseInt(tokens[0]);
		data.playerStartPos.x = ParseFloat(tokens[1]);
		data.playerStartPos.y = ParseFloat(tokens[2]);
		data.playerStartPos.z = ParseFloat(tokens[3]);
		data.goalPos.x = ParseFloat(tokens[4]);
		data.goalPos.y = ParseFloat(tokens[5]);
		data.goalPos.z = ParseFloat(tokens[6]);
		data.goalScale.x = ParseFloat(tokens[7]);
		data.goalScale.y = ParseFloat(tokens[8]);
		data.goalScale.z = ParseFloat(tokens[9]);
		data.timer = ParseFloat(tokens[10]);

		m_LevelData.push_back(data);
	}

	file.close();
	return true;
}

bool CDataReader::LoadTriggerData(const std::wstring& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		return false;
	}

	std::string line;
	std::getline(file, line); // Skip header

	while (std::getline(file, line))
	{
		if (line.empty()) continue;

		auto tokens = ParseCSVLine(line);
		if (tokens.size() < 9) continue;

		TriggerData data;
		data.levelID = ParseInt(tokens[0]);
		data.triggerID = ParseInt(tokens[1]);
		data.position.x = ParseFloat(tokens[2]);
		data.position.y = ParseFloat(tokens[3]);
		data.position.z = ParseFloat(tokens[4]);
		data.scale.x = ParseFloat(tokens[5]);
		data.scale.y = ParseFloat(tokens[6]);
		data.scale.z = ParseFloat(tokens[7]);
		data.blockedPathIndices = ParseIntList(tokens[8]);

		m_TriggersData.push_back(data);
	}

	file.close();
	return true;
}

bool CDataReader::LoadBlockedPathData(const std::wstring& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	std::string line;
	std::getline(file, line); // Skip header

	while (std::getline(file, line))
	{
		if (line.empty()) continue;
		
		auto tokens = ParseCSVLine(line);
		if (tokens.size() < 8) continue;

		BlockedPathData data;
		data.levelID = ParseInt(tokens[0]);
		data.pathID = ParseInt(tokens[1]);
		data.position.x = ParseFloat(tokens[2]);
		data.position.y = ParseFloat(tokens[3]);
		data.position.z = ParseFloat(tokens[4]);
		data.scale.x = ParseFloat(tokens[5]);
		data.scale.y = ParseFloat(tokens[6]);
		data.scale.z = ParseFloat(tokens[7]);
		m_BPathData.push_back(data);
	}
	file.close();
	return true;
}

bool CDataReader::LoadEnemySpawnData(const std::wstring& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	std::string line;
	std::getline(file, line); // Skip header
	while (std::getline(file, line))
	{
		if (line.empty()) continue;
		auto tokens = ParseCSVLine(line);
		if (tokens.size() < 8) continue;
		EnemySpawnData data;
		data.levelID = ParseInt(tokens[0]);
		data.groupID = ParseInt(tokens[1]);
		data.spawnID = ParseInt(tokens[2]);
		data.position.x = ParseFloat(tokens[3]);
		data.position.y = ParseFloat(tokens[4]);
		data.position.z = ParseFloat(tokens[5]);
		data.enemyType = ParseInt(tokens[6]);
		m_EnemyData.push_back(data);
	}
	file.close();
	return true;
}

bool CDataReader::LoadEnemyGroupData(const std::wstring& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	std::string line;
	std::getline(file, line); // Skip header
	while (std::getline(file, line))
	{
		if (line.empty()) continue;
		auto tokens = ParseCSVLine(line);
		if (tokens.size() < 3) continue;
		EnemyGroupData data;
		data.levelID = ParseInt(tokens[0]);
		data.groupID = ParseInt(tokens[1]);
		data.enemyCount = ParseInt(tokens[2]);
		m_EnemyGroupsData.push_back(data);
	}
	file.close();
	return true;
}

bool CDataReader::LoadSettingsData(const std::wstring& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	std::string line;
	std::getline(file, line); // Skip header

	while (std::getline(file, line))
	{
		if (line.empty()) continue;
		auto tokens = ParseCSVLine(line);
		if (tokens.size() < 3) continue;


	}

	file.close();
	return true;
}

const CDataReader::LevelData* CDataReader::GetLevelData(int levelID) const
{
	for (const auto& data : m_LevelData)
	{
		if (data.levelID == levelID)
		{
			return &data;
		}
	}
	return nullptr;
}

std::vector<CDataReader::TriggerData> CDataReader::GetTriggersForLevel(int levelID) const
{
	std::vector<TriggerData> result;
	for (const auto& data : m_TriggersData)
	{
		if (data.levelID == levelID)
		{
			result.push_back(data);
		}
	}
	return result;
}

std::vector<CDataReader::BlockedPathData> CDataReader::GetBlockedPathsForLevel(int levelID) const
{
	std::vector<BlockedPathData> result;
	for (const auto& data : m_BPathData)
	{
		if (data.levelID == levelID)
		{
			result.push_back(data);
		}
	}
	return result;
}

std::vector<CDataReader::EnemySpawnData> CDataReader::GetEnemySpawnsForLevel(int levelID) const
{
	std::vector<EnemySpawnData> result;
	for (const auto& data : m_EnemyData)
	{
		if (data.levelID == levelID)
		{
			result.push_back(data);
		}
	}
	return result;
}

std::vector<CDataReader::EnemySpawnData> CDataReader::GetEnemySpawnsForGroup(int levelID, int groupID) const
{
	std::vector<EnemySpawnData> result;
	for (const auto& data : m_EnemyData)
	{
		if (data.levelID == levelID && data.groupID == groupID)
		{
			result.push_back(data);
		}
	}
	return result;
}

std::vector<CDataReader::EnemyGroupData> CDataReader::GetEnemyGroupsForLevel(int levelID) const
{
	std::vector<EnemyGroupData> result;
	for (const auto& data : m_EnemyGroupsData)
	{
		if (data.levelID == levelID)
		{
			result.push_back(data);
		}
	}
	return result;
}

void CDataReader::Clear()
{
	m_LevelData.clear();
	m_TriggersData.clear();
	m_BPathData.clear();
	m_EnemyData.clear();
	m_EnemyGroupsData.clear();
}

std::vector<std::string> CDataReader::ParseCSVLine(const std::string& line)
{
	std::vector<std::string> tokens;
	std::stringstream ss(line);
	std::string token;

	while (std::getline(ss, token, ','))
	{
		tokens.push_back(Trim(token));
	}

	return tokens;
}

std::vector<int> CDataReader::ParseIntList(const std::string& str, char delimiter)
{
	std::vector<int> result;
	std::string trimmed = Trim(str);

	if (trimmed.empty())
	{
		return result;
	}

	std::stringstream ss(trimmed);
	std::string token;

	while (std::getline(ss, token, delimiter))
	{
		std::string t = Trim(token);
		if (!t.empty())
		{
			result.push_back(ParseInt(t));
		}
	}

	return result;
}

float CDataReader::ParseFloat(const std::string& str)
{
	try
	{
		return std::stof(Trim(str));
	}
	catch (...)
	{
		return 0.0f;
	}
}

int CDataReader::ParseInt(const std::string& str)
{
	try
	{
		return std::stoi(Trim(str));
	}
	catch (...)
	{
		return 0;
	}
}

std::string CDataReader::Trim(const std::string& str)
{
	const char* whitespace = " \t\n\r\f\v";
	size_t start = str.find_first_not_of(whitespace);

	if (start == std::string::npos)
	{
		return "";
	}

	size_t end = str.find_last_not_of(whitespace);
	return str.substr(start, end - start + 1);
}
