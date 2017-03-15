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

ID3D11ShaderResourceView** CTextureResource::ppd3dTexture = NULL;
ID3D11SamplerState** CTextureResource::ppd3dSamplerState = NULL;


CTextureResource::CTextureResource()
{
}


CTextureResource::~CTextureResource()
{
}

void CTextureResource::CreateTextureResource(ID3D11Device *pd3dDevice)
{
	ppd3dTexture = new ID3D11ShaderResourceView*[2];

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("grass.jpg"), NULL, NULL, &ppd3dTexture[GRASS_TEXTURE_0], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("grass.jpg"), NULL, NULL, &ppd3dTexture[WOOD_TEXTURE_0], NULL);


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
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &ppd3dSamplerState[WRAP_MODE_SAMPLER_0]);
}