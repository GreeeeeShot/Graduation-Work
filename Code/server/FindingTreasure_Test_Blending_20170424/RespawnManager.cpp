#include "stdafx.h"
#include "RespawnManager.h"


CRespawnManager::CRespawnManager()
{
	for (int i = 0; i < RESPAWN_SLOT_NUM; i++)
	{
		m_RespawnSlot[i].pRegisteredPlayer = NULL;
		m_RespawnSlot[i].iRespawnTime = 0.0f;
	}
}


CRespawnManager::~CRespawnManager()
{
}

void CRespawnManager::InitRespawnManager(void)
{
	for (int i = 0; i < RESPAWN_SLOT_NUM; i++)
	{
		m_RespawnSlot[i].pRegisteredPlayer = NULL;
		m_RespawnSlot[i].iRespawnTime = 0.0f;
	}
}

// 전제 : 다음의 메서드는 무조건 등록을 성공시킨다.
void CRespawnManager::RegisterRespawnManager(CPlayer *pPlayer)
{
	for (int i = 0; i < RESPAWN_SLOT_NUM; i++)
	{
		if (!m_RespawnSlot[i].pRegisteredPlayer)
		{
			m_RespawnSlot[i].pRegisteredPlayer = pPlayer;
			pPlayer->SetActive(false);
			return;
		}
	}
}

void CRespawnManager::UpdateRespawnManager(float fTimeElapsed)
{
	for (int i = 0; i < RESPAWN_SLOT_NUM; i++)
	{
		//printf("%.4f \n", m_RespawnSlot[i].iRespawnTime);
		if (m_RespawnSlot[i].pRegisteredPlayer && (m_RespawnSlot[i].iRespawnTime += fTimeElapsed) >= RESPAWN_COMPLETE_TIME)
		{
			//printf("부활!\n");
			switch (m_RespawnSlot[i].pRegisteredPlayer->m_BelongType)
			{
			case BELONG_TYPE_BLUE:
				m_RespawnSlot[i].pRegisteredPlayer->SetPosition(-42.0f, 2.5f, 0.0f);
				break;
			case BELONG_TYPE_RED:
				m_RespawnSlot[i].pRegisteredPlayer->SetPosition(42.0f, 2.5f, 0.0f);
				break;
			case BELONG_TYPE_INDIVIDUAL:
				m_RespawnSlot[i].pRegisteredPlayer->SetPosition(0.0f, 10.0f, 0.0f);
				break;
			}
			m_RespawnSlot[i].pRegisteredPlayer->m_CameraOperator.InitCameraOperator(m_RespawnSlot[i].pRegisteredPlayer);
			m_RespawnSlot[i].pRegisteredPlayer->SetActive(true);
			m_RespawnSlot[i].pRegisteredPlayer = NULL;
			m_RespawnSlot[i].iRespawnTime = 0;
		}
	}
}