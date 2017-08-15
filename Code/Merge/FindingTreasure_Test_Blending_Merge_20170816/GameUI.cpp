#include "stdafx.h"
#include "GameUI.h"
#include "ShaderResource.h"


CGameUI::CGameUI()
{
	m_pVoxelPocketUI = NULL;
	m_pVoxelIcons = NULL;
	m_pVoxelActivated = NULL;
	m_pRespawningTextUI = NULL;
	m_pRespawningGaugeUI = NULL;
	m_pStaminaUI = NULL;	
	m_pStaminaGaugeUI = NULL;
	m_pNumbers = NULL;
	m_pKeyboardUI = NULL;
	m_bIsKeyboardActivated = false;
}


CGameUI::~CGameUI()
{
	if (m_pVoxelPocketUI) delete m_pVoxelPocketUI;
	if (m_pVoxelIcons) delete m_pVoxelIcons;
	if (m_pVoxelActivated) delete m_pVoxelActivated;
	if (m_pRespawningTextUI) delete m_pRespawningTextUI;
	if (m_pRespawningGaugeUI) delete m_pRespawningGaugeUI;
	if (m_pStaminaUI) delete m_pStaminaUI;
	if (m_pStaminaGaugeUI) delete m_pStaminaGaugeUI;
	if (m_pNumbers) delete m_pNumbers;
	if (m_pKeyboardUI) delete m_pKeyboardUI;
}

void CGameUI::BuildUIObjects(ID3D11Device *pd3dDevice)
{
	m_pVoxelPocketUI = new CGameObject();
	m_pVoxelPocketUI->SetMesh(CMeshResource::pUI_VoxelPocketMesh);
	m_pVoxelPocketUI->SetTexture(CTextureResource::pUI_VoxelPocket);
	m_pVoxelPocketUI->SetShader(CShaderResource::pUITexturedShader);
	m_pVoxelPocketUI->SetPosition(0.0f, -240.0f, +0.0f);

	m_pVoxelIcons = new CSprite();
	m_pVoxelIcons->SetSpriteTexture(CTextureResource::pUI_VoxelIcons);
	m_pVoxelIcons->SetShader(CShaderResource::pUITexturedShader);
	m_pVoxelIcons->BuildSpritePieces(pd3dDevice, 128.0f, 128.0f, 17);

	m_pVoxelActivated = new CGameObject();
	m_pVoxelActivated->SetMesh(CMeshResource::pUI_VoxelActivatedMesh);
	m_pVoxelActivated->SetTexture(CTextureResource::pUI_VoxelActivated);
	m_pVoxelActivated->SetShader(CShaderResource::pUITexturedShader);
	m_pVoxelActivated->SetPosition(0.0f, -240.0f, 0.0f);	// => 형식 상 존재

	m_pRespawningTextUI = new CGameObject();
	m_pRespawningTextUI->SetMesh(CMeshResource::pUI_RespawningTextMesh);
	m_pRespawningTextUI->SetTexture(CTextureResource::pUI_Respawning);
	m_pRespawningTextUI->SetShader(CShaderResource::pUITexturedShader);
	m_pRespawningTextUI->SetPosition(0.0f, -0.0f, +0.0f);

	m_pRespawningGaugeUI = new CGameObject();
	m_pRespawningGaugeUI->SetMesh(CMeshResource::pUI_RespawnigGaugeMesh);
	m_pRespawningGaugeUI->SetTexture(CTextureResource::pUI_Respawning);
	m_pRespawningGaugeUI->SetShader(CShaderResource::pUITexturedShader);
	m_pRespawningGaugeUI->SetPosition(0.0f, -20.0f, 0.0f);

	m_pStaminaUI = new CGameObject();
	m_pStaminaUI->SetMesh(CMeshResource::pUI_StaminaMesh);
	m_pStaminaUI->SetTexture(CTextureResource::pUI_Stamina);
	m_pStaminaUI->SetShader(CShaderResource::pUITexturedShader);
	m_pStaminaUI->SetPosition(0.0f, -160.0f, 0.0f);

	m_pStaminaGaugeUI = new CGameObject();
	m_pStaminaGaugeUI->SetMesh(CMeshResource::pUI_StaminaGaugeMesh);
	m_pStaminaGaugeUI->SetTexture(CTextureResource::pUI_Stamina);
	m_pStaminaGaugeUI->SetShader(CShaderResource::pUITexturedShader);
	m_pStaminaGaugeUI->SetPosition(0.0f, -160.5f, 0.0f);

	m_pNumbers = new CSprite();
	m_pNumbers->SetSpriteTexture(CTextureResource::pUI_Numbers);
	m_pNumbers->SetShader(CShaderResource::pUITexturedShader);
	m_pNumbers->BuildSpritePieces(pd3dDevice, 33.0f, 55.0f, 11);

	m_pKeyboardUI = new CGameObject();
	m_pKeyboardUI->SetMesh(CMeshResource::pUI_KeyboardMesh);
	m_pKeyboardUI->SetTexture(CTextureResource::pUI_Keyboard);
	m_pKeyboardUI->SetShader(CShaderResource::pUITexturedShader);
	m_pKeyboardUI->SetPosition(0.0f, +75.0f, +0.0f);
}

