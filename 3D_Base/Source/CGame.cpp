#include "CGame.h"
#include "CSoundManager.h"
#include "CEffect.h"	//Effekseerを使うためのクラス

//コンストラクタ.
CGame::CGame( CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd )
	: m_pDx9			( &pDx9 )
	, m_pDx11			( &pDx11 )
	, m_pDbgText		( nullptr )
	, m_pRayY			( nullptr )
	, m_pCrossRay		()
	, m_hWnd			( hWnd )
	, m_mView			()
	, m_mProj			()

	, m_Camera			()
	, m_Light			()

	, m_pSpriteGround	( nullptr )
	, m_pSpritePlayer	( nullptr )
	, m_pSpriteExplosion( nullptr )

	, m_pSprite2DPmon	( nullptr )

	, m_pStaticMeshFighter	( nullptr )
	, m_pStaticMeshGround	( nullptr )
	, m_pStaticMeshRoboA	( nullptr )
	, m_pStaticMeshRoboB	( nullptr )
	, m_pStaticMeshBullet	( nullptr )
	, m_pStaticMeshBSphere	( nullptr )

	, m_pSkinMeshZako	( nullptr )
	, m_ZakoAnimNo		()
	, m_ZakoAnimTime	()
	, m_ZakoBonePos		()

	, m_pExplosion		( nullptr )

	, m_pPmon			( nullptr )
	, m_pBeedrill		( nullptr )
	, m_pParasect		( nullptr )
	, m_pScyther		( nullptr )

	, m_pStcMeshObj		( nullptr )
	, m_pPlayer			( nullptr )
	, m_pEnemy			( nullptr )
	, m_pEnemies		()
	, m_ppEnemies		()
	, m_EnemyMax		( 0 )

	, m_pGround			( nullptr )

	, m_pShot			( nullptr )

	, m_pZako			( nullptr )

	, m_Zako			()
	, m_mousePos		({0,0})
	, m_mouseBeforePos	({0,0})
	, m_mouseDelta		({ 0,0 })
	, m_mouseSense		(0.005f)
{
	//カメラ座標.
	m_Camera.vPosition	= D3DXVECTOR3( 0.0f, 2.0f, 0.0f );
	m_Camera.vLook		= D3DXVECTOR3( 0.0f, 2.0f, 10.0f );

	//ライト情報
	m_Light.vDirection = D3DXVECTOR3( 1.5f, 1.f, -1.f );	//ライト方向
}


//デストラクタ.
CGame::~CGame()
{
	//ザコ複数の破棄
	// rbegin()	:末尾を指す逆イテレータを取得
	// rend()	:先頭を指す逆イテレータを取得
	for (auto it = m_Zako.rbegin(); it != m_Zako.rend(); ++it)
	{
		SAFE_DELETE( *it );
	}


	//ザコの破棄
	SAFE_DELETE( m_pZako );

	//弾の破棄
	SAFE_DELETE( m_pShot );

	//地面の破棄.
	SAFE_DELETE( m_pGround );

	//エネミーの破壊
#if 1
	if (m_ppEnemies != nullptr) {
		for (int No = m_EnemyMax - 1; No >= 0; No--) {
			SAFE_DELETE(m_ppEnemies[No]);
		}
		delete[] m_ppEnemies;
		m_ppEnemies = nullptr;
	}
#else
	for (int No = ENEMY_MAX - 1; No >= 0; No--) {
		SAFE_DELETE( m_pEnemies[No] );
	}
#endif
	SAFE_DELETE( m_pEnemy );

	//プレイヤーの破棄.
	SAFE_DELETE( m_pPlayer );

	//スタティックメッシュオブジェクトの破棄
	SAFE_DELETE( m_pStcMeshObj );

	//UIオブジェクトの破棄
	SAFE_DELETE( m_pScyther );
	SAFE_DELETE( m_pParasect );
	SAFE_DELETE( m_pBeedrill );
	SAFE_DELETE( m_pPmon );

	//爆発の破棄.
	SAFE_DELETE( m_pExplosion );

	//スキンメッシュの破棄
	SAFE_DELETE( m_pSkinMeshZako );

	//スタティックメッシュの破棄
	SAFE_DELETE( m_pStaticMeshBSphere );
	SAFE_DELETE( m_pStaticMeshBullet );
	SAFE_DELETE( m_pStaticMeshRoboB );
	SAFE_DELETE( m_pStaticMeshRoboA );
	SAFE_DELETE( m_pStaticMeshGround );
	SAFE_DELETE( m_pStaticMeshFighter );

	//スプライト2Dの破棄
	SAFE_DELETE( m_pSprite2DPmon );

	//爆発スプライトの解放.
	SAFE_DELETE( m_pSpriteExplosion );
	//プレイヤースプライトの解放.
	SAFE_DELETE( m_pSpritePlayer );
	//地面スプライトの解放.
	SAFE_DELETE( m_pSpriteGround );

	//レイ表示クラスの破棄
	for (int dir = CROSSRAY::max - 1; dir >= 0; dir--) {
		SAFE_DELETE( m_pCrossRay[dir] );
	}
	SAFE_DELETE( m_pRayY );

	//デバッグテキストの破棄
	SAFE_DELETE( m_pDbgText );

	//外部で作成しているので、ここでは破棄しない
	m_hWnd = nullptr;
	m_pDx11 = nullptr;
	m_pDx9 = nullptr;
}

