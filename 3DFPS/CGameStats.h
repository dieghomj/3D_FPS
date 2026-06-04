#pragma once
#include <fstream> 

struct CGameStats
{
	enum DIFFICULTY
	{
		DIFF_EASY = 0,
		DIFF_NORMAL = 1,
		DIFF_HARD = 2
	};

	enum LEVEL_SELECTION
	{
		LEVEL_0 = 0,
		LEVEL_1 = 1,
		LEVEL_2 = 2,
		LEVEL_3 = 3,
		LEVEL_COUNT
	};

	static void SetDifficulty(DIFFICULTY diff) { Difficulty = diff; }
	static DIFFICULTY GetDifficulty() { return Difficulty; }

	static int EnemiesKilled;
	static int DeathCounter;
	static unsigned long RemainingTime;        // total time played in milliseconds
	static int ComboScore;              // combo bonus score
	static int HighestCombo;            // highest combo achieved in current run
	static int Score;                   // total score for current run
	static int HighestScore;            // highest score ever achieved
	static DIFFICULTY Difficulty;
	static int LevelSelection;
	static bool UnlockedLevel[LEVEL_COUNT];
	static float LevelTimer[LEVEL_COUNT];

	// Compute total score with combo bonus
	static void ComputeScore()
	{
		const int perKill = 100;
		const int comboMultiplier = 50;
		int timeBonusPerSec = 2;
		int difficultyMultiplier = 1;

		//switch (Difficulty)
		//{
		//case DIFF_EASY:
		//	timeBonusPerSec = 1;
		//	difficultyMultiplier = 1;
		//	break;
		//case DIFF_NORMAL:
		//	timeBonusPerSec = 2;
		//	difficultyMultiplier = 2;
		//	break;
		//case DIFF_HARD:
		//	timeBonusPerSec = 3;
		//	difficultyMultiplier = 3;
		//	break;
		//}

		// Calculate remaining time bonus
		unsigned long remainingMs = GetRemainingTimeMs();
		int remainingSeconds = static_cast<int>(remainingMs / 1000);
		int timeBonus = remainingSeconds * timeBonusPerSec;

		// Calculate combo bonus
		ComboScore = HighestCombo * comboMultiplier;

		// Death penalty
		int deathPenalty = DeathCounter * 50;

		// Base score calculation
		int baseScore = 1000 + (EnemiesKilled * perKill);

		// Total score
		Score = (baseScore + timeBonus + ComboScore - deathPenalty) * difficultyMultiplier;
		if (Score < 0) Score = 0;

		// Update highest score if beaten
		if (Score > HighestScore)
		{
			HighestScore = Score;
			SaveHighScore();
		}
	}

	// Get remaining time in milliseconds
	static unsigned long GetRemainingTimeMs()
	{
		return RemainingTime * 1000;
	}

	static void Reset()
	{
		EnemiesKilled = 0;
		DeathCounter = 0;
		RemainingTime = 0;
		ComboScore = 0;
		HighestCombo = 0;
		Score = 0;
	}

	// Load the saved high score at startup (highscore.dat in the working
	// directory). No file yet -> HighestScore stays 0.
	static void LoadHighScore()
	{
		std::ifstream ifs("highscore.dat", std::ios::binary);
		if (ifs.is_open())
		{
			int saved = 0;
			ifs.read(reinterpret_cast<char*>(&saved), sizeof(saved));
			if (ifs.gcount() == sizeof(saved) && saved > HighestScore)
				HighestScore = saved;
		}
	}

	// Persist the current high score to disk.
	static void SaveHighScore()
	{
		std::ofstream ofs("highscore.dat", std::ios::binary | std::ios::trunc);
		if (ofs.is_open())
		{
			ofs.write(reinterpret_cast<const char*>(&HighestScore), sizeof(HighestScore));
		}
	}

};

inline int CGameStats::EnemiesKilled = 0;
inline int CGameStats::DeathCounter = 0;
inline unsigned long CGameStats::RemainingTime = 0;        // remaining time seconds
inline int CGameStats::ComboScore = 0;              // combo bonus score
inline int CGameStats::HighestCombo = 0;            // highest combo achieved in current run
inline int CGameStats::Score = 0;                   // total score for current run
inline int CGameStats::HighestScore = 0;            // highest score ever achieved
inline CGameStats::DIFFICULTY CGameStats::Difficulty = DIFF_NORMAL;
inline int CGameStats::LevelSelection = 0;
inline bool CGameStats::UnlockedLevel[CGameStats::LEVEL_COUNT] = { true, false, false, false };
inline float CGameStats::LevelTimer[CGameStats::LEVEL_COUNT] = { 2.f * 60.f, 3.5f * 60.f, 2.f * 60.f, 0.f };