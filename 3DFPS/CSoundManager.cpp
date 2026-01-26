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
		const TCHAR alias[256];	//エイリアス名.
	};
	SoundList SList[] =
	{
		{ enList::BGM_Game		, _T("Data\\Sound\\BGM\\game.mp3"),				_T("BGM_Title")},
		{ enList::BGM_Title		, _T("Data\\Sound\\BGM\\menu.mp3"),				_T("BGM_Game")},
		{ enList::BGM_GameOver	, _T("Data\\Sound\\BGM\\gameover.mp3"),			_T("BGM_GameOver")},
		{ enList::BGM_Restart	, _T("Data\\Sound\\BGM\\restart.mp3"),			_T("BGM_Restart")},

		{ enList::SE_Clear		, _T("Data\\Sound\\SE\\clear.wav"),				_T("SE_Clear")},
		{ enList::SE_Select		, _T("Data\\Sound\\SE\\select.mp3"),			_T("SE_Select")},
		{ enList::SE_Decide		, _T("Data\\Sound\\SE\\confirm.wav"),			_T("SE_Decide")},

		{ enList::SE_Step1		, _T("Data\\Sound\\SE\\footstep1.wav"),			_T("SE_Step1")},
		{ enList::SE_Step2		, _T("Data\\Sound\\SE\\footstep2.wav"),			_T("SE_Step2")},
		{ enList::SE_Step3		, _T("Data\\Sound\\SE\\footstep3.wav"),			_T("SE_Step3")},

		{ enList::SE_Alarm		, _T("Data\\Sound\\SE\\alarm.mp3"),				_T("SE_Alarm")},

		{ enList::SE_PlayerHit		, _T("Data\\Sound\\SE\\player_hit.wav"),		_T("SE_PlayerHit")},
		{ enList::SE_PistolShot1	, _T("Data\\Sound\\SE\\pistol_shot1.wav"),		_T("SE_PistolShot1")},
		{ enList::SE_PistolShot2	, _T("Data\\Sound\\SE\\pistol_shot2.wav"),		_T("SE_PistolShot2")},
		{ enList::SE_ShotgunShot	, _T("Data\\Sound\\SE\\shotgun_shot1.wav"),		_T("SE_ShotgunShot")},
		
		{ enList::SE_BossSE1 	, _T("Data\\Sound\\SE\\bossSE1.wav"),				_T("SE_BossSE1") },

		{ enList::SE_ItemGet	, _T("Data\\Sound\\SE\\item.wav"),					_T("SE_ItemGet")},

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
		lstrcpy(m_SoundInfo[SList[i].listNo].path, SList[i].path);
		lstrcpy(m_SoundInfo[SList[i].listNo].alias, SList[i].alias);
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
	for (auto& kv : m_voicePools)
	{
		for (auto* p : kv.second.voices)
		{
			if (p) { p->Close(); }
			SAFE_DELETE(p);
		}
	}
	m_voicePools.clear();
}

bool CSoundManager::CreateVoicePool(enList list, int count, HWND hWnd, DWORD durationMs)
{
	if (count <= 0) return false;

	// 既存プールがあれば破棄
	auto it = m_voicePools.find(list);
	if (it != m_voicePools.end())
	{
		for (auto* p : it->second.voices)
		{
			if (p) { p->Close(); }
			SAFE_DELETE(p);
		}
		m_voicePools.erase(it);
	}

	const SoundInfo& info = m_SoundInfo[list];
	if (info.path[0] == _T('\0') || info.alias[0] == _T('\0'))
	{
		// Load前
		return false;
	}

	VoicePool pool;
	pool.voices.reserve(static_cast<size_t>(count));
	pool.playStartTimes.resize(static_cast<size_t>(count), 0);
	pool.estimatedDuration = durationMs;  
	for (int i = 0; i < count; ++i)
	{
		CSound* s = new CSound();
		TCHAR alias[64] = _T("");
		wsprintf(alias, _T("%s_%d"), info.alias, i); 

		if (!s->Open(info.path, alias, hWnd))
		{
			s->Close();
			SAFE_DELETE(s);
			for (auto* p : pool.voices)
			{
				if (p) { p->Close(); }
				SAFE_DELETE(p);
			}
			return false;
		}
		pool.voices.push_back(s);
	}
	pool.index = 0;
	m_voicePools[list] = std::move(pool);
	return true;
}

void CSoundManager::PlayFromPool(enList list)
{
	auto it = m_voicePools.find(list);
	if (it == m_voicePools.end() || it->second.voices.empty())
	{
		m_pSound[list]->PlaySE();
		return;
	}

	VoicePool& pool = it->second;
	const size_t poolSize = pool.voices.size();
	DWORD now = timeGetTime();

	for (size_t i = 0; i < poolSize; ++i)
	{
		size_t idx = (pool.index + i) % poolSize;
		DWORD elapsed = now - pool.playStartTimes[idx];

		if (elapsed >= pool.estimatedDuration || pool.playStartTimes[idx] == 0)
		{
			CSound* voice = pool.voices[idx];
			if (voice)
			{
				voice->PlaySE();
				pool.playStartTimes[idx] = now;
			}
			pool.index = (idx + 1) % poolSize;
			return;
		}
	}
}