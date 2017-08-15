#include "stdafx.h"
#include "RespawnManager.h"


CRespawnManager::CRespawnManager()
{
	for (int i = 0; i < RESPAWN_SLOT_NUM; i++)
	{
		m_RespawnSlot[i].pRegisteredPlayer = NULL;
		m_RespawnSlot[i].iRespawnTime = 0.0f;
		m_RespawnSlot[i].bIsDelayed = true;
	}

	m_iBlueRespawnPointIdx = 0;
	m_iRedRespawnPointIdx = 0;
}


CRespawnManager::~CRespawnManager()
{
}

void CRespawnManager::InitRespawnManager(void)
{
	for (int i = 0; i < RESPAWN_SLOT_NUM; i++)
	{
		if(m_RespawnSlot[i].pRegisteredPlayer) m_RespawnSlot[i].pRegisteredPlayer->SetActive(true);
		m_RespawnSlot[i].pRegisteredPlayer = NULL;
		m_RespawnSlot[i].iRespawnTime = 0.0f;
	}

	m_iBlueRespawnPointIdx = 0;
	m_iRedRespawnPointIdx = 0;
}

// 전제 : 다음의 메서드는 무조건 등록을 성공시킨다.
void CRespawnManager::RegisterRespawnManager(CPlayer *pPlayer, bool bIsDelayed)
{
	for (int i = 0; i < RESPAWN_SLOT_NUM; i++)
	{
		if (!m_RespawnSlot[i].pRegisteredPlayer)
		{
			m_RespawnSlot[i].pRegisteredPlayer = pPlayer;
			m_RespawnSlot[i].bIsDelayed = bIsDelayed;
			pPlayer->SetActive(false);
			return;
		}
	}
}

void CRespawnManager::UpdateRespawnManager(float fTimeElapsed)
{
	int bIsRespawnCpl = false;
	for (int i = 0; i < RESPAWN_SLOT_NUM; i++)
	{
		bIsRespawnCpl = false;

		if (m_RespawnSlot[i].bIsDelayed)
		{
			//printf("%.4f \n", m_RespawnSlot[i].iRespawnTime);
			if (m_RespawnSlot[i].pRegisteredPlayer && (m_RespawnSlot[i].iRespawnTime += fTimeElapsed) >= RESPAWN_COMPLETE_TIME)
			{
				bIsRespawnCpl = true;
			}
		}
		else
		{
			bIsRespawnCpl = true;
		}

		if (bIsRespawnCpl)
		{
			//printf("부활!\n");
			if (m_RespawnSlot[i].pRegisteredPlayer)
			{
				switch (m_RespawnSlot[i].pRegisteredPlayer->m_BelongType)
				{
				case BELONG_TYPE_BLUE:
					m_RespawnSlot[i].pRegisteredPlayer->SetPosition(m_d3dxvBlueRespawnPoints[m_iBlueRespawnPointIdx]);
					m_iBlueRespawnPointIdx = (m_iBlueRespawnPointIdx + 1) % 4;
					m_RespawnSlot[i].pRegisteredPlayer->m_d3dxvVelocity = D3DXVECTOR3(0.f, 0.f, 0.f);
					break;
				case BELONG_TYPE_RED:
					m_RespawnSlot[i].pRegisteredPlayer->SetPosition(m_d3dxvRedRespawnPoints[m_iRedRespawnPointIdx]);
					m_iRedRespawnPointIdx = (m_iRedRespawnPointIdx + 1) % 4;
					m_RespawnSlot[i].pRegisteredPlayer->m_d3dxvVelocity = D3DXVECTOR3(0.f, 0.f, 0.f);
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
}

float CRespawnManager::GetRespawnTimeRemaining(CPlayer *pPlayer)
{
	float fRespawnTimeRemaining = 0.0f;
	for (int i = 0; i < RESPAWN_SLOT_NUM; i++)
	{
		if (m_RespawnSlot[i].pRegisteredPlayer == pPlayer)
		{
			return RESPAWN_COMPLETE_TIME - m_RespawnSlot[i].iRespawnTime;
		}
	}
}