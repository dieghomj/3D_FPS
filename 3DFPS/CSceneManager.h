#pragma once
#include <map>

class CScene;

class CSceneManager
{
public:

	CSceneManager();
	~CSceneManager();

	//Stacked State Machine追加
	//<----------
	
	//シーンリストにシーンを追加
	HRESULT AddScene(CScene* scene, const char* name);
	//シーン変更
	CScene* ChangeScene(const char* name);

	//現在のシーン更新
	void Update();
	//現在のシーン描画
	void Draw();	

private:
	
	CScene* m_pCurrentScene;
	std::map< const char*, CScene* > m_pSceneList;
};