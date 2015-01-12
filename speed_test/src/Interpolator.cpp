# include "Interpolator.h"
#include "Utils.h"
Interpolator::Interpolator()
	:
m_initialized(false)
,m_launched (false)
,m_terminated(false)
,m_startTimePassed(false)
,m_loopCount(1)
,m_loopDirection(LOOP_FORWARD_DIRECTION)
,m_life(0.0f)
,m_duration(0.0f)
,m_pFunction(NULL)
,m_target(0.0f)
,m_delta(0.0f)
,m_pValue(NULL)
,m_pOnFinished(NULL)
, m_pData(NULL)
,m_startTime(0.0f)
,m_animationMode(false)
,m_goBack(false)
,m_oldDuration(0.0f)
,m_oldValue(0.0f)
,m_oldTarget (0.0f)
{
}

Interpolator::~Interpolator()
{

}

bool Interpolator::Init(float startTime,
	float duration,
	InterpolatorFunction pFunction,
	float target,
	float *pValue, 
	InterpolatorCallback pOnFinished,
	void *pData,
	int loopCount, 
	LoopDirections loopDirection, 
	bool animationMode)
{
	if (startTime < 0 || duration < 0 || !pFunction || !pValue || loopCount < 0 ||
		(animationMode && (target < 0 || *pValue < 0)))
	{
		return false;
	}

	if (loopDirection == LOOP_BOUNCE_DIRECTION)
	{
		if (animationMode)
		{
			if (target > 1)
			{
				target = target * 2.0f + *pValue - 2.0f;
			}
			else
			{
				target = *pValue;
			}
		}
		else
		{
			target = (target - *pValue) * 2.0f + *pValue;
		}
	}
	else if (animationMode)
	{
		target += *pValue;
	}

	m_initialized = true;
	m_launched = false;
	m_terminated = false;
	m_startTimePassed = false;

	m_loopCount = loopCount;
	m_loopDirection = loopDirection;
	if (duration > 0)
	{
		if (loopDirection == LOOP_BOUNCE_DIRECTION)
			duration *=2.0;
		m_life = (float)duration;

			m_duration = 1.0f / (float)duration;
		m_Startlife = m_life;
	}
	else
	{
		m_life = 0.0f;
		m_duration = 0.0f;
	}
	m_pFunction = pFunction;
	m_target = target;
	m_pValue = pValue;
	m_pOnFinished = pOnFinished;
	m_pData = pData;
	m_startTime = startTime;
	m_animationMode = animationMode;
	m_goBack = false;
	m_oldDuration = duration;
	m_oldValue = *pValue;
	m_oldTarget = target;

	return true;
}

void Interpolator::Term()
{
	if (m_initialized && m_launched && !m_terminated)
	{
		m_terminated = true;

		m_life = 0.0f;
		if (!m_animationMode && m_pValue)
		{
			*m_pValue = m_target;
		}

		if (m_pOnFinished)
		{
			(*m_pOnFinished)(m_pData);
		}
	}
}

bool Interpolator::Launch()
{
	if (m_initialized && !m_launched && !m_terminated)
	{
		m_launched = true;

		m_startTime = speed::misc::GetTime();

		return true;
	}
	else
	{
		return false;
	}
}

void Interpolator::Update(float delta)
{
	float tempValue;

	if (!m_initialized || !m_launched || m_terminated || speed::misc::GetTime() < m_startTime)
	{
		return;
	}

	if (!m_startTimePassed)
	{
		m_startTimePassed = true;

		m_delta = (*m_pValue - m_target);
	}

	m_life -= delta;

	if (m_life > 0.0f)
	{
		if (m_pValue)
		{
			*m_pValue = (*m_pFunction)(m_life, m_duration, m_target, m_delta);

			if (m_loopDirection == LOOP_BOUNCE_DIRECTION && !m_goBack &&
				((m_animationMode && *m_pValue >= (m_oldTarget - m_oldValue) / 2.0 + 0.5 + m_oldValue) ||
				(!m_animationMode && m_life < m_Startlife * 0.5) ) )
			{
				m_goBack = true;

				tempValue = m_oldValue;
				m_oldValue = m_oldTarget;
				m_oldTarget = tempValue;

				if (m_animationMode)
				{
					if (m_oldTarget < m_oldValue)
					{
						m_target = m_oldTarget + 1.0f;
					}
					else
					{
						m_target = m_oldTarget - 1.0f;
					}
				}
				else
				{
					m_target = m_oldTarget;
				}

				m_delta = -m_delta;
			}
		}
	}
	else
	{
		if (m_loopCount > 1 || m_loopCount == 0)
		{
			if (m_loopCount > 1)
			{
				m_loopCount --;
			}

			m_life = m_oldDuration;
			m_duration = 1.0f / m_oldDuration;

			if (m_loopDirection == LOOP_FORWARD_DIRECTION)
			{
				*m_pValue = m_oldValue;
			}
			else
			{
				m_goBack = false;
				tempValue = m_oldValue;
				m_oldValue = m_oldTarget;
				m_oldTarget = tempValue;
				m_target = m_oldTarget;
				m_delta = m_oldValue - m_oldTarget;
			}
		}
		else
		{
			Term();
		}
	}
}

bool Interpolator::IsInitialized()
{
	return m_initialized;
}

bool Interpolator::IsLaunched()
{
	return m_launched;
}

bool Interpolator::IsTerminated()
{
	return m_terminated;
}