#include "CSceneManager.h"
#include "CScene.h"

CSceneManager::CSceneManager()
	: m_pCurrentScene	(nullptr)
	, m_pSceneList		()
{
}

CSceneManager::~CSceneManager()
{
}

HRESULT CSceneManager::AddScene(CScene* scene, const char* name)
{
	
	scene->Create();

	if (FAILED(scene->LoadData()))
	{
		return E_FAIL;
	}
	
	m_pSceneList[name] = scene;
}

CScene* CSceneManager::ChangeScene(const char* name, bool release)
{
	//‘O‚ÌƒV[ƒ“”j‰ó
	if (m_pCurrentScene && release)
	{
		m_pCurrentScene->Release();
	}

	m_pCurrentScene = m_pSceneList[name];

	if (FAILED(m_pCurrentScene->LoadData()))
	{
		return nullptr;
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
