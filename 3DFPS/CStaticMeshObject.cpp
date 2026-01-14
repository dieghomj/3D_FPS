#include "CStaticMeshObject.h"
#include "Util.h"

static constexpr float MAX_ANIM_TIME = 6.28318530718f; // 2π

CStaticMeshObject::CStaticMeshObject()
	: m_pMesh			( nullptr )
	, m_pCollider		( nullptr )
{
	m_pCollider = new CCollider();
}

CStaticMeshObject::~CStaticMeshObject()
{
	SAFE_DELETE( m_pCollider );
	DetachMesh();
}

void CStaticMeshObject::Update()
{
	if( m_pMesh == nullptr ){
		return;
	}
}

void CStaticMeshObject::Draw(
	SCENE_DATA& sceneData)
{

	D3DXMATRIX& View = sceneData.mView;
	D3DXMATRIX& Proj = sceneData.mProj;
	LIGHT& Light = sceneData.Light;
	CAMERA& Camera = sceneData.Camera;
	FOG& Fog = sceneData.Fog;
	SPOT_LIGHT* pSpotLightArr = sceneData.pSpotLightArray;
	int SpotLightNo = sceneData.SpotLightNum;

	if( m_pMesh == nullptr ){
		return;
	}

	if (m_mWorld._44 != 0.0f) {
		m_pMesh->SetWorldMatrix(m_mWorld);
	}
	else {
		m_pMesh->SetPosition(m_vPosition);
		m_pMesh->SetRotation(m_vRotation);
		m_pMesh->SetScale(m_vScale);
	}

	//レンダリング.
	m_pMesh->Render( View, Proj, Light, Camera.vPosition, Fog, pSpotLightArr, SpotLightNo);
}

HRESULT CStaticMeshObject::CreateCollider(CCollider::COLLIDER_SHAPE shape)
{
	if (!m_pMesh) return E_FAIL;
	if (!m_pCollider && !(m_pCollider = new CCollider())) return E_FAIL;
	if (FAILED(m_pCollider->Init())) return E_FAIL;

	// Set initial world position for collider
	m_pCollider->SetPosition(m_vPosition);
	m_pCollider->SetRotation(m_vRotation);
	m_pCollider->SetScale(m_vScale);

	HRESULT hr = E_FAIL;
	switch (shape)
	{
	case CCollider::COLLIDER_SHAPE_SPHERE:
		hr = m_pCollider->CreateSphereForMesh(*m_pMesh);
		break;
	case CCollider::COLLIDER_SHAPE_BOX:
		hr = m_pCollider->CreateBoxForMesh(*m_pMesh);
		break;
	case CCollider::COLLIDER_SHAPE_MESH:
		// Mesh collider creation not implemented
		hr = m_pCollider->CreateColMesh();
		break;
	default:
		return E_FAIL;
	}
	return hr;
}

