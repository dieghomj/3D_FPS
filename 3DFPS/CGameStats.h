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
	static unsigned long RemainingTime;        // プレイ時間の合計（ミリ秒）.
	static int ComboScore;              // コンボボーナススコア.
	static int HighestCombo;            // 今回のプレイで達成した最高コンボ.
	static int Score;                   // 今回のプレイの合計スコア.
	static int HighestScore;            // これまでに達成した最高スコア.
	static DIFFICULTY Difficulty;
	static int LevelSelection;
	static bool UnlockedLevel[LEVEL_COUNT];
	static float LevelTimer[LEVEL_COUNT];

	// コンボボーナスを含めた合計スコアを計算.
	static void ComputeScore()
	{
		const int perKill = 100;
		const int comboMultiplier = 50;
		int timeBonusPerSec = 2;
		int difficultyMultiplier = 1;

		// 残り時間ボーナスを計算.
		unsigned long remainingMs = GetRemainingTimeMs();
		int remainingSeconds = static_cast<int>(remainingMs / 1000);
		int timeBonus = remainingSeconds * timeBonusPerSec;

		// コンボボーナスを計算.
		ComboScore = HighestCombo * comboMultiplier;

		// 死亡ペナルティ.
		int deathPenalty = DeathCounter * 50;

		// ベーススコアの計算.
		int baseScore = 1000 + (EnemiesKilled * perKill);

		// 合計スコア.
		Score = (baseScore + timeBonus + ComboScore - deathPenalty) * difficultyMultiplier;
		if (Score < 0) Score = 0;

		// 上回った場合は最高スコアを更新.
		if (Score > HighestScore)
		{
			HighestScore = Score;
			SaveHighScore();
		}
	}

	// 残り時間をミリ秒で取得.
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

	// 起動時に保存済みのハイスコアを読み込む（作業ディレクトリの
	// highscore.dat）。ファイルがまだ無い場合 -> HighestScore は 0 のまま.
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

	// 現在のハイスコアをディスクに永続化.
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
inline unsigned long CGameStats::RemainingTime = 0;        // 残り時間（秒）.
inline int CGameStats::ComboScore = 0;              // コンボボーナススコア.
inline int CGameStats::HighestCombo = 0;            // 今回のプレイで達成した最高コンボ.
inline int CGameStats::Score = 0;                   // 今回のプレイの合計スコア.
inline int CGameStats::HighestScore = 0;            // これまでに達成した最高スコア.
inline CGameStats::DIFFICULTY CGameStats::Difficulty = DIFF_NORMAL;
inline int CGameStats::LevelSelection = 0;
inline bool CGameStats::UnlockedLevel[CGameStats::LEVEL_COUNT] = { true, false, false, false };
inline float CGameStats::LevelTimer[CGameStats::LEVEL_COUNT] = { 2.f * 60.f, 3.5f * 60.f, 2.f * 60.f, 0.f };