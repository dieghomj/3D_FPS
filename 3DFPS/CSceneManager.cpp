#include "CSceneManager.h"
#include "CScene.h"

CSceneManager::CSceneManager()
	: m_pCurrentScene	(nullptr)
	, m_pSceneList		()
{
}

CSceneManager::~CSceneManager()
{
	for (auto& scenePair : m_pSceneList)
	{
		if (scenePair.second)
		{
			scenePair.second->Release();
			SAFE_DELETE(scenePair.second);
		}
	}
	m_pSceneList.clear();
}

HRESULT CSceneManager::AddScene(CScene* scene, const char* name)
{
	
	scene->Create();

	m_pSceneList[name] = scene;

	return S_OK;
}

CScene* CSceneManager::ChangeScene(const char* name, bool release)
{
	//前のシーン破壊
	if (m_pCurrentScene && release)
	{
		m_pCurrentScene->Release();
	}

	m_pCurrentScene = m_pSceneList[name];

	if (FAILED(m_pCurrentScene->LoadData()))
	{
		OutputDebugString(_T("シーン切り替えのデータロードにエラー発生した"));
	}

	if (m_pCurrentScene)
	{
		m_pCurrentScene->Start();
	}

	return m_pCurrentScene;
}

void CSceneManager::Update()
{
	if (m_pCurrentScene)
	{
		m_pCurrentScene->Update();
	}
}

void CSceneManager::Draw()
{
	if (m_pCurrentScene)
	{
		m_pCurrentScene->Draw();
	}
}
