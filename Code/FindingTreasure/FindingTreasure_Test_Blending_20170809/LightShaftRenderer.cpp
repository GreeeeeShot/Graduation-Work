#include "stdafx.h"
#include "LightShaftRenderer.h"

struct PS_CB_LIGHT_SOURCE_POS_T
{
	D3DXVECTOR2 m_d3dxvLightSourcePosT;		
	D3DXVECTOR2 pad;
};

CLightShaftRenderer::CLightShaftRenderer()
{
	m_pd3dcbLightSourcePosT = NULL;
	m_pLightSource = NULL;
	m_pLightShaftProjMesh = NULL;
	m_pLightShaftPassShader = NULL;
	m_fLightAndShaftMapW = FRAME_BUFFER_WIDTH;
	m_fLightAndShaftMapH = FRAME_BUFFER_HEIGHT;
	m_pLightMapRTV = NULL;				// pass0 
	m_pLightMapSRV = NULL;			// pass1
	m_pLightShaftMapRTV = NULL;		// pass1
	m_pLightShaftMapSRV = NULL;		// pass2
}


CLightShaftRenderer::~CLightShaftRenderer()
{
}


void CLightShaftRenderer::CreateLightShaftSRVAndRTV(ID3D11Device *pd3dDevice, float fLightShaftMapW, float fLightShaftMapH)
{
	//// 화면 크기와 시스템적으로 연관된 자원은 이 메서드가 생성한다.
	m_fLightAndShaftMapW = fLightShaftMapW;
	m_fLightAndShaftMapH = fLightShaftMapH;

	// 라이트&셰프트맵 텍스쳐를 생성한다.
	D3D11_TEXTURE2D_DESC LightShaftMapDesc;
	ZeroMemory(&LightShaftMapDesc, sizeof(LightShaftMapDesc));
	LightShaftMapDesc.Width = m_fLightAndShaftMapW;
	LightShaftMapDesc.Height = m_fLightAndShaftMapH;
	LightShaftMapDesc.MipLevels = 1;
	LightShaftMapDesc.ArraySize = 1;
	LightShaftMapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	LightShaftMapDesc.SampleDesc.Count = 1;
	LightShaftMapDesc.SampleDesc.Quality = 0;
	LightShaftMapDesc.Usage = D3D11_USAGE_DEFAULT;
	LightShaftMapDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	LightShaftMapDesc.CPUAccessFlags = 0;
	LightShaftMapDesc.MiscFlags = 0;

	ID3D11Texture2D* pLightMap = 0;
	pd3dDevice->CreateTexture2D(&LightShaftMapDesc, NULL, &pLightMap);
	ID3D11Texture2D* pLightShaftMap = 0;
	pd3dDevice->CreateTexture2D(&LightShaftMapDesc, NULL, &pLightShaftMap);

	// 라이트셰프트맵에 대한 두 가지 SRV뷰를 설정한다.
	// 1. 라이트맵 SRV뷰 
	D3D11_SHADER_RESOURCE_VIEW_DESC LightShaftMapSRVDesc;
	ZeroMemory(&LightShaftMapSRVDesc, sizeof(LightShaftMapSRVDesc));
	LightShaftMapSRVDesc.Format = LightShaftMapDesc.Format;
	LightShaftMapSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	LightShaftMapSRVDesc.Texture2D.MipLevels = LightShaftMapDesc.MipLevels;
	LightShaftMapSRVDesc.Texture2D.MostDetailedMip = 0;
	pd3dDevice->CreateShaderResourceView(pLightMap, &LightShaftMapSRVDesc, &m_pLightMapSRV);
	// 2. 라이트셰프트맵 SRV뷰 
	pd3dDevice->CreateShaderResourceView(pLightShaftMap, &LightShaftMapSRVDesc, &m_pLightShaftMapSRV);

	// 라이트셰프트맵에 대한 두 가지 RTV뷰를 설정한다.
	// 1. 라이트맵 RTV뷰 
	D3D11_RENDER_TARGET_VIEW_DESC LightShaftMapRTVDesc;
	ZeroMemory(&LightShaftMapRTVDesc, sizeof(LightShaftMapRTVDesc));
	LightShaftMapRTVDesc.Format = LightShaftMapDesc.Format;
	LightShaftMapRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	LightShaftMapRTVDesc.Texture2D.MipSlice = 0;
	pd3dDevice->CreateRenderTargetView(pLightMap, &LightShaftMapRTVDesc, &m_pLightMapRTV);
	// 2. 라이트셰프트맵 RTV뷰 
	pd3dDevice->CreateRenderTargetView(pLightShaftMap, &LightShaftMapRTVDesc, &m_pLightShaftMapRTV);

	if (pLightMap) pLightMap->Release();
	if (pLightShaftMap) pLightShaftMap->Release();
}

void CLightShaftRenderer::DestroyLightShaftRenderer(void)
{
	if(m_pd3dcbLightSourcePosT) m_pd3dcbLightSourcePosT->Release();
	if (m_pLightMapRTV) m_pLightMapRTV->Release();				// pass0 
	if (m_pLightMapSRV) m_pLightMapSRV->Release();			// pass1
	if (m_pLightShaftMapRTV) m_pLightShaftMapRTV->Release();		// pass1
	if (m_pLightShaftMapSRV) m_pLightShaftMapSRV->Release();		// pass2
}

