#include "stdafx.h"
#include "GameUI.h"
#include "ShaderResource.h"


CGameUI::CGameUI()
{
	m_pCamera = NULL;
	m_pVoxelPocketUI = NULL;
	m_pVoxelIcons = NULL;
	m_pVoxelActivated = NULL;
	m_pRespawningTextUI = NULL;
	m_pRespawningGaugeUI = NULL;
	m_pStaminaUI = NULL;	
	m_pStaminaGaugeUI = NULL;
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
}

void CGameUI::SetCamera(CCamera *pCamera)
{
	if (m_pCamera) delete m_pCamera;
	m_pCamera = pCamera;
}

void CGameUI::BuildUIObjects(ID3D11Device *pd3dDevice)
{
	m_pVoxelPocketUI = new CGameObject();
	m_pVoxelPocketUI->SetMesh(CMeshResource::pUI_VoxelPocketMesh);
	m_pVoxelPocketUI->SetTexture(CTextureResource::pUI_VoxelPocket);
	m_pVoxelPocketUI->SetShader(CShaderResource::pTexturedNotLightingShader);
	m_pVoxelPocketUI->SetPosition(0.0f, -2.5f, +4.0f);

	m_pVoxelIcons = new CSprite();
	m_pVoxelIcons->SetSpriteTexture(CTextureResource::pUI_VoxelIcons);
	m_pVoxelIcons->BuildSpritePieces(pd3dDevice, 1.0f, 1.0f, 17);

	m_pVoxelActivated = new CGameObject();
	m_pVoxelActivated->SetMesh(CMeshResource::pUI_VoxelActivatedMesh);
	m_pVoxelActivated->SetTexture(CTextureResource::pUI_VoxelActivated);
	m_pVoxelActivated->SetShader(CShaderResource::pTexturedNotLightingShader);
	m_pVoxelActivated->SetPosition(0.0f, -2.5f, +3.99f);

	m_pRespawningTextUI = new CGameObject();
	m_pRespawningTextUI->SetMesh(CMeshResource::pUI_RespawningTextMesh);
	m_pRespawningTextUI->SetTexture(CTextureResource::pUI_Respawning);
	m_pRespawningTextUI->SetShader(CShaderResource::pTexturedNotLightingShader);
	m_pRespawningTextUI->SetPosition(0.0f, -0.0f, +10.0f);

	m_pRespawningGaugeUI = new CGameObject();
	m_pRespawningGaugeUI->SetMesh(CMeshResource::pUI_RespawnigGaugeMesh);
	m_pRespawningGaugeUI->SetTexture(CTextureResource::pUI_Respawning);
	m_pRespawningGaugeUI->SetShader(CShaderResource::pTexturedNotLightingShader);
	m_pRespawningGaugeUI->SetPosition(0.0f, -0.6f, +9.99f);

	m_pStaminaUI = new CGameObject();
	m_pStaminaUI->SetMesh(CMeshResource::pUI_StaminaMesh);
	m_pStaminaUI->SetTexture(CTextureResource::pUI_Stamina);
	m_pStaminaUI->SetShader(CShaderResource::pTexturedNotLightingShader);
	m_pStaminaUI->SetPosition(0.0f, -4.5f, +10.0f);

	m_pStaminaGaugeUI = new CGameObject();
	m_pStaminaGaugeUI->SetMesh(CMeshResource::pUI_StaminaGaugeMesh);
	m_pStaminaGaugeUI->SetTexture(CTextureResource::pUI_Stamina);
	m_pStaminaGaugeUI->SetShader(CShaderResource::pTexturedNotLightingShader);
	m_pStaminaGaugeUI->SetPosition(0.0f, -4.5f, +9.99f);
}

void CGameUI::BackgroundUIRender(ID3D11DeviceContext*pd3dDeviceContext)
{
	if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dDeviceContext);
	if (m_pVoxelPocketUI) m_pVoxelPocketUI->Render(pd3dDeviceContext);
}

void CGameUI::PlayerHavingVoxelRender(ID3D11DeviceContext *pd3dDeviceCotext, CPlayer *pPlayer)
{
	m_pVoxelActivated->SetPosition(-2.0f + 1.0f * pPlayer->m_iVoxelPocketSlotIdx, -2.5f, +3.99f);
	m_pVoxelActivated->Render(pd3dDeviceCotext);

	for (int i = 0; i < 5; i++)
	{
		if (pPlayer->m_VoxelPocket[i].m_bIsActive)
		{
			m_pVoxelIcons->GetSpritePieceByIndex(pPlayer->m_VoxelPocket[i].m_pVoxel->m_eVoxelType)->SetPosition(-2.0f + 1.0f*i, -2.5f, +3.98f);
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