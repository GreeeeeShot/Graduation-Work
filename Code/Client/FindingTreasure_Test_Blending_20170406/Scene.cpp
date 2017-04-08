#include "stdafx.h"
#include "Scene.h"
#include "Player.h"
#include "TextureResource.h"
#include "ShaderResource.h"

CScene::CScene()
{
	/*m_ppObjects = NULL;*/
	m_nObjects = 0;

	m_ppPlayers = NULL;

	m_pLights = NULL;
	m_pd3dcbLights = NULL;

	m_pVoxelTerrain = NULL;
	m_pWaveEffect = NULL;
	m_fGravitationalAcceleration = GRAVITATIONAL_ACCELERATION;
}

CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_pWaveEffect = new CWaveObject();
	m_pWaveEffect->SetMaterial(CMaterialResource::pHalfTransMaterial);
	m_pWaveEffect->SetTexture(CTextureResource::pWaveTexture);
	m_pWaveEffect->SetMesh(CMeshResource::pWaveMesh);
	m_pWaveEffect->SetShader(CShaderResource::pTexturedLightingShader);

	m_pVoxelTerrain = new CVoxelTerrain();
	//CShader *pTexturedLightingShader = new CTexturedLightingShader();
	//pTexturedLightingShader->CreateShader(pd3dDevice);
	CBlueprint* pTestMapBP = new CBlueprint();
	//m_pVoxelTerrain->CreateVoxelTerrain(pTestMapBP->LoadBlueprint("sample_house.txt"), pTexturedLightingShader);
	m_pVoxelTerrain->CreateVoxelTerrain(pTestMapBP->LoadBlueprint("uphill.txt"), CShaderResource::pTexturedLightingShader);
	delete pTestMapBP;

	CreateShaderVariables(pd3dDevice);							// ������ �Է¹޴� ��� ���۸� �����԰� ���ÿ� ���� ������ ���� �ִ´�.
}

void CScene::SetPlayers(CPlayer **ppPlayers)
{
	m_ppPlayers = ppPlayers;
}

void CScene::ReleaseObjects()
{
	ReleaseShaderVariables();
	//���� ��ü ����Ʈ�� �� ��ü�� ��ȯ(Release)�ϰ� ����Ʈ�� �Ҹ��Ų��.
	/*if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}*/
}

void CScene::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	//���� ���� ��ü�� ���ߴ� �ֺ������� �����Ѵ�.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//3���� ����(�� ����, ���� ����, ���⼺ ����)�� �����Ѵ�.
	m_pLights->m_pLights[0].m_bEnable = 0.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 80.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 8.0f);
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(-120.0f, 100.0f, 120.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(1.0f, 0.0f, -1.0f);
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = 0.0f;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 300.0f;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 2.0f);
	m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3(-130.0f, 20.0f, 130.0f);
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.0f, -0.5f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.1f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 15.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(13.0f));
	m_pLights->m_pLights[2].m_bEnable = 1.0f;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.5f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.7f, 0.1f, 0.1f, 8.0f);
	m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(-0.0f, -5.0f, 0.0f);
	m_pLights->m_pLights[3].m_bEnable = 0.0f;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 50.0f;
	m_pLights->m_pLights[3].m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvPosition = D3DXVECTOR3(-10.0f, 1.0f, 10.0f);
	m_pLights->m_pLights[3].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, -0.0f);
	m_pLights->m_pLights[3].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 60.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(D3DXToRadian(60.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(D3DXToRadian(30.0f));

	m_pLights->m_pLights[4].m_bEnable = 1.0f;
	m_pLights->m_pLights[4].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[4].m_fRange = 200.0f;
	m_pLights->m_pLights[4].m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[4].m_d3dxcDiffuse = D3DXCOLOR(0.9f, 0.3f, 1.0f, 1.0f);
	m_pLights->m_pLights[4].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 16.0f);
	m_pLights->m_pLights[4].m_d3dxvPosition = D3DXVECTOR3(-100.0f, 30.0f, 100.0f);
	m_pLights->m_pLights[4].m_d3dxvDirection = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
	m_pLights->m_pLights[4].m_d3dxvAttenuation = D3DXVECTOR3(0.8f, 0.001f, 0.0001f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(LIGHTS);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights);
}

void CScene::ReleaseShaderVariables()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
}

void CScene::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT, 1, &m_pd3dcbLights);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed)
{
	if(m_pWaveEffect) m_pWaveEffect->m_pMesh->Animate(pd3dDeviceContext, fTimeElapsed);
	m_ppPlayers[0]->RotateMoveDir(fTimeElapsed);
	m_ppPlayers[0]->MovingUnderPhysicalEnvironment(this, m_pVoxelTerrain, fTimeElapsed);
	m_ppPlayers[1]->MovingUnderPhysicalEnvironment(this, m_pVoxelTerrain, fTimeElapsed);
	CPhysicalCollision::ImpurseBasedCollisionResolution(m_ppPlayers[0], m_ppPlayers[1]);
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);
	
	/*for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->Render(pd3dDeviceContext);
	}*/
	m_pVoxelTerrain->Render(pd3dDeviceContext);
	if (m_pWaveEffect) m_pWaveEffect->Render(pd3dDeviceContext);
}