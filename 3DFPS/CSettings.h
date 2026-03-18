#pragma once
class CSettings
{


public:

	static void SetConfigData(float mouseSense, int volume);

	static float GetMouseSensitivity() { return m_mouseSensitivity; }
	static int GetVolume() { return m_volume; }

private:

	CSettings();
	CSettings(const CSettings&) = delete;
	CSettings& operator=(const CSettings&) = delete;
	~CSettings();

	static bool ParseConfigData();

	static float m_mouseSensitivity;
	static int	 m_volume;

};

