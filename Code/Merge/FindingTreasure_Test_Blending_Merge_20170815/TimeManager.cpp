#include "stdafx.h"
#include "TimeManager.h"


CTimeManager::CTimeManager()
{
	m_fTimeLimitSec = 0;
}


CTimeManager::~CTimeManager()
{
}

void CTimeManager::SetTimeLimitSec(float fTimeLimitSec)					// ���� �ð��� �����Ѵ�.
{
	if (fTimeLimitSec <= 0.0f)
	{
		m_fTimeLimitSec = 0.0f;
		return;
	}

	m_fTimeLimitSec = fTimeLimitSec;
}

int CTimeManager::GetTimeSecRemaining(void)								// ���� �ð��� �����´�. (int������)
{
	return m_fTimeLimitSec;
}

void CTimeManager::DecreaseTimeSecRemaining(float fElapsedTime)			// ���� �ð��� ���ҽ�Ų��.
{
	if (fElapsedTime < 0.0f) return;		// fElapsedTime�� ����� ���� ��ȿ�ϴ�.

	m_fTimeLimitSec -= fElapsedTime;

	if (m_fTimeLimitSec < 0.0f) m_fTimeLimitSec = 0.0f;
}

bool CTimeManager::IsTimeOver(void)										// �ð��� �� ��������?
{
	return m_fTimeLimitSec <= 0.0f ? true : false;
}