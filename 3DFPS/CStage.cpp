#include "stdafx.h"
#include "CStage.h"

CStage::CStage()
    : m_pPlayer(nullptr)
{
}

CStage::~CStage()
{
}

void CStage::Update()
{
    if (!m_pPlayer) return;
    D3DXVECTOR3 frameStartPos = m_prevPlayerPos;
    D3DXVECTOR3 currentPos = m_pPlayer->GetPosition();

    //衝突補正の前に位置を保存する.
    HandleCeilingCollisions();

    HandleWallCollisions();

    HandleSweptCollisions(frameStartPos);

    HandleFloorCollisions();

    HandleStepUp();

    m_prevPlayerPos = m_pPlayer->GetPosition();

#if _DEBUG
    debugPlayerPath.push_back(m_prevPlayerPos);
    if (debugPlayerPath.size() > 500) {
        debugPlayerPath.erase(debugPlayerPath.begin());
    }
#endif // _DEBUG

    UpdateEnemyCollisions();

}

void CStage::SavePrevPlayerPos()
{
    m_prevPlayerPos = m_pPlayer->GetPosition();
}

void CStage::Draw(SCENE_DATA& sceneData)
{
    CStaticMeshObject::Draw(sceneData);
}

void CStage::UpdateEnemyCollisions()
{
    for (auto pEnemy : m_pEnemyPool)
    {
        if (!pEnemy || pEnemy->IsDead()) continue;

        // 床の衝突
        HandleEnemyFloorCollisions(pEnemy);

        // 壁の衝突
        HandleEnemyWallCollisions(pEnemy);
    }
}



void CStage::HandleWallCollisions()
{
    const int MAX_ITERATIONS = 4;
    const float MIN_CORRECTION = 0.001f;

    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration)
    {
        D3DXVECTOR3 totalCorrection(0.f, 0.f, 0.f);
        bool hadCollision = false;

        //胴体の高さのレイをチェック.
        CROSSRAY crossRay = m_pPlayer->GetCrossRay();
        D3DXVECTOR3 correction = CalculateWallPushback(&crossRay);
        if (D3DXVec3Length(&correction) > MIN_CORRECTION)
        {
            totalCorrection += correction;
            hadCollision = true;
        }

        //頭の高さのレイをチェック.
        CROSSRAY headCrossRay = m_pPlayer->GetHeadCrossRay();
        correction = CalculateWallPushback(&headCrossRay);
        if (D3DXVec3Length(&correction) > MIN_CORRECTION)
        {
            totalCorrection += correction;
            hadCollision = true;
        }

        if (!hadCollision)
            break;

        //補正を適用する(Yは維持する).
        D3DXVECTOR3 pos = m_pPlayer->GetPosition();
        pos.x += totalCorrection.x;
        pos.z += totalCorrection.z;
        m_pPlayer->SetPosition(pos.x, pos.y, pos.z);

        //壁に向かう速度を減衰させる.
        D3DXVECTOR3 velocity = m_pPlayer->GetVelocity();
        D3DXVECTOR3 correctionDir;
        D3DXVec3Normalize(&correctionDir, &totalCorrection);

        float velDotCorrection = D3DXVec3Dot(&velocity, &correctionDir);
        if (velDotCorrection < 0.f)
        {
            //壁に押し込む方向の速度成分を取り除く.
            velocity -= correctionDir * velDotCorrection * 0.8f;
            m_pPlayer->SetVelocity(velocity);
        }

        m_pPlayer->UpdateAxis();
    }

}

D3DXVECTOR3 CStage::CalculateWallPushback(CROSSRAY* pCrossRay)
{
    static constexpr float WALL_DISTANCE = 0.45f;  //0.3fから増加.
    static constexpr float MIN_HIT_DIST = 0.01f;

    D3DXVECTOR3 totalOffset(0.f, 0.f, 0.f);
    int hitCount = 0;

    for (int dir = 0; dir < CROSSRAY::max; dir++)
    {
        FLOAT distance;
        D3DXVECTOR3 intersect, normal;

        if (IsHitForRay(pCrossRay->Ray[dir], &distance, &intersect, &normal))
        {
            if (distance > MIN_HIT_DIST && distance < WALL_DISTANCE)
            {
                float pushAmount = WALL_DISTANCE - distance;

                //押し出し方向に法線を使用する(レイの方向より正確).
                D3DXVECTOR3 pushDir;
                D3DXVec3Normalize(&pushDir, &normal);

                totalOffset += pushDir * pushAmount;
                hitCount++;
            }
        }
    }

    //複数の壁にヒットした場合は平均をとる(角での過剰補正を防ぐ).
    if (hitCount > 1)
    {
        totalOffset /= (float)hitCount;
    }

    return totalOffset;
}


