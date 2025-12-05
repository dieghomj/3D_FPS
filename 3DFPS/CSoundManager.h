#pragma once
#include "CSound.h"		//サウンドクラス.
#include <vector>
#include <unordered_map>

/**************************************************
*	サウンドマネージャークラス.
*	Manager(マネージャー)：管理者.
*		Singleton(シングルトン：デザインパターンの１つ)で作成.
**/
class CSoundManager
{
public:
	//サウンドリスト列挙型.
	enum enList
	{
		BGM_Title,	//タイトルBGM.
		BGM_Game,	//ゲームBGM.
		SE_Select,	//選択音.
		SE_Decide,	//決定音.
		SE_Step,	//足音.
		SE_GameOver,//ゲームオーバー音.
		SE_ItemGet,	//アイテム取得音
		SE_GhostIdle,	//ゴースト待機音
		SE_GhostChase,	//ゴースト追跡音
		SE_GhostDamage,	//ゴーストダメージ音
		SE_GhostDead,	//ゴースト死亡音
		SE_Flashlight,	//懐中電灯音
		SE_Result,		//リザルト音

		//音が増えたら「ここ」に追加してください.
		max,		//最大数.
	};

public:
	//インスタンス取得(唯一のアクセス経路).
	//※関数の前にstaticを付けることでインスタンス生成しなくても使用できる.
	static CSoundManager* GetInstance()
	{
		//唯一のインスタンスを作成する.
		//※staticで作成されたので2回目以降は、下の1行は無視される.
		static CSoundManager s_Instance;	//s_:staticの意味.
		return &s_Instance;
	}


	~CSoundManager();

	//サウンドデータ読込関数.
	bool Load( HWND hWnd );
	//サウンドデータ解放関数.
	void Release();

	//SEを再生する.
	static void PlaySE( enList list ) {
		CSoundManager::GetInstance()->m_pSound[list]->PlaySE();
	}
	//ループ再生する.
	static void PlayLoop( enList list ) {
		CSoundManager::GetInstance()->m_pSound[list]->PlayLoop();
	}
	static void SetVolume(enList list, int volume) {
		CSoundManager::GetInstance()->m_pSound[list]->SetVolume(volume);
	}
	//停止する.
	static void Stop( enList list ) {
		CSoundManager::GetInstance()->m_pSound[list]->Stop();
	}

	bool CreateVoicePool(enList list, int count, HWND hWnd);
	static void PlaySEPoly(enList list) {
		CSoundManager::GetInstance()->PlayFromPool(list);
	}

private://外部からアクセス不可能.
	//外部からコンストラクタへのアクセスを禁止する.
	CSoundManager();
	//コピーコンストラクタによるコピーを禁止する.
	//「=delete」で関数の定義を削除できる.
	CSoundManager( const CSoundManager& rhs ) = delete;
	//代入演算子によるコピーを禁止する.
	//operator(オペレータ):演算子のオーバーロードで、演算の中身を拡張できる.
	CSoundManager& operator = ( const CSoundManager& rhs ) = delete;

private:
	CSound*		m_pSound[enList::max];
	//込んだファイル情報を保持（プール再オープン用）
	struct SoundInfo {
		TCHAR path[256]{};
		TCHAR alias[32]{};
	};
	SoundInfo m_SoundInfo[enList::max]{};
	struct VoicePool {
		std::vector<CSound*> voices;
		size_t index = 0;
	};

	std::unordered_map<int, VoicePool> m_voicePools;

	// 追加: プールから再生
	void PlayFromPool(enList list);
};