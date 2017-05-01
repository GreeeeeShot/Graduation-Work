#pragma once

#define RESPAWN_SLOT_NUM		9			// 최대 접속자 수 + 보물 상자
#define RESPAWN_COMPLETE_TIME	3.0f		// 

struct RESPAWN_SLOT
{
	CPlayer *pRegisteredPlayer;
	float iRespawnTime;		
	bool bIsDelayed;
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
private:
	D3DXVECTOR3 m_d3dxvBlueRespawnPoints[4] = 
	{ D3DXVECTOR3(-43.0f, 5.0f, 1.0f), D3DXVECTOR3(-41.0f, 5.0f, 1.0f), 
		D3DXVECTOR3(-43.0f, 5.0f, -1.0f), D3DXVECTOR3(-41.0f, 5.0f, -1.0f) };
	D3DXVECTOR3 m_d3dxvRedRespawnPoints[4] = 
	{ D3DXVECTOR3(41.0f, 5.0f, 1.0f), D3DXVECTOR3(43.0f, 5.0f, 1.0f),
		D3DXVECTOR3(41.0f, 5.0f, -1.0f), D3DXVECTOR3(43.0f, 5.0f, -1.0f) };

public:
	int m_iBlueRespawnPointIdx;
	int m_iRedRespawnPointIdx;
	RESPAWN_SLOT m_RespawnSlot[RESPAWN_SLOT_NUM];

	void InitRespawnManager(void);
	void RegisterRespawnManager(CPlayer *pPlayer, bool bIsDelayed);
	void UpdateRespawnManager(float fTimeElapsed);
	
	CRespawnManager();
	~CRespawnManager();
};

