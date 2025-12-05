#include "CTime.h"

CTime::CTime()
	: deltaTime				(0)
	, m_totalTime			(0)
	, m_fixedStepDuration	(0)
	, m_timeAccumulator		(0)
	, m_timerElapsed		(0)
	, m_lastTime			()
	, m_qpFreq				()
	, m_framePerSec			(0)
{
}

CTime::~CTime()
{
}

bool CTime::Init(double fps)
{
	if (!QueryPerformanceFrequency(&m_qpFreq)) {
		return false;
	}

	m_fixedStepDuration = 1000.0/fps;

	QueryPerformanceCounter(&m_lastTime);
	return true;
}

bool CTime::FixedTick(double& fixedDeltaTime)
{
	if (m_timeAccumulator >= m_fixedStepDuration)
	{
		m_framePerSec /= 1000.f;
		m_timeAccumulator = 0;
		fixedDeltaTime = m_fixedStepDuration;
		return true;
	}

	return false;
}

void CTime::Tick()
{
	m_framePerSec++;
	VariableTick();
}

void CTime::VariableTick()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	double elapsedMs = (double)(now.QuadPart - m_lastTime.QuadPart) * 1000.0 / m_qpFreq.QuadPart;

	deltaTime = (float)elapsedMs;
	m_totalTime += elapsedMs;

	m_timerElapsed += elapsedMs;

	m_timeAccumulator += elapsedMs;

	m_lastTime = now;
}

void CTime::StartTimer()
{
	m_timerElapsed = m_totalTime;
}

double CTime::StopTimer()
{
	m_timerElapsed = 0;
	return m_totalTime - m_timerElapsed;
}
