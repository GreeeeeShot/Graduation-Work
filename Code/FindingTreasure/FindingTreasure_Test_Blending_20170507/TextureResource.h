#pragma once

#define TEXTURE_GRASS_0			0
#define TEXTURE_WOOD_0			1

#define TEXTURE_UPHILL_0		2
#define TEXTURE_UPHILL_1		3
#define TEXTURE_UPHILL_2		4
#define TEXTURE_UPHILL_3		5

#define TEXTURE_BUSH_0			6

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
	static ID3D11ShaderResourceView **ppd3dTextureUI;
	static ID3D11ShaderResourceView **ppd3dTextureEffect;
	static ID3D11SamplerState **ppd3dSamplerState;

	static CTexture* pSkydomTexture;
	static CTexture* pPirateTexture;
	static CTexture* pGrassTexture;
	static CTexture* pWaveTexture;
	static CTexture* pWoodTexture;
	static CTexture* pUphillTexture;
	static CTexture* pBushTexture;
	static CTexture* pSwampTexture;
	static CTexture* pJumpTexture;
	static CTexture* pSnowTexture;
	static CTexture* pEmptySnowTexture; 
	static CTexture* pIceTexture;

	static CTexture* pUI_VoxelIcons;
	static CTexture* pUI_VoxelPocket;
	static CTexture* pUI_Respawning;

	static CTexture* pEffect_ParticleBlue0;

	static CTexture** ppRegisteredTexture;

	static void CreateTextureResource(ID3D11Device *pd3dDevice);
};

