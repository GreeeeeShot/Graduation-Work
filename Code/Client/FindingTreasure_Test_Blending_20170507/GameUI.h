#pragma once
#include "Player.h"
#include "Sprite.h"

class CGameUI
{
public:
	CCamera *m_pCamera;
	CGameObject *m_pVoxelPocketUI;
	CSprite *m_pVoxelIcons;
	CGameObject *m_pRespawningTextUI;
	CGameObject *m_pRespawningGaugeUI;
	//CPlayer *pMyPlayer;

	CGameUI();
	~CGameUI();
	
	void SetCamera(CCamera *pCamera);
	//void SetMyPlayer(CPlayer *pMyPlayer);
	void BuildUIObjects(ID3D11Device *pd3dDevice);
	void BackgroundUIRender(ID3D11DeviceContext *pd3dDeviceContext);
	void PlayerHavingVoxelRender(ID3D11DeviceContext *pd3dDeviceCotext, CPlayer *pPlayer);
	void RespawningRender(ID3D11DeviceContext *pd3dDeviceCotext, CRespawnManager* pRespawnManager, CPlayer *pPlayer);
};

