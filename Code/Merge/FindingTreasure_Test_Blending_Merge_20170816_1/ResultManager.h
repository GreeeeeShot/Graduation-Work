#pragma once

enum RESULT_TYPE
{
	RESULT_TYPE_UNKNOWN,				// 아직 모르는
	RESULT_TYPE_DECIDED,				// 둘 중 하나로 정해진
	RESULT_TYPE_DRAW					// 무승부인
};

template <typename TeamType>
class CResultManager
{
private:
	RESULT_TYPE m_eResultType;
	TeamType m_tWinnerType;

public:
	CResultManager();
	~CResultManager();

	void RegisterWinnerType(TeamType tTeamType);		// 등록함으로써 결과 타입이 RESULT_TYPE_DECIDED가 된다.
	void ResultIsDraw(void);							// 호출하면 결과 타입이 RESULT_TYPE_DRAW가 된다.
	void ResetResult(void);								// 호출하면 결과 타입이 RESULT_TYPE_UNKNOWN가 된다.
	bool IsWinner(TeamType tTeamType);					// 승자임을 판단한다. 드로우거나 알 수 없으면 false를 반환한다.
	RESULT_TYPE GetResultType(void);					// 현재의 결과 타입을 가져온다.
	bool IsGameOver(void);								// 현재 게임이 끝났는지 판단한다.
};

template <typename TeamType>
CResultManager<TeamType>::CResultManager()
{
	m_eResultType = RESULT_TYPE_UNKNOWN;
}

template <typename TeamType>
CResultManager<TeamType>::~CResultManager()
{
}

template <typename TeamType>
void CResultManager<TeamType>::RegisterWinnerType(TeamType tTeamType)  // 등록함으로써 결과 타입이 RESULT_TYPE_DECIDED가 된다.
{
	m_eResultType = RESULT_TYPE_DECIDED;
	m_tWinnerType = tTeamType;
}

template <typename TeamType>
void CResultManager<TeamType>::ResultIsDraw(void)							// 호출하면 결과 타입이 RESULT_TYPE_DRAW가 된다.
{
	m_eResultType = RESULT_TYPE_DRAW;
}

template <typename TeamType>
void CResultManager<TeamType>::ResetResult(void)								// 호출하면 결과 타입이 RESULT_TYPE_UNKNOWN가 된다.
{
	m_eResultType = RESULT_TYPE_UNKNOWN;
}

template <typename TeamType>
bool CResultManager<TeamType>::IsWinner(TeamType tTeamType)
{
	switch (m_eResultType)
	{
	case RESULT_TYPE_DRAW:
	case RESULT_TYPE_UNKNOWN:
		return false;
		break;
	case RESULT_TYPE_DECIDED:
		return m_tWinnerType == tTeamType ? true : false;
		break;
	}
}

template <typename TeamType>
RESULT_TYPE  CResultManager<TeamType>::GetResultType(void)
{
	return m_eResultType;
}

template <typename TeamType>
bool CResultManager<TeamType>::IsGameOver(void)
{
	return m_eResultType == RESULT_TYPE_UNKNOWN ? false : true;
}
