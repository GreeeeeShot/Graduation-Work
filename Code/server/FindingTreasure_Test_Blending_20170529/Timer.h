#pragma once

const ULONG MAX_SAMPLE_COUNT = 50; // 50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.

class CTimer
{
public:
	CTimer();
	virtual ~CTimer();

	void Tick(float fLockFPS = 0.0f);												// Ÿ�̸��� �ð��� �����Ѵ�.
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);		// ������ ����Ʈ�� ��ȯ�Ѵ�.
	float GetTimeElapsed();															// �������� ��� ��� �ð��� ��ȯ�Ѵ�.

private:
	bool m_bHardwareHasPerformanceCounter;				// ��ǻ�Ͱ� ���� ī���͸� ������ �ִ°�?
	float m_fTimeScale;									// Scale Counter�� ��
	float m_fTimeElapsed;								// ������ ������ ���� ������ �ð� (������ ��� ������ �ð��� ����ȴ�.)
	__int64 m_nCurrentTime;								// ������ �ð�
	__int64 m_nLastTime;								// ������ �������� �ð�
	__int64 m_PerformanceFrequency;						// ��ǻ���� ���� ���ļ�

	float m_fFrameTime[MAX_SAMPLE_COUNT];				// ������ �ð��� �����ϱ� ���� �迭
	ULONG m_nSampleCount;								// ������ ������ Ƚ��

	unsigned long m_nCurrentFrameRate;					// ������ ������ ����Ʈ
	unsigned long m_FramePerSecond;						// �ʴ� ������ ��
	float m_fFPSTimeElapsed;							// ������ ����Ʈ ��� �ҿ� �ð�
};

