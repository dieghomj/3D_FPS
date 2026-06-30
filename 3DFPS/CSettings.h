#pragma once
#include <fstream>

//********************************************************************************
//	ゲーム設定クラス.
//		音量・マウス感度などのユーザー設定を保持し、
//		settings.dat へ保存・読込を行う（静的メンバのみで構成）.
//********************************************************************************
struct CSettings
{
	// 音量スライダーの範囲・刻み幅（0～100 のパーセント）.
	static constexpr int   VOLUME_MIN  = 0;
	static constexpr int   VOLUME_MAX  = 100;
	static constexpr int   VOLUME_STEP = 5;

	// マウス感度スライダーの範囲・刻み幅（カメラ回転係数）.
	static constexpr float SENSE_MIN  = 0.02f;
	static constexpr float SENSE_MAX  = 0.30f;
	static constexpr float SENSE_STEP = 0.01f;

	// マスター音量（0～100）.
	static int   MasterVolume;
	// マウス感度（カメラ回転に用いる係数）.
	static float MouseSensitivity;

	static int   GetVolume()      { return MasterVolume; }
	static float GetSensitivity() { return MouseSensitivity; }

	// 音量(0～100)を CSound のスケール(0～1000)へ変換する.
	static int VolumeToScale() { return MasterVolume * 10; }

	// 音量を範囲内に丸めつつ加算する.
	static void AddVolume(int delta)
	{
		MasterVolume += delta;
		if (MasterVolume < VOLUME_MIN) { MasterVolume = VOLUME_MIN; }
		if (MasterVolume > VOLUME_MAX) { MasterVolume = VOLUME_MAX; }
	}

	// マウス感度を範囲内に丸めつつ加算する.
	static void AddSensitivity(float delta)
	{
		MouseSensitivity += delta;
		if (MouseSensitivity < SENSE_MIN) { MouseSensitivity = SENSE_MIN; }
		if (MouseSensitivity > SENSE_MAX) { MouseSensitivity = SENSE_MAX; }
	}

	// 設定を settings.dat へ保存する.
	static void Save()
	{
		std::ofstream ofs("settings.dat", std::ios::binary | std::ios::trunc);
		if (ofs.is_open())
		{
			ofs.write(reinterpret_cast<const char*>(&MasterVolume), sizeof(MasterVolume));
			ofs.write(reinterpret_cast<const char*>(&MouseSensitivity), sizeof(MouseSensitivity));
		}
	}

	// 設定を settings.dat から読込する. ファイルが無ければ既定値のまま.
	static void Load()
	{
		std::ifstream ifs("settings.dat", std::ios::binary);
		if (ifs.is_open())
		{
			int   savedVolume = MasterVolume;
			float savedSense  = MouseSensitivity;
			ifs.read(reinterpret_cast<char*>(&savedVolume), sizeof(savedVolume));
			ifs.read(reinterpret_cast<char*>(&savedSense), sizeof(savedSense));
			if (!ifs.fail())
			{
				MasterVolume     = savedVolume;
				MouseSensitivity = savedSense;
				// 念のため範囲内に丸める.
				AddVolume(0);
				AddSensitivity(0.f);
			}
		}
	}
};

inline int   CSettings::MasterVolume     = 80;
inline float CSettings::MouseSensitivity = 0.1f;