//構築.
void CGame::Create()
{
	//デバッグテキストのインスタンス作成
	m_pDbgText = new CDebugText();

	//レイ表示クラスのインスタス作成
	m_pRayY = new CRay();
	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pCrossRay[dir] = new CRay();
	}

	//スプライトのインスタンス作成.
	m_pSpriteGround		= new CSprite3D();
	m_pSpritePlayer		= new CSprite3D();
	m_pSpriteExplosion	= new CSprite3D();

	//スプライト2Dのインスタンス作成
	m_pSprite2DPmon	= new CSprite2D();

	//スタティックメッシュのインスタンス作成
	m_pStaticMeshFighter	= new CStaticMesh();
	m_pStaticMeshGround		= new CStaticMesh();
	m_pStaticMeshRoboA		= new CStaticMesh();
	m_pStaticMeshRoboB		= new CStaticMesh();
	m_pStaticMeshBullet		= new CStaticMesh();
	m_pStaticMeshBSphere	= new CStaticMesh();

	//スキンメッシュのインスタンス作成
	m_pSkinMeshZako		= new CSkinMesh();

	//スプライトオブジェクトクラスのインスタス作成.
	m_pExplosion	= new CExplosion();

	//UIオブジェクトのインスタス作成
	m_pPmon			= new CUIObject();
	m_pBeedrill		= new CUIObject();
	m_pParasect		= new CUIObject();
	m_pScyther		= new CUIObject();

	//スタティックメッシュオブジェクトのインスタンス作成
	m_pStcMeshObj	= new CStaticMeshObject();

	//キャラクタークラスのインスタンス作成
	m_pPlayer		= new CPlayer();
	m_pEnemy		= new CEnemy();
#if 1
	//エネミーを動的に確保
	m_EnemyMax = 3;
	m_ppEnemies = new CCharacter*[m_EnemyMax]();
	for (int No = 0; No < m_EnemyMax; No++) {
		m_ppEnemies[No] = new CEnemy();
	}
#else
	for (int No = 0; No < ENEMY_MAX; No++) {
		m_pEnemies[No] = new CEnemy();
	}
#endif

	//地面クラスのインスタンス作成
	m_pGround = new CGround();
	m_pGround->SetPlayer(*m_pPlayer);	//レイの判定で必要

	//弾クラスのインスタンス作成
	m_pShot = new CShot();

	//ザコクラスのインスタス作成
	m_pZako = new CZako();

	//ザコ複数
	for (int i = 0; i < 3; i++) {
		//push_back(値)	:配列の末尾へ要素を追加
		//size()		:配列の要素数を取得
		m_Zako.push_back(new CZako());
	}

	//Effectクラス
	CEffect::GetInstance()->Create(
		m_pDx11->GetDevice(),
		m_pDx11->GetContext());
}

