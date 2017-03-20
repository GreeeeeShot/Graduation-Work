#include "stdafx.h"
#include "Timer.h"


CTimer::CTimer()
{
	if (QueryPerformanceFrequency((LARGE_INTEGER *)&m_PerformanceFrequency))	// 초당 갱신되는 고정 카운터 수를 저장한다.
	{
		m_bHardwareHasPerformanceCounter = TRUE;								// 성능 카운터를 지니고 있다.
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nLastTime);					// 현재의 카운터를 반환받는다.
		m_fTimeScale = 1.0f / m_PerformanceFrequency;							// 개수당 초를 얻는다.
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
	// 성능 카운터 유무에 따라, 각기 다른 방식(메서드)으로 시간을 얻는다.
	if (m_bHardwareHasPerformanceCounter)	
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentTime);
	}
	else
	{
		m_nCurrentTime = ::timeGetTime();
	}

	//마지막으로 이 함수를 호출한 이후 경과한 시간을 계산한다.
	float fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;

	if (fLockFPS > 0.0f)
	{
		//이 함수의 파라메터(fLockFPS)가 0보다 크면 이 시간만큼 호출한 함수를 기다리게 한다.
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
			//마지막으로 이 함수를 호출한 이후 경과한 시간을 계산한다.
			fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
		}
	}
	// 한 프레임을 그릴 고정 시간(1.0 /  fLockFPS)을 넘기면 위 while문을 벗어난다.
	// fLockFPS 이하의 프레임을 유지한다.

	//현재 시간을 m_nLastTime에 저장한다.
	m_nLastTime = m_nCurrentTime;

	/* 마지막 프레임 처리 시간과 현재 프레임 처리 시간의 차이가 1초보다 작으면 현재 프레임 처리 시간을 m_fFrameTime[0]에 저장한다. */
	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
	{
		memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;	//현재 프레임 처리 시간
		if (m_nSampleCount < MAX_SAMPLE_COUNT) m_nSampleCount++;
	}

	//초당 프레임 수를 1 증가시키고 현재 프레임 처리 시간을 누적하여 저장한다.
	m_FramePerSecond++;							// 해당 함수가 호출되면 무조건 +1 갱신된다.
	m_fFPSTimeElapsed += fTimeElapsed;			// 하나의 프레임을 갱신하는데 걸린 시간을 누적한다.
	if (m_fFPSTimeElapsed > 1.0f) //1초의 시간이 지났다면.
	{
		m_nCurrentFrameRate = m_FramePerSecond; //1초 동안 누적된 프레임 수 저장, 즉 1초동안 m_FramePerSecond(FPS)를 갱신한 것이다.
		m_FramePerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}

	//누적된 프레임 처리 시간의 평균을 구하여 프레임 처리 시간을 구한다.
	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++) m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0) m_fTimeElapsed /= m_nSampleCount;
}

unsigned long CTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	//현재 프레임 레이트를 문자열로 변환하여 lpszString 버퍼에 쓰고 “ FPS”와 결합한다.
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
