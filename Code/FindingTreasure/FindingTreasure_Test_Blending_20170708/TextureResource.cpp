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
	if (m_ppd3dsrvTextures) delete[] m_ppd3dsrvTextures;
	if (m_ppd3dSamplerStates) delete[] m_ppd3dSamplerStates;
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

ID3D11ShaderResourceView** CTextureResource::ppd3dTexture = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dTextureUI = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dTextureEffect = NULL;
ID3D11SamplerState** CTextureResource::ppd3dSamplerState = NULL;

CTexture* CTextureResource::pSkydomTexture = NULL;
CTexture* CTextureResource::pPirateTexture = NULL;
CTexture* CTextureResource::pGrassTexture = NULL;
CTexture* CTextureResource::pWoodTexture = NULL;
CTexture* CTextureResource::pUphillTexture = NULL;
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
CTexture* CTextureResource::pEffect_ParticleBlue0 = NULL;
CTexture** CTextureResource::ppRegisteredTexture = NULL;

CTextureResource::CTextureResource()
{
}


CTextureResource::~CTextureResource()
{
}

void CTextureResource::CreateTextureResource(ID3D11Device *pd3dDevice)
{
	ppd3dTexture = new ID3D11ShaderResourceView*[11];

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/sky4.png"), NULL, NULL, &ppd3dTexture[10], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/pirate.png"), NULL, NULL, &ppd3dTexture[9], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/grass.png"), NULL, NULL, &ppd3dTexture[0], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/wood.png"), NULL, NULL, &ppd3dTexture[1], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/grass.png"), NULL, NULL, &ppd3dTexture[2], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/bush.png"), NULL, NULL, &ppd3dTexture[3], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/swamp.png"), NULL, NULL, &ppd3dTexture[4], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/jump.png"), NULL, NULL, &ppd3dTexture[5], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/snow.png"), NULL, NULL, &ppd3dTexture[6], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/emptysnow.png"), NULL, NULL, &ppd3dTexture[7], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/ice.png"), NULL, NULL, &ppd3dTexture[8], NULL);

	ppd3dTextureUI = new ID3D11ShaderResourceView*[4];
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_voxellist.png"), NULL, NULL, &ppd3dTextureUI[0], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_voxelpocket.png"), NULL, NULL, &ppd3dTextureUI[1], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_voxelactivated.png"), NULL, NULL, &ppd3dTextureUI[2], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_respawning.png"), NULL, NULL, &ppd3dTextureUI[3], NULL);

	ppd3dTextureEffect = new ID3D11ShaderResourceView*[1];
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/effect/particle_blue.png"), NULL, NULL, &ppd3dTextureEffect[0], NULL);

	ID3D11ShaderResourceView **ppd3dWaveTexture = new ID3D11ShaderResourceView*();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/wave.jpg"), NULL, NULL, ppd3dWaveTexture, NULL);
	ID3D11ShaderResourceView **ppd3dWaveNormalMap = new ID3D11ShaderResourceView*();
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
	pUphillTexture = new CTexture(1);
	pBushTexture = new CTexture(1);
	pWaveTexture = new CTexture(1);
	pWaveNormalMap = new CTexture(1);
	pSwampTexture = new CTexture(1);
	pJumpTexture = new CTexture(1);
	pSnowTexture = new CTexture(1);
	pEmptySnowTexture = new CTexture(1);
	pIceTexture = new CTexture(1);

	pSkydomTexture->SetTexture(0, ppd3dTexture[10], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pPirateTexture->SetTexture(0, ppd3dTexture[9], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pGrassTexture->SetTexture(0, ppd3dTexture[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pWoodTexture->SetTexture(0, ppd3dTexture[1], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUphillTexture->SetTexture(0, ppd3dTexture[2], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pBushTexture->SetTexture(0, ppd3dTexture[3], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pSwampTexture->SetTexture(0, ppd3dTexture[4], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pJumpTexture->SetTexture(0, ppd3dTexture[5], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pSnowTexture->SetTexture(0, ppd3dTexture[6], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pEmptySnowTexture->SetTexture(0, ppd3dTexture[7], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pIceTexture->SetTexture(0, ppd3dTexture[8], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pWaveTexture->SetTexture(0, *ppd3dWaveTexture, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pWaveNormalMap->SetTexture(0, *ppd3dWaveNormalMap, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pUI_VoxelIcons = new CTexture(1);
	pUI_VoxelPocket = new CTexture(1);
	pUI_VoxelActivated = new CTexture(1);
	pUI_Respawning = new CTexture(1);

	pUI_VoxelIcons->SetTexture(0, ppd3dTextureUI[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_VoxelPocket->SetTexture(0, ppd3dTextureUI[1], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_VoxelActivated->SetTexture(0, ppd3dTextureUI[2], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_Respawning->SetTexture(0, ppd3dTextureUI[3], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pEffect_ParticleBlue0 = new CTexture(1);
	pEffect_ParticleBlue0->SetTexture(0, ppd3dTextureEffect[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	
	ppRegisteredTexture = new CTexture*[16];				// 16개의 재질을 등록할 수 있다.

	ppRegisteredTexture[0] = pGrassTexture;
	ppRegisteredTexture[1] = pWoodTexture;
	ppRegisteredTexture[2] = pGrassTexture;
	ppRegisteredTexture[3] = pGrassTexture;
	ppRegisteredTexture[4] = pGrassTexture;
	ppRegisteredTexture[5] = pGrassTexture;
	ppRegisteredTexture[6] = pBushTexture;
}