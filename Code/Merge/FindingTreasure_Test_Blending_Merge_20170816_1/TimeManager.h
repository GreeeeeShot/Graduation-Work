#pragma once
class CTimeManager
{
public:
	CTimeManager();
	~CTimeManager();

private:
	float m_fTimeLimitSec;		// 초 단위로 초기화한다.

public:
	void SetTimeLimitSec(float fTimeLimitSec);					// 제한 시간을 세팅한다.
	int GetTimeSecRemaining(void);								// 남은 시간을 가져온다. (int형으로)
	void DecreaseTimeSecRemaining(float fElapsedTime);			// 남은 시간을 감소시킨다.
	bool IsTimeOver(void);										// 시간이 다 지났는지?
};

