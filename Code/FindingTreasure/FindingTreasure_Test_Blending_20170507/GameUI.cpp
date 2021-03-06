#include "stdafx.h"
#include "GameUI.h"
#include "ShaderResource.h"


CGameUI::CGameUI()
{
	m_pCamera = NULL;
	m_pVoxelPocketUI = NULL;
	m_pVoxelIcons = NULL;
	m_pRespawningTextUI = NULL;
	m_pRespawningGaugeUI = NULL;
}


CGameUI::~CGameUI()
{
	if (m_pVoxelPocketUI) delete m_pVoxelPocketUI;
	if (m_pVoxelIcons) delete m_pVoxelIcons;
	if (m_pRespawningTextUI) delete m_pRespawningTextUI;
	if (m_pRespawningGaugeUI) delete m_pRespawningGaugeUI;
}

void CGameUI::SetCamera(CCamera *pCamera)
{
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
	m_pVoxelIcons->BuildSpritePieces(pd3dDevice, 1.0f, 1.0f, 13);

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
}

void CGameUI::BackgroundUIRender(ID3D11DeviceContext*pd3dDeviceContext)
{
	if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dDeviceContext);
	if (m_pVoxelPocketUI) m_pVoxelPocketUI->Render(pd3dDeviceContext);
}

void CGameUI::PlayerHavingVoxelRender(ID3D11DeviceContext *pd3dDeviceCotext, CPlayer *pPlayer)
{
	for (int i = 0; i < 5; i++)
	{
		if (pPlayer->m_VoxelPocket[i].m_bIsActive)
		{
			m_pVoxelIcons->GetSpritePieceByIndex(pPlayer->m_VoxelPocket[i].m_pVoxel->m_eVoxelType)->SetPosition(-2.0f + 1.0f*i, -2.5f, +3.99f);
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