void CStage::HandleWallCollisions(CROSSRAY* cross)
{
    CalculatePositionFromWall(cross);
}

void CStage::HandleFloorCollisions()
{

    RAY downRay = m_pPlayer->GetRayY();

    FLOAT distance;
    D3DXVECTOR3 hitPoint, hitNormal;
    if (IsHitForRay(downRay, &distance, &hitPoint, &hitNormal))
    {
        //床を検出 - プレイヤーの床の高さと法線を更新する.
        m_pPlayer->SetFloorY(hitPoint.y);
        m_pPlayer->SetFloorNormal(hitNormal);
    }
    else
    {
        m_pPlayer->SetFloorY(-FLT_MAX); //床が検出されない.
        m_pPlayer->SetFloorNormal(D3DXVECTOR3(0.f, 1.f, 0.f));  //デフォルトは平坦.
    }
}

void CStage::HandleStepUp()
{
    if (!m_pPlayer->IsGrounded()) return;  //接地している時のみ段差を上る.

    const float MAX_STEP_HEIGHT = 0.25f;
    const float STEP_CHECK_DIST = 0.15f;

    CROSSRAY rays = m_pPlayer->GetCrossRay();

    for (int dir = 0; dir < CROSSRAY::max; ++dir)
    {
        FLOAT wallDist;
        D3DXVECTOR3 wallHit, wallNormal;

        if (IsHitForRay(rays.Ray[dir], &wallDist, &wallHit, &wallNormal))
        {
            if (wallDist < STEP_CHECK_DIST)
            {
                //上に空きスペースがあるかチェック.
                RAY upRay = rays.Ray[dir];
                upRay.Position.y += MAX_STEP_HEIGHT;

                FLOAT upDist;
                D3DXVECTOR3 intersect;

                if (!IsHitForRay(upRay, &upDist, &intersect) || upDist > STEP_CHECK_DIST)
                {
                    //段差なのでプレイヤーをスムーズに持ち上げる.
                    D3DXVECTOR3 pos = m_pPlayer->GetPosition();
                    float stepAmount = min(MAX_STEP_HEIGHT * 0.4f, MAX_STEP_HEIGHT);
                    pos.y += stepAmount;
                    m_pPlayer->SetPosition(pos.x, pos.y, pos.z);
                    break;  //1フレームに1段のみ.
                }
            }
        }
    }
}


//
void CStage::HandleSweptCollisions(const D3DXVECTOR3& frameStartPos)
{
    D3DXVECTOR3 currentPos = m_pPlayer->GetPosition();
    D3DXVECTOR3 movement = currentPos - frameStartPos;

    //スイープ衝突は水平方向の移動のみチェックする.
    D3DXVECTOR3 horizontalMovement(movement.x, 0.f, movement.z);
    float moveDistance = D3DXVec3Length(&horizontalMovement);

    if (moveDistance < 0.01f) return;

	const float PLAYER_RADIUS = 0.4f;

	auto SweepAndResolve = [&](float heightOffset)
	{
		RAY movementRay;
		movementRay.Position = frameStartPos;
		movementRay.Position.y += heightOffset;

		D3DXVec3Normalize(&movementRay.Axis, &horizontalMovement);
		movementRay.Length = moveDistance + PLAYER_RADIUS;
		movementRay.RotationY = 0.f;

		FLOAT hitDistance;
		D3DXVECTOR3 hitPoint, hitNormal;

#if _DEBUG
		debugSweptRay = movementRay;
		debugSweptHit = false;
#endif

		if (IsHitForRay(movementRay, &hitDistance, &hitPoint, &hitNormal))
		{
#if _DEBUG
			debugSweptHit = true;
#endif
			float safeDistance = max(0.f, hitDistance - PLAYER_RADIUS - 0.05f);

			if (safeDistance < moveDistance)
			{
				D3DXVECTOR3 safePos = frameStartPos;

				if (moveDistance > 0.001f)
				{
					D3DXVECTOR3 moveDir;
					D3DXVec3Normalize(&moveDir, &horizontalMovement);
					safePos.x += moveDir.x * safeDistance;
					safePos.z += moveDir.z * safeDistance;
				}

				//壁から離れる方向に少し押し出す.
				safePos.x += hitNormal.x * 0.05f;
				safePos.z += hitNormal.z * 0.05f;
				safePos.y = currentPos.y;  //現在のYを維持する.

				//壁に向かう速度を打ち消す.
				D3DXVECTOR3 velocity = m_pPlayer->GetVelocity();
				float velDotNormal = D3DXVec3Dot(&velocity, &hitNormal);
				if (velDotNormal < 0.f)
				{
					velocity.x -= hitNormal.x * velDotNormal;
					velocity.z -= hitNormal.z * velDotNormal;
					m_pPlayer->SetVelocity(velocity);
				}

				m_pPlayer->SetPosition(safePos.x, safePos.y, safePos.z);
				m_pPlayer->UpdateAxis();
			}
		}
	};

	//下部/中部のスイープ.
	SweepAndResolve(-m_pPlayer->GetHeight() * 0.5f);
	//頭上の障害物(例:低い開口部)を捉えるための上部スイープ.
	SweepAndResolve(0.0f);
}

