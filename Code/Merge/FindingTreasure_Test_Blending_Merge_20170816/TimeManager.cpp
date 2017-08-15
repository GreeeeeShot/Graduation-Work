#include "stdafx.h"
#include "TimeManager.h"


CTimeManager::CTimeManager()
{
	m_fTimeLimitSec = 0;
}


CTimeManager::~CTimeManager()
{
}

void CTimeManager::SetTimeLimitSec(float fTimeLimitSec)					// 제한 시간을 세팅한다.
{
	if (fTimeLimitSec <= 0.0f)
	{
		m_fTimeLimitSec = 0.0f;
		return;
	}

	m_fTimeLimitSec = fTimeLimitSec;
}

int CTimeManager::GetTimeSecRemaining(void)								// 남은 시간을 가져온다. (int형으로)
{
	return m_fTimeLimitSec;
}

void CTimeManager::DecreaseTimeSecRemaining(float fElapsedTime)			// 남은 시간을 감소시킨다.
{
	if (fElapsedTime < 0.0f) return;		// fElapsedTime이 양수일 때만 유효하다.

	m_fTimeLimitSec -= fElapsedTime;

	if (m_fTimeLimitSec < 0.0f) m_fTimeLimitSec = 0.0f;
}

bool CTimeManager::IsTimeOver(void)										// 시간이 다 지났는지?
{
	return m_fTimeLimitSec <= 0.0f ? true : false;
}