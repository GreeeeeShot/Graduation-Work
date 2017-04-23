#include "stdafx.h"
#include "GameUI.h"
#include "ShaderResource.h"


CGameUI::CGameUI()
{
	m_pCamera = NULL;
	m_pVoxelPocketUI = NULL;
	m_pVoxelIcons = NULL;
}


CGameUI::~CGameUI()
{
	if (m_pVoxelPocketUI) delete m_pVoxelPocketUI;
	if (m_pVoxelIcons) delete m_pVoxelIcons;
}

void CGameUI::SetCamera(CCamera *pCamera)
{
	m_pCamera = pCamera;
}

void CGameUI::BuildUIObjects(ID3D11Device *pd3dDevice)
{
	m_pVoxelPocketUI = new CVoxel();
	m_pVoxelPocketUI->SetMesh(CMeshResource::pUI_VoxelPocketMesh);
	m_pVoxelPocketUI->SetTexture(CTextureResource::pUI_VoxelPocket);
	m_pVoxelPocketUI->SetShader(CShaderResource::pTexturedNotLightingShader);
	m_pVoxelPocketUI->SetPosition(0.0f, -2.5f, +4.0f);

	m_pVoxelIcons = new CSprite();
	m_pVoxelIcons->SetSpriteTexture(CTextureResource::pUI_VoxelIcons);
	m_pVoxelIcons->BuildSpritePieces(pd3dDevice, 1.0f, 1.0f, 13);
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