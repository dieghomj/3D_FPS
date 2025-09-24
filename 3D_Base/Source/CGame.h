#pragma once
#include "CDirectX9.h"
#include "CDirectX11.h"
#include "CDebugText.h"
#include "CSprite3D.h"
#include "CSprite2D.h"
#include "CSpriteObject.h"
#include "CExplosion.h"
#include "CUIObject.h"
#include "CStaticMesh.h"
#include "CStaticMeshObject.h"
#include "CCharacter.h"
#include "CPlayer.h"
#include "CGround.h"
#include "CEnemy.h"
#include "CShot.h"
#include "CSkinMesh.h"
#include "CZako.h"
#include "CRay.h"

#include <vector>


/********************************************************************************
*	ゲームクラス.
**/
class CGame
{
public:
	static constexpr int ENEMY_MAX = 3;	//エネミーの最大数

	CGame( CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd );
	~CGame();

	void Create();
	HRESULT LoadData();
	void Release();

	void Update();
	void Draw();

private:
	//カメラ関数.
	void Camera();
	//プロジェクション関数.
	void Projection();

	//三人称カメラ
	void ThirdPersonCamera(
		CAMERA* pCamera, const D3DXVECTOR3& TargetPos, float TargetRotY);

	void FirstPersonCamera(
		CAMERA* pCamera, const D3DXVECTOR3& TargetPos, POINT delta, float sense);

private:
	CDirectX9*		m_pDx9;
	CDirectX11*		m_pDx11;

	CDebugText*		m_pDbgText;	//デバッグテキスト

	//レイ表示クラス
	CRay*			m_pRayY;	//Y方向(垂直)
	CRay*			m_pCrossRay[CROSSRAY::max];	//十字

	//ウィンドウハンドル.
	HWND			m_hWnd;

	//カメラ情報.
	CAMERA			m_Camera;
	//ライト情報
	LIGHT			m_Light;

	//行列.
	D3DXMATRIX		m_mView;	//ビュー(カメラ)行列.
	D3DXMATRIX		m_mProj;	//射影（プロジェクション）行列.

	//ゲームで扱うスプライトデータ(使いまわす資源).
	CSprite3D*		m_pSpriteGround;
	CSprite3D*		m_pSpritePlayer;
	CSprite3D*		m_pSpriteExplosion;

	//スプライト2Dデータ(使いまわす資源)
	CSprite2D*		m_pSprite2DPmon;

	//スタティックメッシュ(使いまわす資源)
	CStaticMesh*	m_pStaticMeshFighter;	//自機
	CStaticMesh*	m_pStaticMeshGround;	//地面
	CStaticMesh*	m_pStaticMeshRoboA;		//ロボA
	CStaticMesh*	m_pStaticMeshRoboB;		//ロボB
	CStaticMesh*	m_pStaticMeshBullet;	//弾
	CStaticMesh*	m_pStaticMeshBSphere;	//バウンディングスフィア(当たり判定用)

	//スキンメッシュ(使いまわす資源)
	CSkinMesh*		m_pSkinMeshZako;		//ザコ
	int				m_ZakoAnimNo;			//ザコ：アニメーション番号
	double			m_ZakoAnimTime;			//ザコ：アニメーション経過時間
	D3DXVECTOR3		m_ZakoBonePos;			//ザコ：ボーン座標

	//スプライトオブジェクトクラス.
	CSpriteObject*		m_pExplosion;

	//UIオブジェクトクラス
	CUIObject*		m_pPmon;
	CUIObject*		m_pBeedrill;	//#015:スピアー
	CUIObject*		m_pParasect;	//#047:パラセクト
	CUIObject*		m_pScyther;		//#123:ストライク

	//スタティックメッシュオブジェクトクラス
	CStaticMeshObject*	m_pStcMeshObj;

	//キャラクタークラス
	CCharacter*			m_pPlayer;
	CCharacter*			m_pEnemy;
	CCharacter*			m_pEnemies[ENEMY_MAX];
	CCharacter**		m_ppEnemies;
	int					m_EnemyMax;

	//地面クラス
	CGround*			m_pGround;

	//弾クラス
	CShot*				m_pShot;

	//ザコクラス
	CZako*				m_pZako;

	//std::vector<宣言したい型名> 変数名
	std::vector<CZako*>	m_Zako;

	//mouse入力
	POINT m_mousePos;
	POINT m_mouseBeforePos;
	POINT m_mouseDelta;
	float m_mouseSense;

private:
	//=delete「削除定義」と呼ばれる機能.
	//指定された場合、その関数は呼び出せなくなる.
	CGame() = delete;	//デフォルトコンストラクタ禁止.
	CGame( const CGame& ) = delete;
	CGame& operator = (const CGame& rhs ) = delete;
};