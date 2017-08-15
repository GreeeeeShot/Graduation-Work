#include "stdafx.h"
#include "TextureResource.h"

CTexture::CTexture(int nTextures)
{
	//m_nReferences = 0;
	m_nTextures = nTextures;
	m_ppd3dsrvTextures = new ID3D11ShaderResourceView*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) m_ppd3dsrvTextures[i] = NULL;
	m_ppd3dSamplerStates = new ID3D11SamplerState*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) m_ppd3dSamplerStates[i] = NULL;
}

CTexture::~CTexture()
{
	delete[] m_ppd3dsrvTextures;
	delete[] m_ppd3dSamplerStates;
	/*if (m_ppd3dsrvTextures)
	{
		for (int i = 0; i < m_nTextures; i++) m_ppd3dsrvTextures[i]->Release();
		delete[] m_ppd3dsrvTextures;
		m_ppd3dsrvTextures = NULL;
	}*/
	/*if (m_ppd3dSamplerStates)
	{
		for (int i = 0; i < m_nTextures; i++) m_ppd3dSamplerStates[i]->Release();
		delete[] m_ppd3dSamplerStates;
		m_ppd3dSamplerStates = NULL;
	}*/
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
	//if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->Release();
	//if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->Release();
	m_ppd3dsrvTextures[nIndex] = pd3dsrvTexture;
	m_ppd3dSamplerStates[nIndex] = pd3dSamplerState;
	//if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->AddRef();
	//if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->AddRef();
}

ID3D11ShaderResourceView** CTextureResource::ppd3dTextureVoxel = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dTextureUI = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dTextureEffect = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dTexturePlayer = NULL;
ID3D11ShaderResourceView* CTextureResource::pd3dTextureArrayVoxel = NULL;
ID3D11SamplerState** CTextureResource::ppd3dSamplerState = NULL;


CTexture* CTextureResource::pSkydomTexture = NULL;
CTexture* CTextureResource::pPirateTexture = NULL;
CTexture* CTextureResource::pCowgirlTexture = NULL;
CTexture* CTextureResource::pShipTexture = NULL;
CTexture* CTextureResource::pTreasureTexture = NULL;
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
CTexture* CTextureResource::pUI_Numbers = NULL;
CTexture* CTextureResource::pUI_Keyboard = NULL;
CTexture* CTextureResource::pUI_Result = NULL;
CTexture* CTextureResource::pEffect_ParticleBlue0 = NULL;

CTexture* CTextureResource::pVoxelTextures[17];

ID3D11ShaderResourceView** CTextureResource::ppd3dShipTexture = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dTreasureTexture = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dSkyTexture = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dWaveTexture = NULL;
ID3D11ShaderResourceView** CTextureResource::ppd3dWaveNormalMap = NULL;

CTextureResource::CTextureResource()
{
}


CTextureResource::~CTextureResource()
{
}

ID3D11ShaderResourceView* CTextureResource::CreateTexture2DArraySRV(
	ID3D11Device *pd3dDevice,
	ID3D11DeviceContext* pd3dDeviceContext,
	std::wstring* ppFileNameList,
	UINT iAttributeNum,
	DXGI_FORMAT format,
	UINT filter,
	UINT mipFilter)
{
	//
	// 텍스쳐 버퍼를 할당하고 이미지를 저장한다.
	// 각 텍스쳐 버퍼에 저장된 이미지를 CPU를 통해 텍스쳐 배열로 옮기기 위하여
	// Usage는 D3D11_USAGE_STAGING으로 설정한다.
	//

	UINT size = iAttributeNum;

	std::vector<ID3D11Texture2D*> srcTex(size);
	for (UINT i = 0; i < size; ++i)
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;

		loadInfo.Width = D3DX11_FROM_FILE;
		loadInfo.Height = D3DX11_FROM_FILE;
		loadInfo.Depth = D3DX11_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = D3DX11_FROM_FILE;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = format;
		loadInfo.Filter = filter;
		loadInfo.MipFilter = mipFilter;
		loadInfo.pSrcInfo = 0;

		D3DX11CreateTextureFromFile(pd3dDevice, ppFileNameList[i].c_str(),
			&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0);					// => srcTex에 각 이미지가 저장된다.
	}

	//
	// 텍스쳐 배열을 생성한다.
	//

	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;							// 이를 통해 텍스쳐 배열이 됨에 주목.
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	pd3dDevice->CreateTexture2D(&texArrayDesc, 0, &texArray);

	//
	// 텍스쳐 버퍼에 각 이미지를 GPU 비디오 메모리 내 저장한다.
	//

	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			pd3dDeviceContext->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
			// srcTex를 staging을 통해 읽을 수 있도록 설정해두었다.

			pd3dDeviceContext->UpdateSubresource(texArray,								// UpdateSubresource에 설명은 교재 474p를 참고할 것.
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			pd3dDeviceContext->Unmap(srcTex[texElement], mipLevel);
		}
	}

	// 텍스쳐 배열에 관한 뷰 인터페이스를 생성한다.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;			

	ID3D11ShaderResourceView* texArraySRV = 0;
	pd3dDevice->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV);

	texArray->Release();

	for (UINT i = 0; i < size; ++i)
		srcTex[i]->Release();

	return texArraySRV;
}

