#pragma once

enum RESULT_TYPE
{
	RESULT_TYPE_UNKNOWN,				// ���� �𸣴�
	RESULT_TYPE_DECIDED,				// �� �� �ϳ��� ������
	RESULT_TYPE_DRAW					// ���º���
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

	void RegisterWinnerType(TeamType tTeamType);		// ��������ν� ��� Ÿ���� RESULT_TYPE_DECIDED�� �ȴ�.
	void ResultIsDraw(void);							// ȣ���ϸ� ��� Ÿ���� RESULT_TYPE_DRAW�� �ȴ�.
	void ResetResult(void);								// ȣ���ϸ� ��� Ÿ���� RESULT_TYPE_UNKNOWN�� �ȴ�.
	bool IsWinner(TeamType tTeamType);					// �������� �Ǵ��Ѵ�. ��ο�ų� �� �� ������ false�� ��ȯ�Ѵ�.
	RESULT_TYPE GetResultType(void);					// ������ ��� Ÿ���� �����´�.
	bool IsGameOver(void);								// ���� ������ �������� �Ǵ��Ѵ�.
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
void CResultManager<TeamType>::RegisterWinnerType(TeamType tTeamType)  // ��������ν� ��� Ÿ���� RESULT_TYPE_DECIDED�� �ȴ�.
{
	m_eResultType = RESULT_TYPE_DECIDED;
	m_tWinnerType = tTeamType;
}

template <typename TeamType>
void CResultManager<TeamType>::ResultIsDraw(void)							// ȣ���ϸ� ��� Ÿ���� RESULT_TYPE_DRAW�� �ȴ�.
{
	m_eResultType = RESULT_TYPE_DRAW;
}

template <typename TeamType>
void CResultManager<TeamType>::ResetResult(void)								// ȣ���ϸ� ��� Ÿ���� RESULT_TYPE_UNKNOWN�� �ȴ�.
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
