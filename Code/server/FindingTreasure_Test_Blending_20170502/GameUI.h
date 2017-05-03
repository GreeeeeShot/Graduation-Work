#pragma once
#include "Player.h"
#include "Sprite.h"

class CGameUI
{
public:
	CCamera *m_pCamera;
	CGameObject *m_pVoxelPocketUI;
	CSprite *m_pVoxelIcons;

	CGameUI();
	~CGameUI();
	
	void SetCamera(CCamera *pCamera);
	void BuildUIObjects(ID3D11Device *pd3dDevice);
	void BackgroundUIRender(ID3D11DeviceContext *pd3dDeviceContext);
	void PlayerHavingVoxelRender(ID3D11DeviceContext *pd3dDeviceCotext, CPlayer *pPlayer);
};

