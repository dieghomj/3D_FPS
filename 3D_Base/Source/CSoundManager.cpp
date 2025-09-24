#include "CSoundManager.h"
#include "MyMacro.h"

CSoundManager::CSoundManager()
	: m_pSound	()
{
	//インスタンス生成.
	for( int i = 0; i < enList::max; i++ )
	{
		m_pSound[i] = new CSound();
	}
}

CSoundManager::~CSoundManager()
{
	Release();

	//インスタンス破棄.
	for( int i = enList::max - 1; i >= 0; i-- )
	{
		SAFE_DELETE( m_pSound[i] );
	}
}

//サウンドデータ読込関数.
bool CSoundManager::Load( HWND hWnd )
{
	struct SoundList
	{
		int listNo;				//enList列挙型を設定.
		const TCHAR path[256];	//ファイルの名前(パス付き).
		const TCHAR alias[32];	//エイリアス名.
	};
	SoundList SList[] =
	{
		{ enList::SE_Jump,		_T("Data\\Sound\\SE\\Jump.wav"),			_T("SE_Jump")	},
		{ enList::BGM_Bonus,	_T("Data\\Sound\\BGM\\BonusGameHouse.mp3"),	_T("BGM_Bonus")	},
		{ enList::SE_Clear,		_T("Data\\Sound\\SE\\Clear.wav"),			_T("SE_Clear")	},
	};
	//配列の最大要素数を算出 (配列全体のサイズ/配列1つ分のサイズ).
	int list_max = sizeof( SList ) / sizeof( SList[0] );
	for( int i = 0; i < list_max; i++ )
	{
		if( m_pSound[SList[i].listNo]->Open(
			SList[i].path,
			SList[i].alias,
			hWnd ) == false )
		{
			return false;
		}
	}

	return true;
}

//サウンドデータ解放関数.
void CSoundManager::Release()
{
	//開いた時と逆順で閉じる.
	for( int i = enList::max - 1; i >= 0; i-- )
	{
		if( m_pSound[i] != nullptr )
		{
			m_pSound[i]->Close();
		}
	}
}
