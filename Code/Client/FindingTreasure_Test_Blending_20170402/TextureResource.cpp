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
ID3D11SamplerState** CTextureResource::ppd3dSamplerState = NULL;

//CTexture** CTextureResource::ppTextureMaterial = NULL;
CTexture* CTextureResource::pGrassTexture = NULL;
CTexture* CTextureResource::pWoodTexture = NULL;
CTexture* CTextureResource::pUphillTexture = NULL;
CTexture* CTextureResource::pBushTexture = NULL;
CTexture** CTextureResource::ppRegisteredTexture = NULL;

CTextureResource::CTextureResource()
{
}


CTextureResource::~CTextureResource()
{
}

void CTextureResource::CreateTextureResource(ID3D11Device *pd3dDevice)
{
	ppd3dTexture = new ID3D11ShaderResourceView*[4];

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("grass.jpg"), NULL, NULL, &ppd3dTexture[0], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("wood.jpg"), NULL, NULL, &ppd3dTexture[1], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("grass.jpg"), NULL, NULL, &ppd3dTexture[2], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("bush_trans.dds"), NULL, NULL, &ppd3dTexture[3], NULL);

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

	pGrassTexture = new CTexture(1);
	pWoodTexture = new CTexture(1);
	pUphillTexture = new CTexture(1);
	pBushTexture = new CTexture(1);

	pGrassTexture->SetTexture(0, ppd3dTexture[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pWoodTexture->SetTexture(0, ppd3dTexture[1], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUphillTexture->SetTexture(0, ppd3dTexture[2], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pBushTexture->SetTexture(0, ppd3dTexture[3], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	
	ppRegisteredTexture = new CTexture*[16];				// 16개의 재질을 등록할 수 있다.

	ppRegisteredTexture[0] = pGrassTexture;
	ppRegisteredTexture[1] = pWoodTexture;
	ppRegisteredTexture[2] = pGrassTexture;
	ppRegisteredTexture[3] = pGrassTexture;
	ppRegisteredTexture[4] = pGrassTexture;
	ppRegisteredTexture[5] = pGrassTexture;
	ppRegisteredTexture[6] = pBushTexture;
}