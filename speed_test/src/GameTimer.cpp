#include "GameTimer.h"
#include "common.h"

GameTimer::GameTimer(float fTarget, TimerType type, TimerCallback pCall, void* pData)
	: 
  m_pCallBack(pCall)
  ,m_pCallBackData(pData)
  ,m_fElapsed(0.0f)
  ,m_fTarget(fTarget)
  ,m_type(type)

{
	KASSERT(m_fTarget > 0.0f, "invalid target type\n");
}
GameTimer::GameTimer()
	: 
   m_pCallBack(NULL)
  ,m_pCallBackData(NULL)
  ,m_fElapsed(0.0f)
  ,m_fTarget(0.0f)
  ,m_type(STOP)


{
}

GameTimer::~GameTimer()
{
}

void GameTimer::Init(float fTarget, TimerType type, TimerCallback pCall, void* pData)
{
	m_type = type;

	//KASSERT(!m_pCallBack, "callbacknot called!\n" );
	m_pCallBack = pCall;
	m_fElapsed = 0.0f;
	m_fTarget = fTarget;
	m_pCallBackData = pData;
}

void GameTimer::Update(float fDelta)
{
	if (m_type == STOP)
		return;

	m_fElapsed += fDelta;
	if (m_pCallBack && m_fElapsed >= m_fTarget )
	{
		if (m_type == LOOP)
		{
			m_fElapsed = 0.0;
		}
		else
		{
			m_type = STOP;
		}

		m_pCallBack(m_pCallBackData);
	}
}

bool GameTimer::IsStopped()
{
	return m_type == STOP;
}

void GameTimer::Restart(TimerType type)
{
	KASSERT(type != STOP, "invalid clock type\n");
	m_fElapsed = 0.0;
	m_type = type;
}

void GameTimer::SetCallback( TimerCallback pCall  ,void *data)
{
	KASSERT(m_pCallBack == NULL, "timer still in execution");
	m_pCallBack = pCall;
	m_pCallBackData = data;
}


float GameTimer::GetRemainingTime() const
{
	return m_fTarget - m_fElapsed;
}
