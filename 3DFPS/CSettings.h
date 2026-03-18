#pragma once
class CSettings
{


public:

	static void SetConfigData();

	static float GetMouseSensitivity() { return m_mouseSensitivity; }
	static int GetVolume() { return m_volume; }

private:

	CSettings();
	~CSettings();

	static bool ParseConfigData();

	static float m_mouseSensitivity;
	static int	 m_volume;

};

