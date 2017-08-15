#pragma once
class CTimeManager
{
public:
	CTimeManager();
	~CTimeManager();

private:
	float m_fTimeLimitSec;		// �� ������ �ʱ�ȭ�Ѵ�.

public:
	void SetTimeLimitSec(float fTimeLimitSec);					// ���� �ð��� �����Ѵ�.
	int GetTimeSecRemaining(void);								// ���� �ð��� �����´�. (int������)
	void DecreaseTimeSecRemaining(float fElapsedTime);			// ���� �ð��� ���ҽ�Ų��.
	bool IsTimeOver(void);										// �ð��� �� ��������?
};

