#include "stdafx.h"
#include "CSettings.h"

float CSettings::m_mouseSensitivity = 1.f;
int CSettings::m_volume = 0;

CSettings::~CSettings()
{
}

void CSettings::SetConfigData(float mouseSense, int volume )
{

	m_mouseSensitivity = mouseSense;
	m_volume = volume;

}

bool CSettings::ParseConfigData()
{
	return false;
}
