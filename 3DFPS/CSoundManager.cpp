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
		{ enList::BGM_Title		, _T("Data\\Sound\\BGM\\game.wav"),				_T("BGM_Title")},
		{ enList::BGM_Game		, _T("Data\\Sound\\BGM\\menu.mp3"),				_T("BGM_Game")},
		{ enList::SE_Select		, _T("Data\\Sound\\SE\\select.wav"),			_T("SE_Select")},
		{ enList::SE_Decide		, _T("Data\\Sound\\SE\\confirm.wav"),			_T("SE_Decide")},
		{ enList::SE_Step		, _T("Data\\Sound\\SE\\Footstep.wav"),			_T("SE_Step")},
		{ enList::SE_GameOver	, _T("Data\\Sound\\SE\\scream.wav"),			_T("SE_GameOver")},
		{ enList::SE_ItemGet	, _T("Data\\Sound\\SE\\item.wav"),				_T("SE_ItemGet")},
		{ enList::SE_GhostIdle	, _T("Data\\Sound\\SE\\enemyIdling.wav"),		_T("SE_GhostIdle")},
		{ enList::SE_GhostChase	, _T("Data\\Sound\\SE\\enemyAttacking.wav"),	_T("SE_GhostChase")},
		{ enList::SE_GhostDamage, _T("Data\\Sound\\SE\\enemyDamage.wav"),		_T("SE_GhostDamage")},
		{ enList::SE_GhostDead	, _T("Data\\Sound\\SE\\enemyDead.wav"),			_T("SE_GhostDead")},
		{ enList::SE_Flashlight	, _T("Data\\Sound\\SE\\Flashlight.wav"),		_T("SE_Flashlight")},
		{ enList::SE_Result		, _T("Data\\Sound\\SE\\result.wav"),			_T("SE_Result")},

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

bool CSoundManager::CreateVoicePool(enList list, int count, HWND hWnd)
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
	for (int i = 0; i < count; ++i)
	{
		CSound* s = new CSound();
		TCHAR alias[64] = _T("");
		wsprintf(alias, _T("%s_%d"), info.alias, i); // 例: SE_Shot_0

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
	CSound* voice = pool.voices[pool.index];
	if (voice)
	{
		voice->PlaySE();
	}
	pool.index = (pool.index + 1) % pool.voices.size();
}