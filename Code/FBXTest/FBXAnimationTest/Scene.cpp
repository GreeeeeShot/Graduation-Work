#include "stdafx.h"
#include "Scene.h"
#include <string>


CScene::CScene()
{
	m_ppObjects = NULL;
	m_nObjects = 0;
	pCubeMesh = NULL;

	m_pLights = NULL;
	m_pd3dcbLights = NULL;
}

CScene::~CScene()
{
}

//void CScene::BuildObjects(ID3D11Device *pd3dDevice)
//{
//	//���̴� ��ü�� �����Ѵ�.
//	CShader *pShader = new CShader();
//	pShader->CreateShader(pd3dDevice);
//
//	//���� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
//	m_nObjects = 1;
//	m_ppObjects = new CGameObject*[m_nObjects];
//
//	//�ﰢ�� �޽�(CTriangleMesh)�� �����Ѵ�.
//	CTriangleMesh *pMesh = new CTriangleMesh(pd3dDevice);
//
//	//�ﰢ�� ��ü(CTriangleObject)�� �����ϰ� �ﰢ�� �޽��� ���̴��� �����Ѵ�.
//	CRotatingObject *pObject = new CRotatingObject();
//	pObject->SetMesh(pMesh);
//	pObject->SetShader(pShader);
//
//	//��ü�� �����Ѵ�.
//	m_ppObjects[0] = pObject;
//}

//void CScene::BuildObjects(ID3D11Device *pd3dDevice)
//{
//	CShader *pShader = new CShader();
//	pShader->CreateShader(pd3dDevice);
//
//	m_nObjects = 1;
//	m_ppObjects = new CGameObject*[m_nObjects];
//	//����x����x���̰� 15x15x15�� ������ü �޽��� �����Ѵ�.
//	CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);
//	CRotatingObject *pObject = new CRotatingObject();
//	pObject->SetMesh(pMesh);
//	pObject->SetShader(pShader);
//	m_ppObjects[0] = pObject;
//}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	//�ؽ��� ���ο� ����� ���÷� ���� ��ü�� �����Ѵ�.
	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	// ���Ϸκ��� �ؽ��� ��ü(ID3D11ShaderResourceView ; ��)�� �����Ѵ�.
	ID3D11ShaderResourceView *pd3dTexture = NULL;
	//DirectX::ScratchImage image = LoadTextureFromFile("snow.tga");
	//DirectX::CreateShaderResourceView(pd3dDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &pd3dTexture);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("pirate.png"), NULL, NULL, &pd3dTexture, NULL);
	
	CTexture *pJungleTexture = new CTexture(1);
	pJungleTexture->SetTexture(0, pd3dTexture, pd3dSamplerState);

	CShader *pTexturedLightingShader = new CTexturedLightingShader();
	pTexturedLightingShader->CreateShader(pd3dDevice);				// ���� ��� �� ���� ��� ���۸� �����Ѵ�.


	pCubeMesh = new CTexturedLightingCharacterMesh(pd3dDevice);
	//CTexturedLightingCubeMesh *pCubeMesh = new CTexturedLightingCubeMesh(pd3dDevice);
	CGameObject *pObject = new CGameObject();

	// ������ ������ ���� ���� �� �ݻ��Ѵ�.
	CMaterial *pStandardMaterial = new CMaterial();				
	pStandardMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pStandardMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	pObject->SetMaterial(pStandardMaterial);
	pObject->SetShader(pTexturedLightingShader);
	pCubeMesh->SetTexture(pJungleTexture);
	pObject->SetMesh(pCubeMesh);
	m_ppObjects[0] = pObject;

	CreateShaderVariables(pd3dDevice);							// ������ �Է¹޴� ��� ���۸� �����԰� ���ÿ� ���� ������ ���� �ִ´�.
}

void CScene::ReleaseObjects()
{
	ReleaseShaderVariables();
	//���� ��ü ����Ʈ�� �� ��ü�� ��ȯ(Release)�ϰ� ����Ʈ�� �Ҹ��Ų��.
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}
}

void CScene::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	//���� ���� ��ü�� ���ߴ� �ֺ������� �����Ѵ�.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);

	//3���� ����(�� ����, ���� ����, ���⼺ ����)�� �����Ѵ�.
	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(300.0f, 300.0f, 300.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = 1.0f;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(20.0f));
	m_pLights->m_pLights[2].m_bEnable = 1.0f;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_bEnable = 1.0f;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcDiffuse = D3DXCOLOR(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	m_pLights->m_pLights[3].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 20.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(D3DXToRadian(15.0f));

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

void CScene::AnimateObjects(ID3D11Device *pd3dDevice,float fTimeElapsed)
{
	//for (int i = 0; i < m_nObjects; i++);// m_ppObjects[i]->Animate(fTimeElapsed);
	pCubeMesh->Animation(pd3dDevice, fTimeElapsed);
	
}


//void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
//{
//	for (int i = 0; i < m_nObjects; i++)
//	{
//		m_ppObjects[i]->Render(pd3dDeviceContext);
//	}
//}

//void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
//{
//	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);
//
//	for (int i = 0; i < m_nShaders; i++)
//	{
//		m_ppShaders[i]->Render(pd3dDeviceContext, pCamera);
//	}
//}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);

	for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->Render(pd3dDeviceContext);
	}
}
