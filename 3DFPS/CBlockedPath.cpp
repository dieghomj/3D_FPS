#include "CBlockedPath.h"

CBlockedPath::CBlockedPath()
	: CStaticMeshObject()
	, m_pBlockedPathSprite(nullptr)
{
}

CBlockedPath::~CBlockedPath()
{
}

void CBlockedPath::Update()
{
	if (!IsActive())
		return;
}

void CBlockedPath::Draw(SCENE_DATA& sceneData)
{
	if (IsActive() == false) {
		return;
	}

	if (m_pBlockedPathSprite == nullptr) {
		return;
	}

#if _DEBUG

	//CStaticMeshObject::Draw(sceneData);
	
#endif // _DEBUG

	m_pBlockedPathSprite->SetAlpha(0.8f);

	D3DXVECTOR3 offsetPosUp = D3DXVECTOR3(0.f, 2.f, 0.f);

	m_pBlockedPathSprite->SetPosition(m_vPosition);
	m_pBlockedPathSprite->SetRotation(m_vRotation);
	m_pBlockedPathSprite->SetScale(m_vScale);
	m_pBlockedPathSprite->Render(sceneData.mView, sceneData.mProj);
	m_pBlockedPathSprite->SetPosition(m_vPosition + offsetPosUp);
	m_pBlockedPathSprite->Render(sceneData.mView, sceneData.mProj);
	m_pBlockedPathSprite->SetPosition(m_vPosition - offsetPosUp);
	m_pBlockedPathSprite->Render(sceneData.mView, sceneData.mProj);
}

void CBlockedPath::HandleCubeCollisions(CPlayer* player)
{

	if (IsActive() == false)
	{
		return;
	}

	CCollider* pCollider = GetCollider();
	if (pCollider == nullptr)
	{
		return;
	}

	CBoundingCube* pBox = pCollider->GetBBox();
	if (pBox == nullptr)
	{
		return;
	}

	const D3DXVECTOR3 minBound = pBox->GetMin();
	const D3DXVECTOR3 maxBound = pBox->GetMax();

	const float playerRadius = player->GetRadius();
	const float playerHeight = player->GetHeight();
	D3DXVECTOR3 playerPos = player->GetPosition();
	const float playerFeet = playerPos.y - playerHeight;
	const float playerHead = playerPos.y + playerHeight * 0.3f;

	const bool overlapX = (playerPos.x + playerRadius) > minBound.x && (playerPos.x - playerRadius) < maxBound.x;
	const bool overlapZ = (playerPos.z + playerRadius) > minBound.z && (playerPos.z - playerRadius) < maxBound.z;
	const bool overlapY = playerHead > minBound.y && playerFeet < maxBound.y;

	if (!(overlapX && overlapZ && overlapY))
	{
		return;
	}

	const float centerX = (minBound.x + maxBound.x) * 0.5f;
	const float centerZ = (minBound.z + maxBound.z) * 0.5f;

	float penetrationX = (playerPos.x >= centerX)
		? maxBound.x - (playerPos.x - playerRadius)
		: (playerPos.x + playerRadius) - minBound.x;
	float penetrationZ = (playerPos.z >= centerZ)
		? maxBound.z - (playerPos.z - playerRadius)
		: (playerPos.z + playerRadius) - minBound.z;

	if (penetrationX <= 0.f && penetrationZ <= 0.f)
	{
		return;
	}

	D3DXVECTOR3 resolvedPos = playerPos;
	D3DXVECTOR3 playerVelocity = player->GetVelocity();

	if (penetrationX < penetrationZ)
	{
		const float pushDir = (playerPos.x >= centerX) ? 1.f : -1.f;
		resolvedPos.x += pushDir * penetrationX;
		playerVelocity.x = 0.f;
	}
	else
	{
		const float pushDir = (playerPos.z >= centerZ) ? 1.f : -1.f;
		resolvedPos.z += pushDir * penetrationZ;
		playerVelocity.z = 0.f;
	}

	player->SetPosition(resolvedPos);
	player->SetVelocity(playerVelocity);
	player->UpdateAxis();

}
