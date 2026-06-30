#include "stdafx.h"
#include "CLevel.h"

CLevel::CLevel()
	: m_IsCleared(false)
	, m_pPlayer(nullptr)
	, m_Goal()
	, m_pBlockedPathList()
	, m_TriggerAreas()
{
}

CLevel::~CLevel()
{
}

void CLevel::Init()
{

	m_IsCleared = false;
}

void CLevel::Update()
{

	CheckTriggers();
	CheckGoal();
	UpdateBlockedPaths();

}

void CLevel::Draw()
{
}

void CLevel::Restart()
{
	m_IsCleared = false;
	for (auto& trigger : m_TriggerAreas)
	{
		trigger.isTriggered = false;
	}
	for (auto pBlockedPath : m_pBlockedPathList)
	{
		pBlockedPath->SetActive(false);
	}
}

void CLevel::CheckGoal()
{

	if (IsPlayerInTriggerArea(m_Goal))
	{
		m_IsCleared = true;
	}

}

void CLevel::CheckTriggers()
{

	for (auto& trigger : m_TriggerAreas)
	{
		if (trigger.isTriggered)
			continue;

		if (IsPlayerInTriggerArea(trigger))
		{
			trigger.isTriggered = true;

			for (auto index : trigger.blockedPathIndices)
			{
				if (index >= 0 && index < m_pBlockedPathList.size())
				{
					m_pBlockedPathList[index]->SetActive(true);

					if (trigger.blockBehindPlayer)
					{
						D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
						D3DXVECTOR3 pathPos = m_pBlockedPathList[index]->GetPosition();
						if (pathPos.z < playerPos.z)
						{
							m_pBlockedPathList[index]->SetActive(false);
						}
					}

				}
			}
		}
	}

}

bool CLevel::IsPlayerInTriggerArea(const COLLISION_TRIGGER& trigger)
{

	D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();

	// AABBによる当たり判定.
	return (playerPos.x >= trigger.position.x - trigger.size.x &&
		playerPos.x <= trigger.position.x + trigger.size.x &&
		playerPos.y >= trigger.position.y - trigger.size.y &&
		playerPos.y <= trigger.position.y + trigger.size.y &&
		playerPos.z >= trigger.position.z - trigger.size.z &&
		playerPos.z <= trigger.position.z + trigger.size.z);

	return false;

}

bool CLevel::IsPlayerInTriggerArea(const GOAL& trigger)
{
	D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();

	// AABBによる当たり判定.
	return (playerPos.x >= trigger.position.x - trigger.size.x &&
		playerPos.x <= trigger.position.x + trigger.size.x &&
		playerPos.y >= trigger.position.y - trigger.size.y &&
		playerPos.y <= trigger.position.y + trigger.size.y &&
		playerPos.z >= trigger.position.z - trigger.size.z &&
		playerPos.z <= trigger.position.z + trigger.size.z);

	return false;
}

void CLevel::UpdateBlockedPaths()
{
	for (auto pBlockedPath : m_pBlockedPathList)
	{
		if (pBlockedPath->IsActive() == false)
		{
			continue;
		}
		pBlockedPath->Update();
		pBlockedPath->UpdateCollider();
		pBlockedPath->HandleCubeCollisions(m_pPlayer);
	}
}

void CLevel::HandleBlockedPath()
{
}