void CLightShaftRenderer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{

	// pass0 resource
	// X

	// pass1 resource
	// 라이트맵을 세팅할 텍스쳐 자원
	// 'CreateLightShaftMapAndView'에서 생성하였다.
	// 텍스쳐 공간 내 광원 위치 정보를 담는 자원.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(PS_CB_LIGHT_SOURCE_POS_T);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbLightSourcePosT);

	// pass2 resource
	// 렌더맵과 라이트셰프트맵을 세팅할 텍스쳐 자원
	// 'CreateLightShaftMapAndView'에서 생성하였다.

	// 가산혼합 상태를 정의한다.
	// BlendingResource에 정의
}

void CLightShaftRenderer::SetLightSource(CLightSource* pLightSource)
{
	m_pLightSource = pLightSource;
}

void CLightShaftRenderer::SetCamera(CCamera* pCamera)
{
	m_pCamera = pCamera;
}

void CLightShaftRenderer::SetLightShaftProjMesh(CMesh *pMesh)
{
	m_pLightShaftProjMesh = pMesh;
}

void CLightShaftRenderer::SetLightShaftPassShader(CShader *pShader)
{
	m_pLightShaftPassShader = pShader;
}

void CLightShaftRenderer::UpdateLightShaftPass0Variables(ID3D11DeviceContext *pd3dDeviceContext)
{
	// X
	// 셰도우맵의 pass1의 렌더타겟 설정
}

void CLightShaftRenderer::UpdateLightShaftPass1Variables(ID3D11DeviceContext *pd3dDeviceContext)
{
	// 라이트 셰프트 맵을 생성하기 위한 자원 세팅
	D3DXVECTOR2 fLightPosT;
	D3DXMATRIX d3dxmtxViewProjTexMatrix;

	D3DXMatrixIdentity(&d3dxmtxViewProjTexMatrix);

	D3DXMatrixMultiply(&d3dxmtxViewProjTexMatrix, &m_pCamera->GetViewMatrix(), &m_pCamera->GetProjectionMatrix());
	D3DXMatrixMultiply(&d3dxmtxViewProjTexMatrix, &d3dxmtxViewProjTexMatrix, &m_d3dxmtxTextureMatrix);

	D3DXVECTOR3 d3dxvLightPos;

	D3DXVec3TransformCoord(&d3dxvLightPos, &m_pLightSource->GetPosition(), &d3dxmtxViewProjTexMatrix);		// 원근 나누기가 된 상태

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLightSourcePosT, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	PS_CB_LIGHT_SOURCE_POS_T *pcbLightSourcePos = (PS_CB_LIGHT_SOURCE_POS_T *)d3dMappedResource.pData;
	ZeroMemory(pcbLightSourcePos, sizeof(PS_CB_LIGHT_SOURCE_POS_T));
	pcbLightSourcePos->m_d3dxvLightSourcePosT = D3DXVECTOR2(d3dxvLightPos.x, d3dxvLightPos.y);				// 텍스쳐 공간 좌표이다.
	pcbLightSourcePos->pad = D3DXVECTOR2(0.0f, 0.0f);
	pd3dDeviceContext->Unmap(m_pd3dcbLightSourcePosT, 0);
	// PS 라이트맵을 세팅한다. 
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT_SOURCE_POS_T, 1, &m_pd3dcbLightSourcePosT);		// 광원의 위치 정보 세팅
	ID3D11ShaderResourceView* null[] = { nullptr};
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_LIGHT_TEXTURE, 1, null);
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_LIGHT_TEXTURE, 1, &m_pLightMapSRV);						// 라이트 셰프트맵을 만들 라이트맵 세팅
	//pd3dDeviceContext->PSSetSamplers(0x00, 1, &CTextureResource::ppd3dSamplerState[0]);
}

void CLightShaftRenderer::UpdateLightShaftPass2Variables(ID3D11DeviceContext *pd3dDeviceContext)
{
	// PS 라이트셰프트맵을 세팅한다.
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_LIGHT_SHAFT_TEXTURE, 1, &m_pLightShaftMapSRV);
	//pd3dDeviceContext->PSSetSamplers(0x00, 1, &CTextureResource::ppd3dSamplerState[0]);
}

void CLightShaftRenderer::ResizeLightShaftMapAndView(ID3D11Device *pd3dDevice, float fLightShaftMapW, float fLightShaftMapH)
{
	if (m_pLightMapSRV)
	{
		m_pLightMapSRV->Release();
		m_pLightMapSRV = NULL;
	}
	if (m_pLightShaftMapSRV)
	{
		m_pLightShaftMapSRV->Release();
		m_pLightShaftMapSRV = NULL;
	}

	if (m_pLightMapRTV)
	{
		m_pLightMapRTV->Release();
		m_pLightMapRTV = NULL;
	}
	if (m_pLightShaftMapRTV)
	{
		m_pLightShaftMapRTV->Release();
		m_pLightShaftMapRTV = NULL;
	}

	CreateLightShaftSRVAndRTV(pd3dDevice, fLightShaftMapW, fLightShaftMapH);
}

void CLightShaftRenderer::BindLightShaftMapSRVToRenderTarget(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->OMSetRenderTargets(1, &m_pLightMapRTV, 0);		// 쉐도우맵을 장착한다.
}

void CLightShaftRenderer::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pLightShaftPassShader) m_pLightShaftPassShader->Render(pd3dDeviceContext);
	if (m_pLightShaftProjMesh) m_pLightShaftProjMesh->Render(pd3dDeviceContext);
}