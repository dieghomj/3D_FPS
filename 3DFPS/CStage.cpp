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

    // Save position before collision correction
    HandleCeilingCollisions();

    HandleWallCollisions();

    HandleSweptCollisions();

    HandleFloorCollisions();

    HandleStepUp();

    m_prevPlayerPos = m_pPlayer->GetPosition();
    debugPlayerPath.push_back(m_prevPlayerPos);
    if (debugPlayerPath.size() > 500) {
        debugPlayerPath.erase(debugPlayerPath.begin());
    }

    UpdateEnemyCollisions();

}

void CStage::Draw(SCENE_DATA& sceneData)
{
    CStaticMeshObject::Draw(sceneData);
}

void CStage::UpdateEnemyCollisions()
{
    if (!m_pEnemyList) return;

    for (auto pEnemy : *m_pEnemyList)
    {
        if (!pEnemy) continue;

        // 床の衝突
        HandleEnemyFloorCollisions(pEnemy);

        // 壁の衝突
        HandleEnemyWallCollisions(pEnemy);

		//HandleEnemyStepUp(pEnemy);
    }
}

void CStage::HandleWallCollisions()
{
	CROSSRAY cross = m_pPlayer->GetCrossRay();
    HandleWallCollisions(&cross);
    cross = m_pPlayer->GetHeadCrossRay();
    HandleWallCollisions(&cross);

}

void CStage::HandleWallCollisions(CROSSRAY* cross)
{
    const int MAX_ITERATIONS = 3;  // Multiple passes for strong collision

    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration)
    {
        CROSSRAY crossRay = *cross;

        D3DXVECTOR3 beforePos = crossRay.Ray[0].Position;

        // Push away from walls
        CalculatePositionFromWall(&crossRay);

        D3DXVECTOR3 afterPos = crossRay.Ray[0].Position;

        // Apply correction
        D3DXVECTOR3 correctedPos = afterPos;
        correctedPos.y = m_pPlayer->GetPosition().y;  // Preserve Y
        m_pPlayer->SetPosition(correctedPos.x, correctedPos.y, correctedPos.z);

        // If no correction happened, we're done
        D3DXVECTOR3 correction = afterPos - beforePos;
        if (D3DXVec3Length(&correction) < 0.00001f)
        {
            break;
        }

        // Update rays for next iteration
        m_pPlayer->UpdateCrossRay();

    }
}

void CStage::HandleFloorCollisions()
{

    RAY downRay = m_pPlayer->GetRayY();

    FLOAT distance;
    D3DXVECTOR3 hitPoint, hitNormal;

    if (IsHitForRay(downRay, &distance, &hitPoint, &hitNormal))
    {
        // Floor found - update player's floor height
        m_pPlayer->SetFloorY(hitPoint.y);
    }
    else
    {
        m_pPlayer->SetFloorY(-FLT_MAX); // No floor detected
    }
}

void CStage::HandleStepUp()
{
    const float MAX_STEP_HEIGHT = 0.2f; // Maximum climbable step

    // Check if there's a wall in front
    CROSSRAY rays = m_pPlayer->GetCrossRay();
    FLOAT wallDist;
    D3DXVECTOR3 wallHit, wallNormal;

    for (int dir = 0; dir < CROSSRAY::max; ++dir)
    {

        if (IsHitForRay(rays.Ray[dir], &wallDist, &wallHit, &wallNormal))
        {
            if (wallDist < 0.1f) // Wall is close
            {
                // Check if there's empty space above the wall
                RAY upRay = rays.Ray[dir];
                upRay.Position.y += MAX_STEP_HEIGHT;

                if (!IsHitForRay(upRay, &wallDist, &wallHit, nullptr))
                {
                    // Empty above = it's a stair! Lift player
                    D3DXVECTOR3 pos = m_pPlayer->GetPosition();
                    pos.y += MAX_STEP_HEIGHT * 0.3f; // Smooth step
                    m_pPlayer->SetPosition(pos.x, pos.y, pos.z);
                }
            }
        }

    }
}


