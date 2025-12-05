#pragma once

struct CGameStats
{
	enum DIFFICULTY
	{
		DIFF_EASY = 0,
		DIFF_NORMAL = 1,
		DIFF_HARD = 2
	};

	static void SetDifficulty(DIFFICULTY diff) { Difficulty = diff; }
	static DIFFICULTY GetDifficulty() { return Difficulty; }

	static int EnemiesKilled;
	static unsigned long TimeMs; // total time played in milliseconds
	static int Score;
	static DIFFICULTY Difficulty;

	// Simple score: 100 per kill, time penalty = floor(TimeMs/1000) * 2
	static void ComputeScore()
	{
		const int perKill = 100;
		int timePenaltyPerSec = 2;
		int bonus = 0;
		switch (Difficulty)
		{
			case DIFF_EASY:
				timePenaltyPerSec = 4;
				bonus = -100;
				break;
			case DIFF_NORMAL:
				timePenaltyPerSec = 3;
				bonus = 0;
				break;
			case DIFF_HARD:
				timePenaltyPerSec = 1;
				bonus = 100;
				break;
		}
		int seconds = static_cast<int>(TimeMs / 1000);
		Score = 1000 + (EnemiesKilled * perKill) - (seconds * timePenaltyPerSec);
		if (Score < 0) Score = 0;
	}

	static void Reset()
	{
		EnemiesKilled = 0;
		TimeMs = 0;
		Score = 0;
	}
};

// Static definitions
inline int CGameStats::EnemiesKilled = 0;
inline unsigned long CGameStats::TimeMs = 0;
inline int CGameStats::Score = 0;
inline CGameStats::DIFFICULTY CGameStats::Difficulty = CGameStats::DIFF_NORMAL;