//ロードデータ関数.
HRESULT CGame::LoadData()
{
	//デバッグテキストの読み込み
	if (FAILED(m_pDbgText->Init( *m_pDx11 ))){
		return E_FAIL;
	}

	//サウンドデータの読み込み
	if (CSoundManager::GetInstance()->Load(m_hWnd) == false) {
		return E_FAIL;
	}

	//Effectクラス
	if (FAILED(CEffect::GetInstance()->LoadData())) {
		return E_FAIL;
	}

	//レイ表示クラスの初期化
	RAY ray = m_pPlayer->GetRayY();
	m_pRayY->Init(*m_pDx11, ray);
	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		ray = m_pPlayer->GetCrossRay().Ray[dir];
		m_pCrossRay[dir]->Init(*m_pDx11, ray);
	}

	//地面スプライトの構造体
	CSprite3D::SPRITE_STATE SSGround;
	SSGround.Disp.w = 1.f;
	SSGround.Disp.h = 1.f;
	SSGround.Base.w = 256.f;
	SSGround.Base.h = 256.f;
	SSGround.Stride.w = 256.f;
	SSGround.Stride.h = 256.f;
	//地面スプライトの読み込み.
	m_pSpriteGround->Init( *m_pDx11,
		_T( "Data\\Texture\\Ground.png" ), SSGround);

	//プレイヤースプライトの構造体
	CSprite3D::SPRITE_STATE SSPlayer =
		{ 1.f, 1.f, 64.f, 64.f, 64.f, 64.f };
	////プレイヤースプライトの読み込み.
	m_pSpritePlayer->Init( *m_pDx11,
		_T( "Data\\Texture\\Player.png" ), SSPlayer);

	//爆発スプライトの構造体
	CSprite3D::SPRITE_STATE SSExplosion =
		{ 1.f, 1.f, 256.f, 256.f, 32.f, 32.f };
	//爆発スプライトの読み込み.
	m_pSpriteExplosion->Init( *m_pDx11,
		_T( "Data\\Texture\\explosion.png" ), SSExplosion);

#if 1
	//Pモンスプライトの構造体
	CSprite2D::SPRITE_STATE SSPmon =
	{ 64.f, 64.f, 896.f, 560.f, 896.f / 16.f, 560.f / 10.f };
#else
	//Pモンスプライトの構造体
	CSprite2D::SPRITE_STATE SSPmon;
	SSPmon.Disp.w = 896.f;
	SSPmon.Disp.h = 560.f;
	SSPmon.Base = SSPmon.Stride = SSPmon.Disp;
