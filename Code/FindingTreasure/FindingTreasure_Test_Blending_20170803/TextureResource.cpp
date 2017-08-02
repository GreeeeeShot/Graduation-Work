#include "stdafx.h"
#include "TextureResource.h"

CTexture::CTexture(int nTextures)
{
	m_nReferences = 0;
	m_nTextures = nTextures;
	m_ppd3dsrvTextures = new ID3D11ShaderResourceView*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) m_ppd3dsrvTextures[i] = NULL;
	m_ppd3dSamplerStates = new ID3D11SamplerState*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) m_ppd3dSamplerStates[i] = NULL;
}

CTexture::~CTexture()
{
	if (m_ppd3dsrvTextures)
	{
		for (int i = 0; i < m_nTextures; i++) m_ppd3dsrvTextures[i]->Release();
		delete[] m_ppd3dsrvTextures;
		m_ppd3dsrvTextures = NULL;
	}
	if (m_ppd3dSamplerStates)
	{
		for (int i = 0; i < m_nTextures; i++) m_ppd3dSamplerStates[i]->Release();
		delete[] m_ppd3dSamplerStates;
		m_ppd3dSamplerStates = NULL;
	}
}

void CTexture::AddRef()
{
	m_nReferences++;
}

void CTexture::Release()
{
	if (m_nReferences > 0) m_nReferences--;
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppd3dsrvTextures[i]) m_ppd3dsrvTextures[i]->Release();
		if (m_ppd3dSamplerStates[i]) m_ppd3dSamplerStates[i]->Release();
	}
	if (m_nReferences == 0) delete this;
}

void CTexture::SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState)
{
	if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->Release();
	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->Release();
	m_ppd3dsrvTextures[nIndex] = pd3dsrvTexture;
	m_ppd3dSamplerStates[nIndex] = pd3dSamplerState;
	if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->AddRef();
	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->AddRef();
}

ID3D11ShaderResourceView** CTextureResource::ppd3dTextureVoxel = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dTextureUI = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dTextureEffect = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dTexturePlayer = NULL;
ID3D11SamplerState** CTextureResource::ppd3dSamplerState = NULL;

CTexture* CTextureResource::pSkydomTexture = NULL;
CTexture* CTextureResource::pPirateTexture = NULL;
CTexture* CTextureResource::pGrassTexture = NULL;
CTexture* CTextureResource::pWoodTexture = NULL;
CTexture* CTextureResource::pZungleUphillTexture = NULL;
CTexture* CTextureResource::pSnowUphillTexture = NULL;
CTexture* CTextureResource::pBushTexture = NULL;
CTexture* CTextureResource::pSwampTexture = NULL;
CTexture* CTextureResource::pJumpTexture = NULL;
CTexture* CTextureResource::pSnowTexture = NULL;
CTexture* CTextureResource::pEmptySnowTexture = NULL;
CTexture* CTextureResource::pIceTexture = NULL;
CTexture* CTextureResource::pWaveTexture = NULL;
CTexture* CTextureResource::pWaveNormalMap = NULL;
CTexture* CTextureResource::pUI_VoxelIcons = NULL;
CTexture* CTextureResource::pUI_VoxelPocket = NULL;
CTexture* CTextureResource::pUI_VoxelActivated = NULL;
CTexture* CTextureResource::pUI_Respawning = NULL;
CTexture* CTextureResource::pUI_Stamina = NULL;
CTexture* CTextureResource::pEffect_ParticleBlue0 = NULL;

CTexture* CTextureResource::pVoxelTextures[17];

ID3D11ShaderResourceView** CTextureResource::ppd3dSkyTexture = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dWaveTexture = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dWaveNormalMap = NULL;

CTextureResource::CTextureResource()
{
}


CTextureResource::~CTextureResource()
{
}

