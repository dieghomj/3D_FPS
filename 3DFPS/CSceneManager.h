#pragma once
#include <map>

class CScene;

class CSceneManager
{
public:

	CSceneManager();
	~CSceneManager();

	//スタック式ステートマシンの追加
	//<----------
	
	//シーンリストにシーンを追加
	HRESULT AddScene(CScene* scene, const char* name);
	//シーン変更
	CScene* ChangeScene(const char* name, bool release = false);

	//現在のシーン更新
	void Update();
	//現在のシーン描画
	void Draw();	

private:
	
	CScene* m_pCurrentScene;
	std::map< const char*, CScene* > m_pSceneList;
};