void CGameUI::BackgroundUIRender(ID3D11DeviceContext*pd3dDeviceContext)
{
	if (m_pVoxelPocketUI) m_pVoxelPocketUI->Render(pd3dDeviceContext);
}

void CGameUI::PlayerHavingVoxelRender(ID3D11DeviceContext *pd3dDeviceCotext, CPlayer *pPlayer)
{
	m_pVoxelActivated->SetPosition(-128.0f * 2.0f + 128.0f * pPlayer->m_iVoxelPocketSlotIdx, -240.0f, 0.0f);
	m_pVoxelActivated->Render(pd3dDeviceCotext);

	for (int i = 0; i < 5; i++)
	{
		if (pPlayer->m_VoxelPocket[i].m_bIsActive)
		{
			m_pVoxelIcons->GetSpritePieceByIndex(pPlayer->m_VoxelPocket[i].m_pVoxel->m_eVoxelType)->SetPosition(-128.0f * 2.0f + 128.0f*i, -240.0f, 0.0f);
			m_pVoxelIcons->GetSpritePieceByIndex(pPlayer->m_VoxelPocket[i].m_pVoxel->m_eVoxelType)->Render(pd3dDeviceCotext);
		}
	}
}

void CGameUI::RespawningRender(ID3D11DeviceContext *pd3dDeviceCotext, CRespawnManager* pRespawnManager, CPlayer *pPlayer)
{
	m_pRespawningTextUI->Render(pd3dDeviceCotext);

	m_pRespawningGaugeUI->m_pMesh->Animate(pd3dDeviceCotext, 1.0f - pRespawnManager->GetRespawnTimeRemaining(pPlayer) / RESPAWN_COMPLETE_TIME);
	m_pRespawningGaugeUI->Render(pd3dDeviceCotext);
}

void CGameUI::StaminaRender(ID3D11DeviceContext *pd3dDeviceCotext, CPlayer *pPlayer)
{
	m_pStaminaUI->Render(pd3dDeviceCotext);

	m_pStaminaGaugeUI->m_pMesh->Animate(pd3dDeviceCotext, pPlayer->m_fCurStamina / PLAYER_MAX_STAMINA);
	m_pStaminaGaugeUI->Render(pd3dDeviceCotext);
}

void CGameUI::TimerRender(ID3D11DeviceContext *pd3dDeviceCotext, int iTimeForSec)
{
	if (iTimeForSec < 0) iTimeForSec = 0;

	int iMin = iTimeForSec / 60;
	int iSec = iTimeForSec % 60;

	// 분의 십의 자리
	m_pNumbers->GetSpritePieceByIndex(iMin / 10)->SetPosition(-66.0f, 320.0f, 0.0f);
	m_pNumbers->GetSpritePieceByIndex(iMin / 10)->Render(pd3dDeviceCotext);

	// 분의 일의 자리
	m_pNumbers->GetSpritePieceByIndex(iMin % 10)->SetPosition(-33.0f, 320.0f, 0.0f);
	m_pNumbers->GetSpritePieceByIndex(iMin % 10)->Render(pd3dDeviceCotext);

	// :
	m_pNumbers->GetSpritePieceByIndex(10)->SetPosition(0.0f, 320.0f, 0.0f);
	m_pNumbers->GetSpritePieceByIndex(10)->Render(pd3dDeviceCotext);

	m_pNumbers->GetSpritePieceByIndex(iSec / 10)->SetPosition(33.0f, 320.0f, 0.0f);
	m_pNumbers->GetSpritePieceByIndex(iSec / 10)->Render(pd3dDeviceCotext);

	m_pNumbers->GetSpritePieceByIndex(iSec % 10)->SetPosition(66.0f, 320.0f, 0.0f);
	m_pNumbers->GetSpritePieceByIndex(iSec % 10)->Render(pd3dDeviceCotext);
}

void CGameUI::ONAndOffKeyboardUI(void)
{
	m_bIsKeyboardActivated = m_bIsKeyboardActivated ? false : true;
}

void CGameUI::KeyboardRender(ID3D11DeviceContext *pd3dDeviceCotext)
{
	if (m_pKeyboardUI && m_bIsKeyboardActivated) m_pKeyboardUI->Render(pd3dDeviceCotext);
}