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
		//前回の状態を保存.
		m_KeyPrevState[i] = m_KeyState[i];
		//現在のキーの押下状態を取得.
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

	if(!m_KeyPrevState[key] && m_KeyState[key])
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
