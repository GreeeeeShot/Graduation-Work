#include "stdafx.h"
#include "LightShaftRenderer.h"


CLightShaftRenderer::CLightShaftRenderer()
{
	m_pLightSource = NULL;
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
	//// ȭ�� ũ��� �ý��������� ������ �ڿ��� �� �޼��尡 �����Ѵ�.
	m_fLightAndShaftMapW = fLightShaftMapW;
	m_fLightAndShaftMapH = fLightShaftMapH;

	// ����Ʈ&����Ʈ�� �ؽ��ĸ� �����Ѵ�.
	D3D11_TEXTURE2D_DESC LightShaftMapDesc;
	LightShaftMapDesc.Width = m_fLightAndShaftMapW;
	LightShaftMapDesc.Height = m_fLightAndShaftMapH;
	LightShaftMapDesc.MipLevels = 1;
	LightShaftMapDesc.ArraySize = 1;
	LightShaftMapDesc.Format = DXGI_FORMAT_B8G8R8A8_TYPELESS;
	LightShaftMapDesc.SampleDesc.Count = 1;
	LightShaftMapDesc.SampleDesc.Quality = 0;
	LightShaftMapDesc.Usage = D3D11_USAGE_DEFAULT;
	LightShaftMapDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	LightShaftMapDesc.CPUAccessFlags = 0;
	LightShaftMapDesc.MiscFlags = 0;

	ID3D11Texture2D* pLightMap = 0;
	pd3dDevice->CreateTexture2D(&LightShaftMapDesc, 0, &pLightMap);
	ID3D11Texture2D* pLightShaftMap = 0;
	pd3dDevice->CreateTexture2D(&LightShaftMapDesc, 0, &pLightShaftMap);

	// ����Ʈ����Ʈ�ʿ� ���� �� ���� SRV�並 �����Ѵ�.
	// 1. ����Ʈ�� SRV�� 
	D3D11_SHADER_RESOURCE_VIEW_DESC LightShaftMapSRVDesc;
	ZeroMemory(&LightShaftMapSRVDesc, sizeof(LightShaftMapSRVDesc));
	LightShaftMapSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	LightShaftMapSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	LightShaftMapSRVDesc.Texture2D.MipLevels = LightShaftMapDesc.MipLevels;
	LightShaftMapSRVDesc.Texture2D.MostDetailedMip = 0;
	pd3dDevice->CreateShaderResourceView(pLightMap, &LightShaftMapSRVDesc, &m_pLightMapSRV);
	// 2. ����Ʈ����Ʈ�� SRV�� 
	pd3dDevice->CreateShaderResourceView(pLightShaftMap, &LightShaftMapSRVDesc, &m_pLightShaftMapSRV);

	// ����Ʈ����Ʈ�ʿ� ���� �� ���� RTV�並 �����Ѵ�.
	// 1. ����Ʈ�� RTV�� 
	D3D11_RENDER_TARGET_VIEW_DESC LightShaftMapRTVDesc;
	ZeroMemory(&LightShaftMapRTVDesc, sizeof(LightShaftMapRTVDesc));
	LightShaftMapRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	LightShaftMapRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	LightShaftMapRTVDesc.Texture2D.MipSlice = 0;
	pd3dDevice->CreateRenderTargetView(pLightMap, &LightShaftMapRTVDesc, &m_pLightMapRTV);
	// 2. ����Ʈ����Ʈ�� RTV�� 
	pd3dDevice->CreateRenderTargetView(pLightShaftMap, &LightShaftMapRTVDesc, &m_pLightShaftMapRTV);

	if (pLightMap) pLightMap->Release();
	if (pLightShaftMap) pLightShaftMap->Release();
}

void CLightShaftRenderer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{

	// pass0 resource
	// X

	// pass1 resource
	// ����Ʈ���� ������ �ؽ��� �ڿ�
	// 'CreateLightShaftMapAndView'���� �����Ͽ���.

	// pass2 resource
	// �����ʰ� ����Ʈ����Ʈ���� ������ �ؽ��� �ڿ�
	// 'CreateLightShaftMapAndView'���� �����Ͽ���.

	// ����ȥ�� ���¸� �����Ѵ�.
	// BlendingResource�� ����
}

void CLightShaftRenderer::SetLightSource(CLightSource* pLightSource)
{
	m_pLightSource = pLightSource;
}

void CLightShaftRenderer::UpdateLightShaftPass0Variables(ID3D11DeviceContext *pd3dDeviceContext)
{
	// X
	// �ε������ pass1�� ����Ÿ�� ����
}

void CLightShaftRenderer::CLightShaftRenderer::UpdateLightShaftPass1Variables(ID3D11DeviceContext *pd3dDeviceContext)
{
	// PS ����Ʈ���� �����Ѵ�. 
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_LIGHT_TEXTURE, 1, &m_pLightMapSRV);
}

void CLightShaftRenderer::UpdateLightShaftPass2Variables(ID3D11DeviceContext *pd3dDeviceContext)
{
	// PS ����Ʈ����Ʈ���� �����Ѵ�.
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_LIGHT_SHAFT_TEXTURE, 1, &m_pLightShaftMapSRV);
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
	pd3dDeviceContext->OMSetRenderTargets(1, &m_pLightMapRTV, 0);		// ��������� �����Ѵ�.
}