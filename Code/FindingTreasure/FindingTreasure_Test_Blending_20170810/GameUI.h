#pragma once
#include "Player.h"
#include "Sprite.h"

class CGameUI
{
public:
	CGameObject *m_pVoxelPocketUI;
	CSprite *m_pVoxelIcons;
	CGameObject *m_pVoxelActivated;
	CGameObject *m_pRespawningTextUI;
	CGameObject *m_pRespawningGaugeUI;
	CGameObject *m_pStaminaUI;			// ������ ��
	CGameObject *m_pStaminaGaugeUI;

	CGameUI();
	~CGameUI();
	
	void BuildUIObjects(ID3D11Device *pd3dDevice);
	void BackgroundUIRender(ID3D11DeviceContext *pd3dDeviceContext);
	void PlayerHavingVoxelRender(ID3D11DeviceContext *pd3dDeviceCotext, CPlayer *pPlayer);
	void RespawningRender(ID3D11DeviceContext *pd3dDeviceCotext, CRespawnManager* pRespawnManager, CPlayer *pPlayer);
	void StaminaRender(ID3D11DeviceContext *pd3dDeviceCotext, CPlayer *pPlayer);
};

