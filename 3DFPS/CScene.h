#pragma once
#include "CDirectX9.h"
#include "CDirectX11.h"
#include "CTime.h"
#include "CSceneManager.h"
#include "CSoundManager.h"
#include "CCamera.h"
#include "CCameraController.h"
#include "CSpotLight.h"
#include "CGameStats.h"
#include "CDataReader.h"

/********************************************************************************
*	ゲームシーンクラス.
**/
class CScene
{
public:

	CScene(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& m_pManager);

	virtual ~CScene();

	virtual void Create() = 0;
	virtual HRESULT LoadData() = 0;
	virtual void Release() = 0;

	virtual void Start() = 0;
	virtual void Update();
	virtual void Draw() = 0;

	virtual bool ShouldLockMouse() const { return true; }
	int AddSpotLight(CSpotLight* spotlight);
	void UpdateSpotLight(CSpotLight* spotlight);
	bool IsPause() const { return m_IsPause; }
	POINT GetMouseSeudoPos();
	static const TCHAR* DifficultyToText(CGameStats::DIFFICULTY d);
	void SaveSettings(float mouseSensitivity, float volume);

private :

	void UpdateMousePos();

protected:
	CDirectX9*		m_pDx9;
	CDirectX11*		m_pDx11;
	//ウィンドウハンドル.
	HWND			m_hWnd;
	CTime*			m_pTime;
	CSceneManager*	m_pManager;

	D3DXMATRIX		m_mView;
	D3DXMATRIX		m_mProj;

	LIGHT			m_GlobalLight;
	FOG				m_Fog;
	CAMERA			m_Camera;
	CCamera*		m_pCamera;
	CCameraController*			m_pCameraController;
	std::vector<SPOT_LIGHT>	m_pSpotLightList;
	SCENE_DATA		m_SceneInfo;

	bool			m_IsPause = false;

	//DATA READER
	CDataReader* m_pDataReader;

	//mouse入力
	POINT			m_mousePos;
	POINT			m_mouseSeudoPos;
	POINT			m_mouseBeforePos;
	POINT			m_mouseDelta;
	float			m_mouseSense;

private:
	//=delete「削除定義」と呼ばれる機能.
	//指定された場合、その関数は呼び出せなくなる.
	CScene() = delete;	//デフォルトコンストラクタ禁止.
	CScene(const CScene&) = delete;
	CScene& operator = (const CScene& rhs) = delete;
};