//レイとメッシュの当たり判定
bool CStaticMeshObject::IsHitForRay(
	const RAY& pRay,
	float* pDistance,
	D3DXVECTOR3* pIntersect,
	D3DXVECTOR3* pNormal)		//(out)法線(ベクトル)
{
	D3DXVECTOR3 vAxis;		//軸ベクトル
	D3DXMATRIX	mRotationY;	//Y軸回転行列

	//Y軸回転行列を作成
	D3DXMatrixRotationY(&mRotationY, pRay.RotationY);
	//軸ベクトルを現在の回転状態に変換する
	D3DXVec3TransformCoord(&vAxis, &pRay.Axis, &mRotationY);

	//レイの始点と終点
	D3DXVECTOR3 StartPoint, EndPoint;
	StartPoint	= pRay.Position;	//レイの始点を設定
	EndPoint	= pRay.Position + (vAxis * pRay.Length);	//レイの終点を設定

	//レイを当てたいメッシュが移動している場合でも、
	//対象のWorld行列の逆行列を用いれば正しくレイが当たる
	D3DXMATRIX mWorld, mInverseWorld;
	D3DXMATRIX mTran;
	//移動処理
	D3DXMatrixTranslation(
		&mTran, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	//回転処理
	//※この行列計算は「CStaticMesh::Render()関数」と同じにする必要あり
	D3DXMATRIX mRot, mYaw, mPitch, mRoll;
	//Y軸回転行列作成
	D3DXMatrixRotationY(&mYaw, m_vRotation.y);
	//X軸回転行列作成
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);
	//Z軸回転行列作成
	D3DXMatrixRotationZ(&mRoll, m_vRotation.z);
	//回転行列を作成
	mRot = mYaw * mPitch * mRoll;

	//拡縮処理
	D3DXMATRIX mScale;
	D3DXMatrixScaling(&mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	//ワールド行列計算
	//拡縮×回転×移動　※順番がとても大切！！
	mWorld = mScale * mRot * mTran;

	//逆行列を求める
	D3DXMatrixInverse(&mInverseWorld, nullptr, &mWorld);
	//レイの始点、終点に反映
	D3DXVec3TransformCoord(&StartPoint, &StartPoint, &mInverseWorld);
	D3DXVec3TransformCoord(&EndPoint, &EndPoint, &mInverseWorld);

	//向きと長さ（大きさ）を求める
	D3DXVECTOR3 vDirection = EndPoint - StartPoint;

	BOOL bHit = FALSE;		//命中フラグ
	DWORD dwIndex = 0;		//インデックス番号
	D3DXVECTOR3 Vertex[3];	//頂点座標
	FLOAT U = 0, V = 0;		//重心ヒット座標

	//メッシュとレイの交差を調べる
	D3DXIntersect(
		m_pMesh->GetMeshForRay(),	//対象メッシュ
		&StartPoint,				//レイの始点
		&vDirection,				//レイの向きと長さ（大きさ）
		&bHit,						//(out)判定結果
		&dwIndex,	//(out)bHitがTRUEの時、レイの始点に最も近い面のインデックス値へのポインタ
		&U, &V,						//(out)重心ヒット座標
		pDistance,					//(out)メッシュとの距離
		nullptr, nullptr);

	//無限に伸びるレイのどこかでメッシュが当たっていたら
	if (bHit == TRUE)
	{
		//命中したとき
		FindVerticesOnPoly(
			m_pMesh->GetMeshForRay(), dwIndex, Vertex);

		//重心座標から交点を算出
		//ローカル交点は v0 + U*(v1-v0) + V*(v2-v0) で求まる
		*pIntersect =
			Vertex[0] + U * (Vertex[1] - Vertex[0]) + V * (Vertex[2] - Vertex[0]);

		//モデルデータが「拡縮」「回転」「移動」していれば行列が必要
		D3DXVec3TransformCoord(pIntersect, pIntersect, &mWorld);

		//法線の出力要求があれば
		if (pNormal != nullptr)
		{
			D3DXPLANE p;
			//その頂点(3点)から「平面の方程式」を得る
			D3DXPlaneFromPoints(&p, &Vertex[0], &Vertex[1], &Vertex[2]);
			//「平面の方程式」の係数が法線の成分
			*pNormal = D3DXVECTOR3(p.a, p.b, p.c);

			//法線は移動量算出に使用するので回転のみ処理する
			D3DXVec3TransformCoord(pNormal, pNormal, &mRot);
		}

		//EndPointから見た距離で1.fより小さければ当たっている
		if (*pDistance < 1.f) {
			return true;	//命中している
		}
	}
	return false;	//外れている
}

//壁からの位置を計算する
void CStaticMeshObject::CalculatePositionFromWall(CROSSRAY* pCrossRay)
{
#if 1
	static constexpr float WSPACE = 0.3f;	//壁との限界距離

	FLOAT Distance;			//レイの距離
	D3DXVECTOR3 Intersect;	//レイの交差点

	D3DXVECTOR3 vNormal;
	D3DXVECTOR3 vOffset = D3DXVECTOR3(0.f, 0.f, 0.f);

	//レイの向きにより当たる壁までの距離を求める（前後左右）
	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		if (IsHitForRay(pCrossRay->Ray[dir], &Distance, &Intersect, &vNormal))
		{
			if (Distance <= WSPACE)
			{
				//法線は壁から真っすぐに出ているので、法線とかけ合わせて、対象を動かすベクトルが得られる
				vOffset = vNormal * (WSPACE - Distance);

				//レイの位置を更新
				for (int i = 0; i < CROSSRAY::max; i++)
				{
					pCrossRay->Ray[i].Position += vOffset;
				}
			}
		}
	}

#else
	FLOAT Distance[CROSSRAY::max];			//各レイごとの距離
	D3DXVECTOR3 Intersect[CROSSRAY::max];	//各レイごとの交差点

	//レイの向きにより当たる壁までの距離を求める（前後左右）
	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		IsHitForRay(pCrossRay->Ray[dir], &Distance[dir], &Intersect[dir]);
	}

	float RotY;	//Y軸回転値
	//回転情報は全てのレイで同じはずなのでXLを使用する
	RotY = fabsf(pCrossRay->Ray[CROSSRAY::XL].RotationY);	//fabsf関数:絶対値(float版)
	ClampDirection(&RotY);	//0～2πの間に収める

	//-----------------------------------
	// 定数宣言
	//-----------------------------------
	static const float WSPACE = 0.8f;	//壁との限界距離
	static const float DEG45	= D3DXToRadian( 45.f);	//0.785f
	static const float DEG135	= D3DXToRadian(135.f);	//2.355f
	static const float DEG225	= D3DXToRadian(225.f);	//3.925f
	static const float DEG315	= D3DXToRadian(315.f);	//5.496f

	float Dis = 0.f;	//一時使用で宣言
	float TrgRotY = pCrossRay->Ray[CROSSRAY::XL].RotationY;
	D3DXVECTOR3 TrgPos = pCrossRay->Ray[CROSSRAY::XL].Position;

	//前が壁に接近
	Dis = Distance[CROSSRAY::ZF];
	if (0.01f < Dis && Dis < WSPACE) {
		//時計回り
		if (TrgRotY < 0.f)
		{
			//右から
			if( DEG45 <= RotY && RotY < DEG135)			{ TrgPos.x += WSPACE - Dis; }
			//前から
			else if( DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z += WSPACE - Dis; }
			//左から
			else if( DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x -= WSPACE - Dis; }
			//奥から
			else										{ TrgPos.z -= WSPACE - Dis; }
		}
		//反時計回り
		else
		{
			//右から
			if( DEG45 <= RotY && RotY < DEG135)			{ TrgPos.x -= WSPACE - Dis; }
			//前から
			else if( DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z += WSPACE - Dis; }
			//左から
			else if( DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x += WSPACE - Dis; }
			//奥から
			else										{ TrgPos.z -= WSPACE - Dis; }
		}
	}

	//後ろが壁に接近
	Dis = Distance[CROSSRAY::ZB];
	if (0.01f < Dis && Dis < WSPACE) {
		//時計回り
		if (TrgRotY < 0.f)
		{
			//右から
			if( DEG45 <= RotY && RotY < DEG135)			{ TrgPos.x -= WSPACE - Dis; }
			//前から
			else if( DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z -= WSPACE - Dis; }
			//左から
			else if( DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x += WSPACE - Dis; }
			//奥から
			else										{ TrgPos.z += WSPACE - Dis; }
		}
		//反時計回り
		else
		{
			//右から
			if( DEG45 <= RotY && RotY < DEG135)			{ TrgPos.x += WSPACE - Dis; }
			//前から
			else if( DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z -= WSPACE - Dis; }
			//左から
			else if( DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x -= WSPACE - Dis; }
			//奥から
			else										{ TrgPos.z += WSPACE - Dis; }
		}
	}

	//右が壁に接近
	Dis = Distance[CROSSRAY::XR];
	if (0.01f < Dis && Dis < WSPACE) {
		//時計回り
		if (TrgRotY < 0.f)
		{
			//右から
			if( DEG45 <= RotY && RotY < DEG135)			{ TrgPos.z -= WSPACE - Dis; }
			//前から
			else if( DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x += WSPACE - Dis; }
			//左から
			else if( DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z += WSPACE - Dis; }
			//奥から
			else										{ TrgPos.x -= WSPACE - Dis; }
		}
		//反時計回り
		else
		{
			//右から
			if( DEG45 <= RotY && RotY < DEG135)			{ TrgPos.z += WSPACE - Dis; }
			//前から
			else if( DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x += WSPACE - Dis; }
			//左から
			else if( DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z -= WSPACE - Dis; }
			//奥から
			else										{ TrgPos.x -= WSPACE - Dis; }
		}
	}

	//左が壁に接近
	Dis = Distance[CROSSRAY::XL];
	if (0.01f < Dis && Dis < WSPACE) {
		//時計回り
		if (TrgRotY < 0.f)
		{
			//右から
			if( DEG45 <= RotY && RotY < DEG135)			{ TrgPos.z += WSPACE - Dis; }
			//前から
			else if( DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x -= WSPACE - Dis; }
			//左から
			else if( DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z -= WSPACE - Dis; }
			//奥から
			else										{ TrgPos.x += WSPACE - Dis; }
		}
		//反時計回り
		else
		{
			//右から
			if( DEG45 <= RotY && RotY < DEG135)			{ TrgPos.z -= WSPACE - Dis; }
			//前から
			else if( DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x -= WSPACE - Dis; }
			//左から
			else if( DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z += WSPACE - Dis; }
			//奥から
			else										{ TrgPos.x += WSPACE - Dis; }
		}
	}


	//最終的な座標をレイに返す
	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		pCrossRay->Ray[dir].Position = TrgPos;
	}
