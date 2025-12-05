#pragma once
#include <Windows.h>

class CInput
{

public:

	CInput();
	~CInput();
	//キー入力の更新.
	void Update();
	//キーが押された瞬間を取得.
	bool GetKeyDown(int key);
	//キーが離された瞬間を取得.
	bool GetKeyUp(int key);
	//キーが押されているか取得.
	bool GetKey(int key);

private:

	bool m_KeyState[256];			//キーの押下状態を保持.
	bool m_KeyPrevState[256];		//キーの前回の押下状態を保持.

};

