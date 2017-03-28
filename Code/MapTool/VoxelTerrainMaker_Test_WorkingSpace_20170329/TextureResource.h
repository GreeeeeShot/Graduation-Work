#pragma once

#define TEXTURE_GRASS_0			0
#define TEXTURE_WOOD_0			1

#define SAMPLER_WRAP_MODE_0		0

class CTexture
{
public:
	CTexture(int nTextures);
	virtual ~CTexture();

	int m_nReferences;
	void AddRef();
	void Release();

	void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState);

	ID3D11ShaderResourceView **m_ppd3dsrvTextures;
	ID3D11SamplerState **m_ppd3dSamplerStates;

	int m_nTextures;
};

class CTextureResource
{
public:
	CTextureResource();
	~CTextureResource();

	static ID3D11ShaderResourceView **ppd3dTexture;
	static ID3D11SamplerState **ppd3dSamplerState;

	static CTexture** ppTextureMaterial;

	static void CreateTextureResource(ID3D11Device *pd3dDevice);
};