#endif
}

void CStaticMeshObject::RotateAnim(float dt, float speed)
{

	m_RotateAnimTime += dt * speed;

	m_vRotation.y += m_RotateAnimTime;
	ClampDirection(&m_vRotation.y);

}

void CStaticMeshObject::UpDownAnim(float dt, float amp, float speed)
{

	m_UpDownAnimTime += dt * speed;

	m_vPosition.y += amp * sinf(m_UpDownAnimTime);

}

void CStaticMeshObject::VibrateAnim(float dt, float amp, float speed)
{
	m_VibrateAnimTime += dt * speed;
	m_vPosition += Util::CalcVibrationOffset(m_VibrateAnimTime, amp, m_vForward);
}

void CStaticMeshObject::ScaleAnim(float dt, float speed)
{
	m_ScaleAnimTime += dt * speed;

	// 各軸に異なるランダムな周波数を使用してモーフィング効果を作成
	float scaleOffsetX = sinf(m_ScaleAnimTime * 1.0f) * 0.1f;
	float scaleOffsetY = sinf(m_ScaleAnimTime * 1.3f) * 0.1f;
	float scaleOffsetZ = sinf(m_ScaleAnimTime * 0.8f) * 0.1f;

	// ベーススケール + ランダムオフセット
	m_vScale.x = 1.0f + scaleOffsetX;
	m_vScale.y = 1.0f + scaleOffsetY;
	m_vScale.z = 1.0f + scaleOffsetZ;
}

