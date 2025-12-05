#include "CBaseEnemy.h"
#include "CMaze.h"
#include "CSpotLight.h"

CBaseEnemy::CBaseEnemy()
	: CAnimCharacter()
	, moveTime		(0.f)
	, moveSpeed		(0.09f)
	, m_pathStep		(0)
	, m_MazeWidth		(0)
	, m_MazeHeight		(0)
	, m_CurrentRow		(0)
	, m_CurrentCol		(0)
	, m_StepList		()
{
}

CBaseEnemy::CBaseEnemy(std::vector<Pair> path, int width, int height)
	: CAnimCharacter()
	, moveTime		(0.f)
	, moveSpeed		(0.09f)
	, m_pathStep	(0)
	, m_MazeWidth	(width)
	, m_MazeHeight	(height)
	, m_StepList	(path)
	, m_CurrentCol	(path[0].x)
	, m_CurrentRow	(path[0].y)
	, m_State		(Move)
	, m_pPlayer		(nullptr)
{
}


CBaseEnemy::~CBaseEnemy()
{
	m_StepList.clear();
}

void CBaseEnemy::Update()
{
	moveTime += 0.016f;
	D3DXVECTOR3 diff = m_pPlayer->GetPosition() - m_vPosition;
	m_fDistanceFromPlayer = D3DXVec3Length(&diff);

	switch (m_State)
	{
	case enState::Move:
		// ˆÚ“®ˆ—
		IdleMove();
		break;

	case enState::Attack:
		AttackPlayer();
		break;

	case enState::Retiring:
		// ‘Þ‹pˆ—
		if (m_RetreatTimer > 0.0f)
		{
			D3DXVECTOR3 away = m_vPosition - m_pPlayer->GetPosition();
			if (D3DXVec3LengthSq(&away) > 0.f)
			{
				D3DXVec3Normalize(&away, &away);
			}
			// Slightly faster flee than normal move
			const float fleeSpeed = moveSpeed;
			m_vPosition += away * fleeSpeed;
			m_RetreatTimer -= 0.016f;

			// Face away direction for visual feedback
			float targetAngle = atan2f(away.x, away.z);
			m_vRotation.y += (targetAngle - m_vRotation.y) * 0.15f;
		}
		else
		{
			// Return to normal behavior if still alive
			if (m_Health > 0.0f)
			{
				m_State = Move;
			}
			else
			{
				m_State = Dead;
			}
		}

		break;

	case enState::Dead:
		// Ž€–Sˆ—
		break;

	default:
		break;
	}

	if (m_State == Attack)
	{
		// Use forward toward the player to orient sideways jitter
		D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
		D3DXVECTOR3 forward = playerPos - m_vPosition;
		if (D3DXVec3LengthSq(&forward) > 0.f)
		{
			D3DXVec3Normalize(&forward, &forward);
		}

		// Tune amplitude (units) and frequency (Hz)
		const float amplitude = 0.09f;   // small spatial jitter
		const float frequency = 10.0f;    // 8 shakes per second

		m_vPosition += CalcVibrationOffset(moveTime, amplitude, frequency, forward);
	}

	CAnimCharacter::Update();

}

void CBaseEnemy::Draw(SCENE_DATA& sceneData)
{
	if (m_State == Dead)
		return;
	CAnimCharacter::Draw(sceneData);
}

void CBaseEnemy::Start()
{
	m_State = Move;
	m_Health = 100.f;
}

void CBaseEnemy::SetPath(std::vector<Pair> path)
{
	m_StepList = path;
	m_pathStep = 0;

}

void CBaseEnemy::ReactToSpotLight(const CSpotLight& spotLight)
{
	if (IsDead())
		return;

	if (IsLitBySpot(spotLight))
	{
		const float damage = 0.5f;
		const float retreatForce = moveSpeed;
		ApplyRetreatAndDamage(damage, retreatForce);
	}
}

void CBaseEnemy::GetNextStep()
{
	m_pathStep++;
	if (m_pathStep >= m_StepList.size()) {
		m_pathStep = 0;
	}
}

