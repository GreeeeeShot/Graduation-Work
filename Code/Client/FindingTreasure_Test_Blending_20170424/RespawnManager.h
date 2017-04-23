#pragma once

#define RESPAWN_SLOT_NUM		9			// 최대 접속자 수 + 보물 상자
#define RESPAWN_COMPLETE_TIME	3.0f		// 

struct RESPAWN_SLOT
{
	CPlayer *pRegisteredPlayer;
	float iRespawnTime;			
};

struct RESPAWN_POINT
{
	D3DXVECTOR3 pos;				
};

struct RESPAWN_RANGE
{
	AABB m_AABB;
};

class CRespawnManager
{
public:
	RESPAWN_SLOT m_RespawnSlot[RESPAWN_SLOT_NUM];

	void InitRespawnManager(void);
	void RegisterRespawnManager(CPlayer *pPlayer);
	void UpdateRespawnManager(float fTimeElapsed);
	
	CRespawnManager();
	~CRespawnManager();
};