void CTextureResource::CreateTextureResource(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3dDeviceContext)
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
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/snow.png"), NULL, NULL, &ppd3dTextureVoxel[9], NULL);

	std::wstring wstrTextureVoxelList[10] = {
		_T("Texture/cpl/grass.png"),
		_T("Texture/cpl/wood.png"),
		_T("Texture/cpl/grass.png"),
		_T("Texture/cpl/bush.png"),
		_T("Texture/cpl/swamp.png"),
		_T("Texture/cpl/jump.png"),
		_T("Texture/cpl/snow.png"),
		_T("Texture/cpl/emptysnow.png"),
		_T("Texture/cpl/ice.png"),
		_T("Texture/cpl/snow.png")
	};
	pd3dTextureArrayVoxel = CreateTexture2DArraySRV(pd3dDevice, pd3dDeviceContext, wstrTextureVoxelList, 10);// 사실 중복되니 사용할 이유는 없네.

	ppd3dTextureUI = new ID3D11ShaderResourceView*[8];
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_voxellist.png"), NULL, NULL, &ppd3dTextureUI[0], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_voxelpocket.png"), NULL, NULL, &ppd3dTextureUI[1], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_voxelactivated.png"), NULL, NULL, &ppd3dTextureUI[2], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_respawning.png"), NULL, NULL, &ppd3dTextureUI[3], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_stamina.png"), NULL, NULL, &ppd3dTextureUI[4], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_number.png"), NULL, NULL, &ppd3dTextureUI[5], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_keyboard.png"), NULL, NULL, &ppd3dTextureUI[6], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/ui/UI_result.png"), NULL, NULL, &ppd3dTextureUI[7], NULL);

	ppd3dTextureEffect = new ID3D11ShaderResourceView*[1];
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/effect/particle_blue.png"), NULL, NULL, &ppd3dTextureEffect[0], NULL);

	ppd3dTexturePlayer = new ID3D11ShaderResourceView*[2];
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/pirate.png"), NULL, NULL, &ppd3dTexturePlayer[0], NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/cowgirl.png"), NULL, NULL, &ppd3dTexturePlayer[1], NULL);


	ppd3dSkyTexture = new ID3D11ShaderResourceView*();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/sky4.png"), NULL, NULL, ppd3dSkyTexture, NULL);
	ppd3dWaveTexture = new ID3D11ShaderResourceView*();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/wave1.jpg"), NULL, NULL, ppd3dWaveTexture, NULL);
	ppd3dWaveNormalMap = new ID3D11ShaderResourceView*();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/wavenormal.png"), NULL, NULL, ppd3dWaveNormalMap, NULL);
	ppd3dShipTexture = new ID3D11ShaderResourceView*();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/boat.png"), NULL, NULL, ppd3dShipTexture, NULL);
	ppd3dTreasureTexture = new ID3D11ShaderResourceView*();
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Texture/cpl/TreasureBox.png"), NULL, NULL, ppd3dTreasureTexture, NULL);
	

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

	pShipTexture = new CTexture(1);
	pTreasureTexture = new CTexture(1);
	pSkydomTexture = new CTexture(1);
	pCowgirlTexture = new CTexture(1);
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
	pUI_Numbers = new CTexture(1);
	pUI_Keyboard = new CTexture(1);
	pUI_Result = new CTexture(1);

	pUI_VoxelIcons->SetTexture(0, ppd3dTextureUI[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_VoxelPocket->SetTexture(0, ppd3dTextureUI[1], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_VoxelActivated->SetTexture(0, ppd3dTextureUI[2], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_Respawning->SetTexture(0, ppd3dTextureUI[3], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_Stamina->SetTexture(0, ppd3dTextureUI[4], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_Numbers->SetTexture(0, ppd3dTextureUI[5], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_Keyboard->SetTexture(0, ppd3dTextureUI[6], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pUI_Result->SetTexture(0, ppd3dTextureUI[7], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pEffect_ParticleBlue0 = new CTexture(1);
	pEffect_ParticleBlue0->SetTexture(0, ppd3dTextureEffect[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pPirateTexture->SetTexture(0, ppd3dTexturePlayer[0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pCowgirlTexture->SetTexture(0, ppd3dTexturePlayer[1], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);

	pWaveTexture->SetTexture(0, *ppd3dWaveTexture, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pWaveNormalMap->SetTexture(0, *ppd3dWaveNormalMap, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pSkydomTexture->SetTexture(0, *ppd3dSkyTexture, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pShipTexture->SetTexture(0, *ppd3dShipTexture, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	pTreasureTexture->SetTexture(0, *ppd3dTreasureTexture, CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
}

void CTextureResource::DestroyTextureResource(void)
{
	if (pSkydomTexture)
	{
		delete pSkydomTexture;
		pSkydomTexture = NULL;
	}
	if (pPirateTexture)
	{
		delete pPirateTexture;
		pPirateTexture = NULL;
	}
	if (pGrassTexture)
	{
		delete pGrassTexture;
		pGrassTexture = NULL;
	}
	if (pWoodTexture)
	{
		delete pWoodTexture;
		pWoodTexture = NULL;
	}
	if (pZungleUphillTexture)
	{
		delete pZungleUphillTexture;
		pZungleUphillTexture = NULL;
	}
	if (pSnowUphillTexture)
	{
		delete pSnowUphillTexture;
		pSnowUphillTexture = NULL;
	}
	if (pBushTexture)
	{
		delete pBushTexture;
		pBushTexture = NULL;
	}
	if (pSwampTexture)
	{
		delete pSwampTexture;
		pSwampTexture = NULL;
	}
	if (pJumpTexture)
	{
		delete pJumpTexture;
		pJumpTexture = NULL;
	}
	if (pSnowTexture)
	{
		delete pSnowTexture;
		pSnowTexture = NULL;
	}
	if (pEmptySnowTexture)
	{
		delete pEmptySnowTexture;
		pEmptySnowTexture = NULL;
	}
	if (pIceTexture)
	{
		delete pIceTexture;
		pIceTexture = NULL;
	}

	if (pWaveTexture)
	{
		delete pWaveTexture;
		pWaveTexture = NULL;
	}
	if (pWaveNormalMap)
	{
		delete pWaveNormalMap;
		pWaveNormalMap = NULL;
	}
	if (pUI_VoxelIcons)
	{
		delete pUI_VoxelIcons;
		pUI_VoxelIcons = NULL;
	}
	if (pUI_VoxelPocket)
	{
		delete pUI_VoxelPocket;
		pUI_VoxelPocket = NULL;
	}
	if (pUI_VoxelActivated)
	{
		delete pUI_VoxelActivated;
		pUI_VoxelActivated = NULL;
	}
	if (pUI_Respawning)
	{
		delete pUI_Respawning;
		pUI_Respawning = NULL;
	}
	if (pUI_Stamina)
	{
		delete pUI_Stamina;
		pUI_Stamina = NULL;
	}
	if (pUI_Numbers)
	{
		delete pUI_Numbers;
		pUI_Numbers = NULL;
	}
	if (pUI_Keyboard)
	{
		delete pUI_Keyboard;
		pUI_Keyboard = NULL;
	}
	if (pUI_Result)
	{
		delete pUI_Result;
		pUI_Result = NULL;
	}
	if (pEffect_ParticleBlue0)
	{
		delete pEffect_ParticleBlue0;
		pEffect_ParticleBlue0 = NULL;
	}
	if (pd3dTextureArrayVoxel)
	{
		pd3dTextureArrayVoxel->Release();
		pd3dTextureArrayVoxel = NULL;
	}
		
		
	if (ppd3dSkyTexture)
	{
		(*ppd3dSkyTexture)->Release();
		delete ppd3dSkyTexture;
		ppd3dSkyTexture = NULL;
	}

	if (ppd3dWaveTexture)
	{
		(*ppd3dWaveTexture)->Release();
		delete ppd3dWaveTexture;
		ppd3dWaveTexture = NULL;
	}

	if (ppd3dWaveNormalMap)
	{
		(*ppd3dWaveNormalMap)->Release();
		delete ppd3dWaveNormalMap;
		ppd3dWaveNormalMap = NULL;
	}

	for (int i = 0; i < 10; i++)
	{
		ppd3dTextureVoxel[i]->Release();
	}
	delete[]ppd3dTextureVoxel;
	ppd3dTextureVoxel = NULL;
	
	for (int i = 0; i < 8; i++)
	{
		ppd3dTextureUI[i]->Release();
	}
	delete[]ppd3dTextureUI;
	ppd3dTextureUI = NULL;
	
	for (int i = 0; i < 1; i++)
	{
		ppd3dTextureEffect[i]->Release();
	}
	delete[]ppd3dTextureEffect;
	ppd3dTextureEffect = NULL;
	
	for (int i = 0; i < 2; i++)
	{
		ppd3dTexturePlayer[i]->Release();
	}
	delete[]ppd3dTexturePlayer;
	ppd3dTexturePlayer = NULL;
	
	for (int i = 0; i < 1; i++)
	{
		ppd3dSamplerState[i]->Release();
	}
	delete[]ppd3dSamplerState;
	ppd3dSamplerState = NULL;
}