void CTextureResource::CreateTextureResource(ID3D11Device *pd3dDevice)
{
	ppd3dTextureVoxel = new ID3D11ShaderResourceView*[10];

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/grass.png"), NULL, NULL, &ppd3dTextureVoxel[0], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/wood.png"), NULL, NULL, &ppd3dTextureVoxel[1], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/grass.png"), NULL, NULL, &ppd3dTextureVoxel[2], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/bush.png"), NULL, NULL, &ppd3dTextureVoxel[3], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/swamp.png"), NULL, NULL, &ppd3dTextureVoxel[4], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/jump.png"), NULL, NULL, &ppd3dTextureVoxel[5], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/snow.png"), NULL, NULL, &ppd3dTextureVoxel[6], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/emptysnow.png"), NULL, NULL, &ppd3dTextureVoxel[7], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/ice.png"), NULL, NULL, &ppd3dTextureVoxel[8], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/snow.png"), NULL, NULL, &ppd3dTextureVoxel[9], NULL);			// 사실 중복되니 사용할 이유는 없네.

	ppd3dTextureUI = new ID3D11ShaderResourceView*[5];
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_voxellist.png"), NULL, NULL, &ppd3dTextureUI[0], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_voxelpocket.png"), NULL, NULL, &ppd3dTextureUI[1], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_voxelactivated.png"), NULL, NULL, &ppd3dTextureUI[2], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_respawning.png"), NULL, NULL, &ppd3dTextureUI[3], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_stamina.png"), NULL, NULL, &ppd3dTextureUI[4], NULL);

	ppd3dTextureEffect = new ID3D11ShaderResourceView*[1];
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/effect/particle_blue.png"), NULL, NULL, &ppd3dTextureEffect[0], NULL);

	ppd3dTexturePlayer = new ID3D11ShaderResourceView*[1];
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/pirate.png"), NULL, NULL, &ppd3dTexturePlayer[0], NULL);

	ppd3dSkyTexture = new ID3D11ShaderResourceView*();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/sky4.png"), NULL, NULL, ppd3dSkyTexture, NULL);
	ppd3dWaveTexture = new ID3D11ShaderResourceView*();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/wave1.jpg"), NULL, NULL, ppd3dWaveTexture, NULL);
	ppd3dWaveNormalMap = new ID3D11ShaderResourceView*();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/wavenormal.png"), NULL, NULL, ppd3dWaveNormalMap, NULL);

	

	ppd3dSamplerState = new  ID3D11SamplerState*[1];

	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pSkydomTexture = new CTexture(1);
	pPirateTexture = new CTexture(1);
	pGrassTexture = new CTexture(1);
	pWoodTexture = new CTexture(1);
	pZungleUphillTexture = new CTexture(1);
	pBushTexture = new CTexture(1);
	pWaveTexture = new CTexture(1);
	pWaveNormalMap = new CTexture(1);
	pSwampTexture = new CTexture(1);
	pJumpTexture = new CTexture(1);
	pSnowTexture = new CTexture(1);
	pEmptySnowTexture = new CTexture(1);
	pIceTexture = new CTexture(1);
	pSnowUphillTexture = new CTexture(1);

	pGrassTexture->SetTexture(0, ppd3dTextureVoxel[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pWoodTexture->SetTexture(0, ppd3dTextureVoxel[1], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pZungleUphillTexture->SetTexture(0, ppd3dTextureVoxel[2], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pBushTexture->SetTexture(0, ppd3dTextureVoxel[3], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pSwampTexture->SetTexture(0, ppd3dTextureVoxel[4], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pJumpTexture->SetTexture(0, ppd3dTextureVoxel[5], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pSnowTexture->SetTexture(0, ppd3dTextureVoxel[6], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pEmptySnowTexture->SetTexture(0, ppd3dTextureVoxel[7], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pIceTexture->SetTexture(0, ppd3dTextureVoxel[8], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pSnowUphillTexture->SetTexture(0, ppd3dTextureVoxel[9], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pVoxelTextures[0] = NULL;
	pVoxelTextures[1] = CTextureResource::pGrassTexture;
	pVoxelTextures[2] = CTextureResource::pWoodTexture;
	pVoxelTextures[3] = CTextureResource::pZungleUphillTexture;
	pVoxelTextures[4] = CTextureResource::pZungleUphillTexture;
	pVoxelTextures[5] = CTextureResource::pZungleUphillTexture;
	pVoxelTextures[6] = CTextureResource::pZungleUphillTexture;
	pVoxelTextures[7] = CTextureResource::pBushTexture;
	pVoxelTextures[8] = CTextureResource::pSwampTexture;
	pVoxelTextures[9] = CTextureResource::pJumpTexture;
	pVoxelTextures[10] = CTextureResource::pSnowTexture;
	pVoxelTextures[11] = CTextureResource::pEmptySnowTexture;
	pVoxelTextures[12] = CTextureResource::pIceTexture;
	pVoxelTextures[13] = CTextureResource::pSnowUphillTexture;
	pVoxelTextures[14] = CTextureResource::pSnowUphillTexture;
	pVoxelTextures[15] = CTextureResource::pSnowUphillTexture;
	pVoxelTextures[16] = CTextureResource::pSnowUphillTexture;

	pUI_VoxelIcons = new CTexture(1);
	pUI_VoxelPocket = new CTexture(1);
	pUI_VoxelActivated = new CTexture(1);
	pUI_Respawning = new CTexture(1);
	pUI_Stamina = new CTexture(1);

	pUI_VoxelIcons->SetTexture(0, ppd3dTextureUI[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_VoxelPocket->SetTexture(0, ppd3dTextureUI[1], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_VoxelActivated->SetTexture(0, ppd3dTextureUI[2], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_Respawning->SetTexture(0, ppd3dTextureUI[3], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_Stamina->SetTexture(0, ppd3dTextureUI[4], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pEffect_ParticleBlue0 = new CTexture(1);
	pEffect_ParticleBlue0->SetTexture(0, ppd3dTextureEffect[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pPirateTexture->SetTexture(0, ppd3dTexturePlayer[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pWaveTexture->SetTexture(0, *ppd3dWaveTexture, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pWaveNormalMap->SetTexture(0, *ppd3dWaveNormalMap, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pSkydomTexture->SetTexture(0, *ppd3dSkyTexture, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
}

void CTextureResource::DestroyTextureResource(void)
{
	if(pSkydomTexture) delete pSkydomTexture;
	if(pPirateTexture) delete pPirateTexture;
	if(pGrassTexture) delete pGrassTexture;
	if(pWoodTexture) delete pWoodTexture;
	if(pZungleUphillTexture) delete pZungleUphillTexture;
	if (pSnowUphillTexture) delete pSnowUphillTexture;
	if (pBushTexture) delete pBushTexture;
	if(pSwampTexture) delete pSwampTexture;
	if(pJumpTexture) delete pJumpTexture;
	if (pSnowTexture) delete pSnowTexture;
	if (pEmptySnowTexture) delete pEmptySnowTexture;
	if (pIceTexture) delete pIceTexture;
	if (pWaveTexture) delete pWaveTexture;
	if (pWaveNormalMap) delete pWaveNormalMap;
	if (pUI_VoxelIcons) delete pUI_VoxelIcons;
	if(pUI_VoxelPocket) delete pUI_VoxelPocket;
	if (pUI_VoxelActivated) delete pUI_VoxelActivated;
	if (pUI_Respawning) delete pUI_Respawning;
	if (pUI_Stamina) delete pUI_Stamina;
	if (pEffect_ParticleBlue0) delete pEffect_ParticleBlue0;

	if (ppd3dSkyTexture)
	{
		(*ppd3dSkyTexture)->Release();
		delete ppd3dSkyTexture;
	}

	if (ppd3dWaveTexture)
	{
		(*ppd3dWaveTexture)->Release();
		delete ppd3dWaveTexture;
	}

	if (ppd3dWaveNormalMap)
	{
		(*ppd3dWaveNormalMap)->Release();
		delete ppd3dWaveNormalMap;
	}

	for (int i = 0; i < 10; i++)
	{
		ppd3dTextureVoxel[i]->Release();
	}
	delete[]ppd3dTextureVoxel;
	
	for (int i = 0; i < 5; i++)
	{
		ppd3dTextureUI[i]->Release();
	}
	delete[]ppd3dTextureUI;
	
	for (int i = 0; i < 1; i++)
	{
		ppd3dTextureEffect[i]->Release();
	}
	delete[]ppd3dTextureEffect;
	
	for (int i = 0; i < 1; i++)
	{
		ppd3dTexturePlayer[i]->Release();
	}
	delete[]ppd3dTexturePlayer;
	
	for (int i = 0; i < 1; i++)
	{
		ppd3dSamplerState[i]->Release();
	}
	delete[]ppd3dSamplerState;
}