#endif
	//Pモンスプライトの読み込み
	m_pSprite2DPmon->Init( *m_pDx11,
		_T( "Data\\Texture\\pmon.png" ), SSPmon );

	//スタティックメッシュの読み込み
	m_pStaticMeshFighter->Init( *m_pDx9, *m_pDx11,
		_T( "Data\\Mesh\\Static\\Shotgun\\Shotgun.x" ) );
	m_pStaticMeshGround->Init( *m_pDx9, *m_pDx11,
//		_T("Data\\Mesh\\Static\\Ground\\ground.x" ));
		_T("Data\\Mesh\\Static\\Stage\\stage.x"));
	m_pStaticMeshRoboA->Init(*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Robo\\RobotA_pivot.x"));
	m_pStaticMeshRoboB->Init(*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Robo\\RobotB_pivot.x"));
	m_pStaticMeshBullet->Init(*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Bullet\\bullet.x"));
	//バウンディングスフィア(当たり判定用)
	m_pStaticMeshBSphere->Init(*m_pDx9, *m_pDx11,
		_T("Data\\Collision\\Sphere.x"));

	//スキンメッシュの読み込み
	m_pSkinMeshZako->Init(*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Skin\\zako\\zako.x"));

	//爆発スプライトを設定.
	m_pExplosion->AttachSprite( *m_pSpriteExplosion );

	//Pモンスプライトを設定
	m_pPmon->AttachSprite( *m_pSprite2DPmon );
	m_pBeedrill->AttachSprite( *m_pSprite2DPmon );
	m_pParasect->AttachSprite( *m_pSprite2DPmon );
	m_pScyther->AttachSprite( *m_pSprite2DPmon );

	//スタティックメッシュを設定
	m_pStcMeshObj->AttachMesh( *m_pStaticMeshFighter );
	m_pPlayer->AttachMesh( *m_pStaticMeshFighter );
	m_pGround->AttachMesh( *m_pStaticMeshGround );
	m_pEnemy->AttachMesh( *m_pStaticMeshRoboB );
	m_pShot->AttachMesh( *m_pStaticMeshBullet );

	//スキンメッシュを設定
	m_pZako->AttachMesh( *m_pSkinMeshZako );
	m_pZako->SetScale( 0.002f );
	m_pZako->SetPosition( 0.f, 0.f, 8.f );

	//ザコ複数
	//範囲for文
	//auto:自動で型を推論、&をつけると参照になり不要なコピーを避けれる
	for (auto& e : m_Zako)
	{
		int i = static_cast<int>(&e - &m_Zako[0]);	//現在のインデックス番号算出

		e->SetPosition( -3.f + ( i * 3.f ), 0.f, 12.f);
		e->SetScale( 0.002f );
		e->AttachMesh( *m_pSkinMeshZako );
		e->SetAnimSpeed( 0.0001 + ( i * 0.0001 ) );
	}

	//Pモンそれぞれの画像パターンを設定
	m_pBeedrill->SetPatternNo( 14, 0 );
	m_pParasect->SetPatternNo( 14, 2 );
	m_pScyther->SetPatternNo( 10, 7 );

	//Pモンそれぞれの位置を設定
	const float size = SSPmon.Disp.w * 0.5f;	// 64.f
	const float pos_y = static_cast<float>(WND_H) - SSPmon.Disp.h;
	m_pBeedrill->SetPosition( size * 0.f, pos_y, 0.f );
	m_pParasect->SetPosition( size * 1.f, pos_y, 0.f );
	m_pScyther->SetPosition(  size * 2.f, pos_y, 0.f );

	//バウンディングスフィアの作成
	m_pPlayer->CreateBSphereForMesh(*m_pStaticMeshBSphere);
	m_pEnemy->CreateBSphereForMesh(*m_pStaticMeshBSphere);
	m_pShot->CreateBSphereForMesh(*m_pStaticMeshBullet);

	//キャラクターの初期座標を設定
	m_pPlayer->SetPosition( 0.f, 1.f, 6.f );
	m_pEnemy->SetPosition( 0.f, 1.f, 16.f );

	//エネミー複数設定
#if 1
	for (int No = 0; No < m_EnemyMax; No++) {
		auto& pE = m_ppEnemies[No];
		pE->AttachMesh(*m_pStaticMeshRoboA);
		pE->CreateBSphereForMesh(*m_pStaticMeshBSphere);
		pE->SetPosition(-3.f + (No * 3.f), 1.f, 10.f);
	}
#else
	for (int No = 0; No < ENEMY_MAX; No++) {
		auto& pE = m_pEnemies[No];
		pE->AttachMesh(*m_pStaticMeshRoboA);
		pE->CreateBSphereForMesh(*m_pStaticMeshBSphere);
		pE->SetPosition(-3.f + (No * 3.f), 1.f, 10.f);
	}
#endif

	return S_OK;
}

//解放関数.
void CGame::Release()
{
}


//更新処理.
void CGame::Update()
{
	//BGMのループ再生
	CSoundManager::PlayLoop(CSoundManager::BGM_Bonus);
	
#if 1
	// Get current mouse position in client coordinates
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(m_hWnd, &mousePos);

	// Calculate delta from center
	POINT center = { WND_W / 2, WND_H / 2 };
	m_mouseDelta.x = mousePos.x - center.x;
	m_mouseDelta.y = mousePos.y - center.y;

	// Reset cursor to center
	ClientToScreen(m_hWnd, &center);
	SetCursorPos(center.x, center.y);
#else
	//マウスの位置更新
	GetCursorPos(&m_mousePos);
	ClientToScreen(m_hWnd, &m_mousePos);

	//マウスの位置の変更更新
	m_mouseDelta = { 
		m_mousePos.x - m_mouseBeforePos.x , 
		m_mousePos.y - m_mouseBeforePos.y
	};
#endif

	m_mouseBeforePos = m_mousePos;

	//カメラ座標のデバックコマンド.
	float add_value = 0.1f;
	if( GetAsyncKeyState( 'W' ) & 0x8000 ){
		m_Camera.vPosition.y += add_value;
	}
	if( GetAsyncKeyState( 'S') & 0x8000 ){
		m_Camera.vPosition.y -= add_value;
	}
	if( GetAsyncKeyState( 'A' ) & 0x8000 ){
		m_Camera.vPosition.x -= add_value;
	}
	if( GetAsyncKeyState( 'D' ) & 0x8000 ){
		m_Camera.vPosition.x += add_value;
	}
	if( GetAsyncKeyState( 'Q' ) & 0x8000 ){
		m_Camera.vPosition.z += add_value;
	}
	if( GetAsyncKeyState( 'E' ) & 0x8000 ){
		m_Camera.vPosition.z -= add_value;
	}

//	m_pStcMeshObj->Update();

//	m_pGround->SetPosition(0.f, 0.f, -0.02f);
//	float rotX = static_cast<float>( D3DXToRadian(90.0) );
//	m_pGround->SetRotation(rotX, 0.f, 0.f);
//	m_pGround->SetScale( 5.f, 5.f, 1.f );

	m_pPlayer->Update();
	m_pGround->Update();

#if 1
	//弾を飛ばしたい!
	if (m_pPlayer->IsShot() == true) {
		m_pShot->Reload(
			m_pPlayer->GetPosition(),
			m_pPlayer->GetRotation().y);
	}
#else
	//弾を飛ばしたい!
	//dynamic_cast：親クラスのポインタを子クラスのポインタに変換する
	if (dynamic_cast<CPlayer*>(m_pPlayer)->IsShot() == true) {
		m_pShot->Reload(m_pPlayer->GetPosition());
	}
#endif
	m_pShot->Update();

	m_pEnemy->Update();

#if 1
	for (int No = 0; No < m_EnemyMax; No++) {
		m_ppEnemies[No]->Update();
	}
#else
	for (int No = 0; No < ENEMY_MAX; No++) {
		m_pEnemies[No]->Update();
	}
#endif

	m_pExplosion->Update();

	//----------------------------
	// スキンメッシュ
	//----------------------------
	m_pZako->Update();

	//ザコ複数
	for (auto& e : m_Zako) {
		e->Update();
	}

#if 1
	static POINTS PatternNo = { 0, 0 };
	const static POINTS PatternMax = m_pSprite2DPmon->GetPatternMax();

	if (GetAsyncKeyState('P') & 0x0001) {
		PatternNo.x++;
		if (PatternNo.x >= PatternMax.x) {
			PatternNo.x = 0;
			PatternNo.y++;
			if (PatternNo.y >= PatternMax.y) {
				PatternNo.y = 0;
			}
		}
		m_pPmon->SetPatternNo( PatternNo.x, PatternNo.y );
	}
#endif
	m_pPmon->Update();
	m_pBeedrill->Update();
	m_pParasect->Update();
	m_pScyther->Update();

	//Effect制御
	{
		//エフェクトのインスタンスごとに必要なハンドル
		//※３つ表示して制御するなら３つ必要になる
		static ::EsHandle hEffect = -1;

		if (GetAsyncKeyState('Y') & 0x0001) {
			hEffect = CEffect::Play(CEffect::Test0, D3DXVECTOR3(0.f, 1.f, 0.f));

			//拡大縮小
			CEffect::SetScale(hEffect, D3DXVECTOR3(0.8f, 0.8f, 0.8f));

			//回転(Y軸回転)
			CEffect::SetRotation(hEffect, D3DXVECTOR3(0.f, D3DXToRadian(90.0f), 0.f));

			//位置を再設定
			CEffect::SetLocation(hEffect, D3DXVECTOR3(0.f, 1.f, 1.f));
		}
		if (GetAsyncKeyState('T') & 0x0001) {
			CEffect::Stop(hEffect);
		}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
	}

	//三人称カメラ
	//ThirdPersonCamera(
	//	&m_Camera,
	//	m_pPlayer->GetPosition(),
	//	m_pPlayer->GetRotation().y);

	//一人称カメラ
	FirstPersonCamera(
		&m_Camera,
		m_pPlayer->GetPosition() ,
		m_mouseDelta,
		m_mouseSense);

	float rotX = m_Camera.yaw;
	m_pPlayer->SetRotation(0, rotX, 0);
}

//描画処理.
void CGame::Draw()
{
	Camera();
	Projection();

//	m_pStcMeshObj->Draw( m_mView, m_mProj, m_Light, m_Camera );

#if 1
	m_pGround->Draw( m_mView, m_mProj, m_Light, m_Camera );

	m_pPlayer->Draw( m_mView, m_mProj, m_Light, m_Camera );

	m_pShot->Draw( m_mView, m_mProj, m_Light, m_Camera );

	m_pEnemy->Draw( m_mView, m_mProj, m_Light, m_Camera );

#if 1
	for (int No = 0; No < m_EnemyMax; No++) {
		m_ppEnemies[No]->Draw( m_mView, m_mProj, m_Light, m_Camera );
	}
#else
	for (int No = 0; No < ENEMY_MAX; No++) {
		m_pEnemies[No]->Draw( m_mView, m_mProj, m_Light, m_Camera );
	}
#endif

	//----------------------------
	// スキンメッシュ
	//----------------------------
	m_pZako->Draw(m_mView, m_mProj, m_Light, m_Camera);
//	//ボーン座標に合わせて球体を表示
//	m_pStaticMeshBSphere->SetPosition(m_ZakoBonePos);
//	m_pStaticMeshBSphere->Render(m_mView, m_mProj, m_Light, m_Camera.vPosition);

	//ザコ複数
	for (auto& e : m_Zako) {
		e->Draw(m_mView, m_mProj, m_Light, m_Camera);
	}

	m_pPmon->Draw();

	//深度テスト無効にすることで、処理順番で描画させることができる
	m_pDx11->SetDepth(false);
	m_pBeedrill->Draw();
	m_pParasect->Draw();
	m_pScyther->Draw();
	//やりたいことが終わったので、深度テストを有効にしておく
	m_pDx11->SetDepth(true);

	//当たり判定の中心座標を更新する
	m_pPlayer->UpdateBSpherePos();
	m_pEnemy->UpdateBSpherePos();
	m_pShot->UpdateBSpherePos();

	//プレイヤーとエネミーの当たり判定
	if (m_pPlayer->GetBSphere()->IsHit(*m_pEnemy->GetBSphere()))
	{
		SetWindowText(m_hWnd, _T("衝突しています"));
	}
	else
	{
		SetWindowText(m_hWnd, _T(""));
	}

	//弾とエネミーの当たり判定
	if (m_pShot->GetBSphere()->IsHit(*m_pEnemy->GetBSphere()))
	{
		//爆発
		m_pExplosion->SetPosition(m_pEnemy->GetPosition());	//エネミーの位置にそろえる
		dynamic_cast<CExplosion*>( m_pExplosion )->ResetAnimation();//アニメーションリセット
		//弾
		m_pShot->SetDisplay(false);				//非表示
		m_pShot->SetPosition(0.f, -10.f, 0.f);	//地面に埋める
		//エネミー
		m_pEnemy->SetPosition(0.f, 1.f, 20.f);	//奥へ再配置
	}
	m_pExplosion->Draw(m_mView, m_mProj);

#else
	//ゲームオブジェクトのポインタのリストを作成
	CGameObject* pObjList[] =
	{
		m_pGround,
		m_pPlayer,
		m_pExplosion,
		m_pPmon,
		m_pBeedrill,
		m_pParasect,
		m_pScyther,
	};
	//配列の最大要素数を算出
	const int ObjListMax = sizeof(pObjList) / sizeof(pObjList[0]);
	//作成したリストでループ処理する
	for (int i = 0; i < ObjListMax; i++) {
		pObjList[i]->Draw( m_mView, m_mProj, m_Light, m_Camera );
	}

#endif

	//Effectクラス
	CEffect::GetInstance()->Draw(m_mView, m_mProj, m_Light, m_Camera);

	//レイの描画
	m_pRayY->Render(m_mView, m_mProj, m_pPlayer->GetRayY());
	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pCrossRay[dir]->Render(
			m_mView, m_mProj, m_pPlayer->GetCrossRay().Ray[dir]);
	}

	//デバッグテキストの描画
	m_pDbgText->SetColor(0.9f, 0.6f, 0.f);	//色の設定
	m_pDbgText->Render(_T("ABCD"), 10, 100);

	//デバッグテキスト(数値入り)の描画
	m_pDbgText->SetColor(1.f, 0.f, 0.f);
	TCHAR dbgText[64];
	_stprintf_s(dbgText, _T("Float:%f, %f"), 1.f, 2.2f );
	m_pDbgText->Render( dbgText, 10, 110 );

	m_pDbgText->SetColor(1.f, 1.f, 1.f);
	_stprintf_s(dbgText, _T("DELTA:%d, %d"), m_mouseDelta.x, m_mouseDelta.y);
	m_pDbgText->Render(dbgText, 10, 150);
}

//カメラ関数.
//※カメラ専用のクラスを作成したら便利だよ.
void CGame::Camera()
{
	D3DXVECTOR3 cam_pos	= m_Camera.vPosition;
	D3DXVECTOR3 cam_look = m_Camera.vLook;
	D3DXVECTOR3	vUpVec( 0.0f, 1.0f, 0.0f );	//上方（ベクトル）.

	//ビュー（カメラ）変換.
	D3DXMatrixLookAtLH(
		&m_mView,	//(out)ビュー計算結果.
		&cam_pos, &cam_look, &vUpVec );
}

//プロジェクション関数.
void CGame::Projection()
{
	//y方向の視野角。数値を大きくしたら視野が狭くなる.
	float fov_y	 = static_cast<FLOAT>( D3DXToRadian( 45.0 ) );	//ラジアン値.
	//アスペクト（幅÷高さ）.
	float aspect = static_cast<FLOAT>( WND_W ) / static_cast<FLOAT>( WND_H );
	float near_z = 0.1f;
	float far_z	 = 100.0f;

	//プロジェクション（射影）変換.
	D3DXMatrixPerspectiveFovLH(
		&m_mProj,	//(out)プロジェクション計算結果.
		fov_y,		//視野角（FOV：Field of View）.
		aspect,		//アスペクト.
		near_z,		//近いビュー平面のz値.
		far_z );	//遠いビュー平面のz値.
}

//三人称カメラ
void CGame::ThirdPersonCamera(
	CAMERA* pCamera, const D3DXVECTOR3& TargetPos, float TargetRotY)
{
	//Z軸ベクトル（Z+方向の単位ベクトル）
	D3DXVECTOR3 vecAxisZ(0.f, 0.f, 1.f);

	//Y方向の回転行列
	D3DXMATRIX mRotationY;
	//Y軸回転行列を作成
	D3DXMatrixRotationY(
		&mRotationY,	//(out)行列
		TargetRotY);	//対象のY方向の回転値

	//Y軸回転行列を使ってZ軸ベクトルを座標変換する
	D3DXVec3TransformCoord(
		&vecAxisZ,		//(out)Z軸ベクトル
		&vecAxisZ,		//(in)Z軸ベクトル
		&mRotationY);	//Y軸回転行列

	//カメラの位置、注視位置を対象にそろえる
	pCamera->vPosition	= TargetPos;
	pCamera->vLook		= TargetPos;

	//カメラの位置、注視位置をZ軸ベクトルを用いて調整
	pCamera->vPosition	-= vecAxisZ * 4.f;	//対象の背中側
	pCamera->vLook		+= vecAxisZ * 3.f;	//対象を挟んで向こう側

	//カメラの位置、注視位置の高さをそれぞれ微調整
	pCamera->vPosition.y+= 2.f;
	pCamera->vLook.y	+= 0.5f;
}

void CGame::FirstPersonCamera(
	CAMERA* pCamera, const D3DXVECTOR3& TargetPos, POINT delta, float sense)
{
	
	D3DXVECTOR3 vecAxisZ(0.f, 0.f, 1.f);

	pCamera->yaw	+= (float)delta.x * sense;
	pCamera->pitch	-= (float)delta.y * sense;

	const float pitchLimit = D3DX_PI * 0.49f;
	if (pCamera->pitch > pitchLimit) pCamera->pitch = pitchLimit;
	if (pCamera->pitch < -pitchLimit) pCamera->pitch = -pitchLimit;

	D3DXVECTOR3 lookDirection;
	lookDirection.x = cosf(pCamera->pitch) * sinf(pCamera->yaw);
	lookDirection.y = sinf(pCamera->pitch);
	lookDirection.z = cosf(pCamera->pitch) * cosf(pCamera->yaw);

	//カメラの位置、注視位置を対象にそろえる
	pCamera->vPosition = TargetPos;
	//カメラの位置、注視位置の高さをそれぞれ微調整
	pCamera->vPosition.y += 2.f;
	
	pCamera->vLook = pCamera->vPosition + lookDirection ;	//対象を挟んで向こう側
	//pCamera->vLook.y += 0.5f;
}