void CStage::HandleEnemyWallCollisions(CAnimEnemy* pEnemy)
{
    if (!pEnemy) return;

    const int MAX_ITERATIONS = 3;

    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration)
    {
        CROSSRAY crossRay = pEnemy->GetCrossRay();
        D3DXVECTOR3 beforePos = crossRay.Ray[0].Position;

        CalculatePositionFromWall(&crossRay);

        D3DXVECTOR3 afterPos = crossRay.Ray[0].Position;
        D3DXVECTOR3 correction = afterPos - beforePos;

        if (D3DXVec3Length(&correction) < 0.0001f)
            break;

        D3DXVECTOR3 correctedPos = afterPos;
        correctedPos.y = pEnemy->GetPosition().y;
        pEnemy->SetPosition(correctedPos);
        pEnemy->UpdateCrossRay();
    }
 
}

void CStage::HandleEnemyFloorCollisions(CAnimEnemy* pEnemy)
{
    if (!pEnemy) return;

    RAY downRay = pEnemy->GetRayY();

    FLOAT distance;
    D3DXVECTOR3 hitPoint, hitNormal;

    if (IsHitForRay(downRay, &distance, &hitPoint, &hitNormal))
    {
        pEnemy->SetFloorY(hitPoint.y);
        pEnemy->SetFloorNormal(hitNormal);  // 床の法線を設定
    }
    else
    {
        pEnemy->SetFloorY(-FLT_MAX);
        pEnemy->SetFloorNormal(D3DXVECTOR3(0.0f, 1.0f, 0.0f));  // デフォルトの上向き法線
    }
}

void CStage::HandleEnemyStepUp(CAnimEnemy* pEnemy)
{
    if (!pEnemy) return;
    
    const float MAX_STEP_HEIGHT = 0.3f;  // 敵が上れる最大段差
    
    CROSSRAY rays = pEnemy->GetCrossRay();
    FLOAT wallDist;
    D3DXVECTOR3 wallHit, wallNormal;
    
    for (int dir = 0; dir < CROSSRAY::max; ++dir)
    {
        if (IsHitForRay(rays.Ray[dir], &wallDist, &wallHit, &wallNormal))
        {
            if (wallDist < 0.3f)  // 壁が近い
            {
                // 上に空きスペースがあるかチェック
                RAY upRay = rays.Ray[dir];
                upRay.Position.y += MAX_STEP_HEIGHT;
                
                if (!IsHitForRay(upRay, &wallDist, &wallHit, nullptr))
                {
                    // 階段検出！敵を持ち上げる
                    D3DXVECTOR3 pos = pEnemy->GetPosition();
                    pos.y += MAX_STEP_HEIGHT * 0.5f;  // スムーズにステップ
                    pEnemy->SetPosition(pos);
                    break;  // 1フレームに1方向のみ処理
                }
            }
        }
    }
}

void CStage::HandleCeilingCollisions()
{

	CROSSRAY headCrossray = m_pPlayer->GetHeadCrossRay();
    RAY upRay;
    D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
    upRay.Position = playerPos;
    upRay.Axis = D3DXVECTOR3(0.f, 1.f, 0.f);
    upRay.Length = m_pPlayer->GetHeight() * 1.5f;  //頭上をチェックする.
    upRay.RotationY = 0.f;

    FLOAT distance;
    D3DXVECTOR3 hitPoint, hitNormal;

    if (IsHitForRay(upRay, &distance, &hitPoint, &hitNormal))
    {
        m_pPlayer->SetCeilingY(hitPoint.y);
    }
    else
    {
        m_pPlayer->SetCeilingY(FLT_MAX);  //天井が検出されない.
    }
}