//
void CStage::HandleSweptCollisions()
{
    D3DXVECTOR3 currentPos = m_pPlayer->GetPosition();
    D3DXVECTOR3 movement = currentPos - m_prevPlayerPos;    //移動ベクトル
    float moveDistance = D3DXVec3Length(&movement);         //移動距離

    if (moveDistance < 0.001f) return;

    RAY movementRay;
    movementRay.Position = m_prevPlayerPos;
    movementRay.Position.y -= m_pPlayer->GetHeight() * 0.1f;  // Center of player
    movementRay.Axis = movement;
    D3DXVec3Normalize(&movementRay.Axis, &movementRay.Axis);
    movementRay.Length = moveDistance + 0.5f;  // Add buffer for player radius
    movementRay.RotationY = 0.f;                    //移動距離

    FLOAT hitDistance;
    D3DXVECTOR3 hitPoint, hitNormal;

    if (IsHitForRay(movementRay, &hitDistance, &hitPoint, &hitNormal))
    {
        float safeDistance = max(0.2f, hitDistance - 0.5f);
        D3DXVECTOR3 safePos = m_prevPlayerPos + movement * (safeDistance / moveDistance);
        safePos += hitNormal * 0.6f;

        D3DXVECTOR3 velocity = m_pPlayer->GetVelocity();
        float velDotNormal = D3DXVec3Dot(&velocity, &hitNormal);
        if (velDotNormal < 0.0f)  // Moving into wall
        {
            velocity -= hitNormal * velDotNormal;  // Remove velocity component toward wall
            m_pPlayer->SetVelocity(velocity);
        }

        m_pPlayer->SetPosition(safePos.x, safePos.y, safePos.z);
    }
    else
    {

    }
}

void CStage::HandleEnemyWallCollisions(CAnimEnemy* pEnemy)
{
    if (!pEnemy) return;

    const int MAX_ITERATIONS = 3;

    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration)
    {
        CROSSRAY crossRay = pEnemy->GetCrossRay();
        D3DXVECTOR3 beforePos = crossRay.Ray[0].Position;

        // 壁から押し出す
        CalculatePositionFromWall(&crossRay);

        D3DXVECTOR3 afterPos = crossRay.Ray[0].Position;

        // 補正を適用
        D3DXVECTOR3 correctedPos = afterPos;
        correctedPos.y = pEnemy->GetPosition().y;  // Y座標を保持
        pEnemy->SetPosition(correctedPos);

        // 補正がなければ終了
        D3DXVECTOR3 correction = afterPos - beforePos;
        if (D3DXVec3Length(&correction) < 0.00001f)
        {
            break;
        }

        // 次のイテレーション用にレイを更新
        pEnemy->UpdateCrossRay();
    }

    // 頭部の衝突も処理
    //for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration)
    //{
    //    CROSSRAY headCrossRay = pEnemy->GetHeadCrossRay();
    //    D3DXVECTOR3 beforePos = headCrossRay.Ray[0].Position;

    //    CalculatePositionFromWall(&headCrossRay);

    //    D3DXVECTOR3 afterPos = headCrossRay.Ray[0].Position;

    //    D3DXVECTOR3 correctedPos = afterPos;
    //    correctedPos.y = pEnemy->GetPosition().y;
    //    pEnemy->SetPosition(correctedPos);

    //    D3DXVECTOR3 correction = afterPos - beforePos;
    //    if (D3DXVec3Length(&correction) < 0.00001f)
    //    {
    //        break;
    //    }

    //    pEnemy->UpdateCrossRay();
    //}
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
    upRay.Length = m_pPlayer->GetHeight() * 1.5f;  // Check above head
    upRay.RotationY = 0.f;

    FLOAT distance;
    D3DXVECTOR3 hitPoint, hitNormal;

    if (IsHitForRay(upRay, &distance, &hitPoint, &hitNormal))
    {
        m_pPlayer->SetCeilingY(hitPoint.y);
    }
    else
    {
        m_pPlayer->SetCeilingY(FLT_MAX);  // No ceiling detected
    }
}



