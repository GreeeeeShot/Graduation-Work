#include "stdafx.h"
#include "Timer.h"


CTimer::CTimer()
{
	if (QueryPerformanceFrequency((LARGE_INTEGER *)&m_PerformanceFrequency))	// �ʴ� ���ŵǴ� ���� ī���� ���� �����Ѵ�.
	{
		m_bHardwareHasPerformanceCounter = TRUE;								// ���� ī���͸� ���ϰ� �ִ�.
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nLastTime);					// ������ ī���͸� ��ȯ�޴´�.
		m_fTimeScale = 1.0f / m_PerformanceFrequency;							// ������ �ʸ� ��´�.
	}
	else
	{
		m_bHardwareHasPerformanceCounter = FALSE;
		m_nLastTime = ::timeGetTime();
		m_fTimeScale = 0.001f;
	}

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_FramePerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}



CTimer::~CTimer()
{
}



void CTimer::Tick(float fLockFPS)
{
	// ���� ī���� ������ ����, ���� �ٸ� ���(�޼���)���� �ð��� ��´�.
	if (m_bHardwareHasPerformanceCounter)	
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentTime);
	}
	else
	{
		m_nCurrentTime = ::timeGetTime();
	}

	//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�.
	float fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;

	if (fLockFPS > 0.0f)
	{
		//�� �Լ��� �Ķ����(fLockFPS)�� 0���� ũ�� �� �ð���ŭ ȣ���� �Լ��� ��ٸ��� �Ѵ�.
		while (fTimeElapsed < (1.0f / fLockFPS))
		{
			if (m_bHardwareHasPerformanceCounter)
			{
				QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentTime);
			}
			else
			{
				m_nCurrentTime = ::timeGetTime();
			}
			//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�.
			fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
		}
	}
	// �� �������� �׸� ���� �ð�(1.0 /  fLockFPS)�� �ѱ�� �� while���� �����.
	// fLockFPS ������ �������� �����Ѵ�.

	//���� �ð��� m_nLastTime�� �����Ѵ�.
	m_nLastTime = m_nCurrentTime;

	/* ������ ������ ó�� �ð��� ���� ������ ó�� �ð��� ���̰� 1�ʺ��� ������ ���� ������ ó�� �ð��� m_fFrameTime[0]�� �����Ѵ�. */
	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
	{
		memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;	//���� ������ ó�� �ð�
		if (m_nSampleCount < MAX_SAMPLE_COUNT) m_nSampleCount++;
	}

	//�ʴ� ������ ���� 1 ������Ű�� ���� ������ ó�� �ð��� �����Ͽ� �����Ѵ�.
	m_FramePerSecond++;							// �ش� �Լ��� ȣ��Ǹ� ������ +1 ���ŵȴ�.
	m_fFPSTimeElapsed += fTimeElapsed;			// �ϳ��� �������� �����ϴµ� �ɸ� �ð��� �����Ѵ�.
	if (m_fFPSTimeElapsed > 1.0f) //1���� �ð��� �����ٸ�.
	{
		m_nCurrentFrameRate = m_FramePerSecond; //1�� ���� ������ ������ �� ����, �� 1�ʵ��� m_FramePerSecond(FPS)�� ������ ���̴�.
		m_FramePerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}

	//������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ���Ѵ�.
	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++) m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0) m_fTimeElapsed /= m_nSampleCount;
}

unsigned long CTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� �� FPS���� �����Ѵ�.
	if (lpszString)
	{
		_itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}

	return(m_nCurrentFrameRate);
}

float CTimer::GetTimeElapsed()
{
	return(m_fTimeElapsed);
}