void CStaticMeshObject::ScaleMorphAnim(float dt, float amp, float speed)
{
	// 時間ベースのランダムなモーフィング
	m_ScaleMorphAnimTime += dt * speed;

	// 複数の正弦波を組み合わせて複雑なモーフィング効果を作成
	float morphX = sinf(m_ScaleMorphAnimTime * 1.0f) + cosf(m_ScaleMorphAnimTime * 2.3f) * 0.5f;
	float morphY = sinf(m_ScaleMorphAnimTime * 1.5f) + cosf(m_ScaleMorphAnimTime * 1.8f) * 0.5f;
	float morphZ = sinf(m_ScaleMorphAnimTime * 0.9f) + cosf(m_ScaleMorphAnimTime * 2.1f) * 0.5f;

	// 振幅を適用してスケールを設定
	m_vScale.x += (morphX * amp * 0.1f);
	m_vScale.y += (morphY * amp * 0.1f);
	m_vScale.z += (morphZ * amp * 0.1f);

	// スケールが負にならないように制限
	m_vScale.x = max(0.1f, m_vScale.x);
	m_vScale.y = max(0.1f, m_vScale.y);
	m_vScale.z = max(0.1f, m_vScale.z);
}

//交差位置のポリゴンの頂点を見つける
//※頂点座標はローカル座標
HRESULT CStaticMeshObject::FindVerticesOnPoly(
	LPD3DXMESH pMesh, DWORD dwPolyIndex, D3DXVECTOR3* pVertices)
{
	//頂点ごとのバイト数を取得
	DWORD dwStride = pMesh->GetNumBytesPerVertex();
	//頂点数を取得
	DWORD dwVertexAmt = pMesh->GetNumVertices();
	//面数を取得
	DWORD dwPolyAmt = pMesh->GetNumFaces();

	WORD* pwPoly = nullptr;

	//インデックスバッファをロック（読み込みモード）
	pMesh->LockIndexBuffer(
		D3DLOCK_READONLY,
		reinterpret_cast<VOID**>(&pwPoly));

	BYTE* pbVertices = nullptr;	//頂点（バイト型）
	FLOAT* pfVertices = nullptr;//頂点（FLOAT型）
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr;	//頂点バッファ

	//頂点情報の取得
	pMesh->GetVertexBuffer(&VB);

	//頂点バッファのロック
	if (SUCCEEDED(
		VB->Lock(0, 0, reinterpret_cast<VOID**>(&pbVertices), 0)))
	{
		//ポリゴンの頂点１つ目を取得
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3]]);
		pVertices[0].x = pfVertices[0];
		pVertices[0].y = pfVertices[1];
		pVertices[0].z = pfVertices[2];
		//ポリゴンの頂点２つ目を取得
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 1]]);
		pVertices[1].x = pfVertices[0];
		pVertices[1].y = pfVertices[1];
		pVertices[1].z = pfVertices[2];
		//ポリゴンの頂点３つ目を取得
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 2]]);
		pVertices[2].x = pfVertices[0];
		pVertices[2].y = pfVertices[1];
		pVertices[2].z = pfVertices[2];

		pMesh->UnlockIndexBuffer();	//ロック解除
		VB->Unlock();	//ロック解除
	}
	VB->Release();	//不要になったので解放

	return S_OK;
}

//回転値調整（１周以上している時の調整）
void CStaticMeshObject::ClampDirection(float* dir)
{
	if (*dir > D3DX_PI * 2.f)
	{
		//1周以上している
		*dir -= D3DX_PI * 2.f;	//2π(360°)分引く
	}

	if (*dir > D3DX_PI * 2.f)
	{
		//再帰関数
		ClampDirection(dir);
	}
}
