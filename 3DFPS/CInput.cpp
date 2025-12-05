#include "stdafx.h"
#include "CInput.h"

CInput::CInput()
	: m_KeyState{ false }
	, m_KeyPrevState{ false }
{
}

CInput::~CInput()
{
}

void CInput::Update()
{
	for (int i = 0; i < 256; ++i)
	{
		//‘O‰ñ‚Ìó‘Ô‚ð•Û‘¶.
		m_KeyPrevState[i] = m_KeyState[i];
		//Œ»Ý‚ÌƒL[‚Ì‰Ÿ‰ºó‘Ô‚ðŽæ“¾.
		if (GetAsyncKeyState(i) & 0x8000)
		{
			m_KeyState[i] = true;
		}
		else
		{
			m_KeyState[i] = false;
		}
	}
}

bool CInput::GetKeyDown(int key)
{

	if(m_KeyState[key])
	{
		return true;
	}
	return false;
}

bool CInput::GetKeyUp(int key)
{

	if (m_KeyPrevState[key] && !m_KeyState[key])
	{
		return true;
	}
	return false;
}

bool CInput::GetKey(int key)
{
	if (m_KeyState[key])
	{
		return true;
	}
	return false;
}
