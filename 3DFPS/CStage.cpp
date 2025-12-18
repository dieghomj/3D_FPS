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

	// 前回のプレイヤー位置を保存
    m_prevPlayerPos = m_pPlayer->GetPosition();
	debugPlayerPath.push_back(m_prevPlayerPos);

    HandleFloorCollisions();
    HandleCeilingCollisions();

    HandleWallCollisions();
    HandleSweptCollisions();

    HandleStepUp();


#if _DEBUG
	//debug用:移動経路を保存
    if(debugPlayerPath.size() > 200) {
        debugPlayerPath.erase(debugPlayerPath.begin());
	}
#endif
}

void CStage::Draw(SCENE_DATA& sceneData)
{
	CStaticMeshObject::Draw(sceneData);
}

void CStage::HandleWallCollisions()
{

    const int MAX_ITERATIONS = 3;

    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration)
    {
        CROSSRAY crossRay = m_pPlayer->GetCrossRay();
        D3DXVECTOR3 beforePos = crossRay.Ray[0].Position;

        CalculatePositionFromWall(&crossRay);

        D3DXVECTOR3 afterPos = crossRay.Ray[0].Position;

        D3DXVECTOR3 correctedPos = afterPos;
        correctedPos.y = m_pPlayer->GetPosition().y; 
        m_pPlayer->SetPosition(correctedPos.x, correctedPos.y, correctedPos.z);

        // If no correction happened, we're done
        D3DXVECTOR3 correction = afterPos - beforePos;
        correction.y = 0.f;
        if (D3DXVec3Length(&correction) < 0.001f)
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
    // Only allow step climbing when grounded
    if (!m_pPlayer->IsGrounded())
        return;

    const float MAX_STEP_HEIGHT = 0.4f; // Maximum climbable step

    CROSSRAY rays = m_pPlayer->GetCrossRay();
    FLOAT wallDist;
    D3DXVECTOR3 wallHit, wallNormal;

    for (int dir = 0; dir < CROSSRAY::max; ++dir)
    {
        if (IsHitForRay(rays.Ray[dir], &wallDist, &wallHit, &wallNormal))
        {
            if (wallDist < 0.3f) // Wall is close
            {
                // Check if there's empty space above the wall
                RAY upRay = rays.Ray[dir];
                upRay.Position.y += MAX_STEP_HEIGHT;

                FLOAT upDist;
                D3DXVECTOR3 upHit;

                if (!IsHitForRay(upRay, &upDist, &upHit, nullptr))
                {
                    // Empty above = it's a stair! Lift player
                    D3DXVECTOR3 pos = m_pPlayer->GetPosition();
                    pos.y += MAX_STEP_HEIGHT * 0.5f; // Smooth step
                    m_pPlayer->SetPosition(pos.x, pos.y, pos.z);
                    return; // Only climb one step per frame
                }
            }
        }
    }
}


//
void CStage::HandleSweptCollisions()
{
    D3DXVECTOR3 currentPos = m_pPlayer->GetPosition();
    D3DXVECTOR3 movement = currentPos - m_prevPlayerPos;

    // Only check horizontal movement (Y handled separately)
    movement.y = 0.f;
    float moveDistance = D3DXVec3Length(&movement);

    if (moveDistance < 0.001f) return;

    RAY movementRay;
    movementRay.Position = m_prevPlayerPos;
    movementRay.Position.y = m_prevPlayerPos.y - m_pPlayer->GetHeight() * 0.5f;  // Player center
    movementRay.Axis = movement;
    D3DXVec3Normalize(&movementRay.Axis, &movementRay.Axis);
    movementRay.Length = moveDistance + 0.5f;  // Add buffer for player radius
    movementRay.RotationY = 0.f;

#if _DEBUG
    debugSweptRay = movementRay;
    debugSweptHit = false;
#endif

    FLOAT hitDistance;
    D3DXVECTOR3 hitPoint, hitNormal;

    if (IsHitForRay(movementRay, &hitDistance, &hitPoint, &hitNormal))
    {
#if _DEBUG
        debugSweptHit = true;
#endif
        // Calculate safe distance
        float safeDistance = max(0.0f, hitDistance - 0.4f);
        D3DXVECTOR3 safePos = m_prevPlayerPos + movement * (safeDistance / moveDistance);
        safePos.y = currentPos.y;  // Preserve Y coordinate

        // Remove velocity component toward wall (horizontal only)
        D3DXVECTOR3 velocity = m_pPlayer->GetVelocity();
        D3DXVECTOR3 horizontalVel(velocity.x, 0.f, velocity.z);
        D3DXVECTOR3 horizontalNormal(hitNormal.x, 0.f, hitNormal.z);
        D3DXVec3Normalize(&horizontalNormal, &horizontalNormal);

        float velDotNormal = D3DXVec3Dot(&horizontalVel, &horizontalNormal);
        if (velDotNormal < 0.0f)  // Moving into wall
        {
            horizontalVel -= horizontalNormal * velDotNormal;  // Remove component toward wall
            velocity.x = horizontalVel.x;
            velocity.z = horizontalVel.z;
            // Keep velocity.y unchanged
            m_pPlayer->SetVelocity(velocity);
        }

        m_pPlayer->SetPosition(safePos.x, safePos.y, safePos.z);
    }
}

void CStage::HandleCeilingCollisions()
{
    RAY upRay;
    D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
    upRay.Position = playerPos;
    upRay.Position.y = playerPos.y - m_pPlayer->GetHeight() * 0.3f;  // From center-ish
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