void CBaseEnemy::AttackPlayer()
{
	{
		// UŒ‚ˆ—
		D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
		D3DXVECTOR3 vDirection = SetRotationToTarget(playerPos);

		if (m_fDistanceFromPlayer < 15.f)
		{
			m_vPosition += vDirection * moveSpeed;
		}
		else
		{
			m_State = Move;
		}

	}
}

D3DXVECTOR3 CBaseEnemy::SetRotationToTarget(D3DXVECTOR3 target)
{
	D3DXVECTOR3 vDirection = target - m_vPosition;
	D3DXVec3Normalize(&vDirection, &vDirection);
	float targetAngle = atan2f(vDirection.x, vDirection.z);
	D3DXMATRIX rotY;

	D3DXMatrixRotationY(&rotY, m_vRotation.y);
	D3DXVec3TransformCoord(&m_vRotation, &m_vRotation, &rotY);

	m_vRotation.y += (targetAngle - m_vRotation.y) * 0.1f;

	return vDirection;
}


void CBaseEnemy::IdleMove()
{
	if (m_fDistanceFromPlayer < 10.f)
	{
		m_State = Attack;
		return;
	}

	if (m_StepList.empty())
		return;

	m_CurrentCol = m_StepList[m_pathStep].x;
	m_CurrentRow = m_StepList[m_pathStep].y;

	D3DXVECTOR3 vNextStep = CMaze::CellToWorldRC(m_CurrentRow, m_CurrentCol, m_MazeHeight, m_MazeWidth, 3.f, 12);

	if (m_pathStep == 0)
	{
		m_vPosition = vNextStep;
		GetNextStep();
		return;
	}

	D3DXVECTOR3 vDirection = SetRotationToTarget(vNextStep);
	D3DXVECTOR3 diff = vNextStep - m_vPosition;
	float distance = D3DXVec3Length(&diff);

	if (distance > 0.5f)
	{
		m_vPosition += vDirection * moveSpeed;
	}
	else
	{
		m_vPosition = vNextStep;
		GetNextStep();
	}
}

bool CBaseEnemy::IsLitBySpot(const CSpotLight& spotLight) const
{

	D3DXVECTOR3 Lpos = spotLight.GetPosition();
	D3DXVECTOR3 Ldir = spotLight.GetDirection();
	float range = spotLight.GetRange();
	float inner = spotLight.GetInnerAngle();
	float outer = spotLight.GetOuterAngle();
	float intensity = spotLight.GetIntensity();

	if (intensity <= 0.0f)
		return false;


	D3DXVECTOR3 toEnemy = m_vPosition - Lpos;
	float dist = D3DXVec3Length(&toEnemy);
	if (dist > range)
		return false;

	if (D3DXVec3LengthSq(&toEnemy) > 0.f)
	{
		D3DXVec3Normalize(&toEnemy, &toEnemy);
	}
	if (D3DXVec3LengthSq(&Ldir) > 0.f)
	{
		D3DXVec3Normalize(&Ldir, &Ldir);
	}

	float cosTheta = D3DXVec3Dot(&Ldir, &toEnemy);
	// Convert cone half-angle to cosine and compare
	// outer is wider; we consider lit if within outer cone
	float outerCos = cosf(D3DX_PI / 2.0f - outer); // adjust if angles are half-angle; if they are already half-angle, this can be simply cos(outer)
	// Robust approach: compute actual angle with acos
	float angle = acosf(max(-1.0f, min(1.0f, cosTheta)));

	return angle <= outer;

}

void CBaseEnemy::ApplyRetreatAndDamage(float damage, float retreatForce)
{
	if (m_State == Dead)
		return;

	m_Health -= damage;
	if (m_Health <= 0.0f)
	{
		m_Health = 0.0f;
		m_State = Dead;
		return;
	}

	m_State = Retiring;
	m_RetreatTimer = m_RetreatDuration;

	D3DXVECTOR3 away = m_vPosition - m_pPlayer->GetPosition();
	if (D3DXVec3LengthSq(&away) > 0.f)
	{
		D3DXVec3Normalize(&away, &away);
	}
	m_vPosition += away * retreatForce;

}