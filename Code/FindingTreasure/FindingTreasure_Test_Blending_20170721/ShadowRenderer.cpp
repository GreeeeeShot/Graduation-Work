#include "stdafx.h"
#include "ShadowRenderer.h"

CLightSource::CLightSource()
{
	D3DXMatrixIdentity(&m_d3dxmtxView);
	D3DXMatrixIdentity(&m_d3dxmtxProjection);
	m_d3dxvPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
}

CLightSource::~CLightSource()
{

}


void CLightSource::CreateViewMatrix(D3DXVECTOR3 d3dxvPos, D3DXVECTOR3 d3dxvTarget, D3DXVECTOR3 d3dxvUp)
{
	m_d3dxvPos = d3dxvPos;
	m_d3dxvTarget = d3dxvTarget;
	m_d3dxvUp = d3dxvUp;
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &d3dxvPos, &d3dxvTarget, &d3dxvUp);
}

void CLightSource::CreateProjectionMatrix(float fFovDegree, float fLightEmitW, float fLightEmitH, float fLightStart, float fLightEnd)
{
	if (fFovDegree != 0.0f)		// ���������� �����Ѵ�.
	{
		D3DXMatrixPerspectiveFovLH(&m_d3dxmtxProjection, (float)D3DXToRadian(fFovDegree), fLightEmitW/fLightEmitH, fLightStart, fLightEnd);
	}
	else						// ���������� �����Ѵ�.
	{
		D3DXMatrixOrthoLH(&m_d3dxmtxProjection, fLightEmitW, fLightEmitH, fLightStart, fLightEnd);
	}
}

void CLightSource::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	m_d3dxvPos = d3dxvPosition;
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &m_d3dxvPos, &m_d3dxvTarget, &m_d3dxvUp);
}

D3DXVECTOR3 CLightSource::GetPosition(void)
{
	return m_d3dxvPos;
}

void CLightSource::SetTarget(D3DXVECTOR3 d3dxvTarget)
{
	m_d3dxvTarget = d3dxvTarget;
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &m_d3dxvPos, &m_d3dxvTarget, &m_d3dxvUp);
}

D3DXVECTOR3 CLightSource::GetTarget(void)
{
	return m_d3dxvTarget;
}

void CLightSource::SetUp(D3DXVECTOR3 d3dxvUp)
{
	m_d3dxvUp = d3dxvUp;
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &m_d3dxvPos, &m_d3dxvTarget, &m_d3dxvUp);
}

D3DXVECTOR3 CLightSource::GetUp(void)
{
	return m_d3dxvUp;
}


CShadowRenderer::CShadowRenderer()
{
	m_fShadowMapW = FRAME_BUFFER_WIDTH;
	m_fShadowMapH = FRAME_BUFFER_HEIGHT;
	m_pShadowMapDSV = NULL;		
	m_pShadowMapSRV = NULL;
}


CShadowRenderer::~CShadowRenderer()
{
}

void CShadowRenderer::InitShadowRenderer(ID3D11Device *pd3dDevice, float fShadowMapW, float fShadowMapH)
{
	m_fShadowMapW = fShadowMapW;
	m_fShadowMapH = fShadowMapH;

	// ������������� ��¿� ���� ����Ʈ�� �����Ѵ�.
	m_d3dxViewport.TopLeftX = 0.0f;
	m_d3dxViewport.TopLeftY = 0.0f;
	m_d3dxViewport.Width = m_fShadowMapW;
	m_d3dxViewport.Height = m_fShadowMapH;
	m_d3dxViewport.MinDepth = 0.0f;
	m_d3dxViewport.MaxDepth = 0.0f;

	// ������� �ؽ��ĸ� �����Ѵ�.
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	shadowMapDesc.Width = m_fShadowMapW;
	shadowMapDesc.Height = m_fShadowMapH;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;			// ���̴� ���ҽ� �� ���̽��ٽ� ��� ���� ���̹Ƿ� ���������� ����
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;
	shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowMapDesc.CPUAccessFlags = 0;
	shadowMapDesc.MiscFlags = 0;

	ID3D11Texture2D* shadowMap = 0;
	pd3dDevice->CreateTexture2D(&shadowMapDesc, 0, &shadowMap);

	// ������ʿ� ���� �� ���� �並 �����Ѵ�.
	// 1. ���� ���ٽ� �� (pass0 ���� ���ȴ�.)
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	pd3dDevice->CreateDepthStencilView(shadowMap, &dsvDesc, &m_pShadowMapDSV);

	// 2. ���̴� ���ҽ� �� (pass1 ���� ���ȴ�.)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = shadowMapDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	pd3dDevice->CreateShaderResourceView(shadowMap, &srvDesc, &m_pShadowMapSRV);

	if (shadowMap) shadowMap->Release();
}

void CShadowRenderer::ResizeShadowMap(ID3D11Device *pd3dDevice, float fShadowMapW, float fShadowMapH)
{
	if (m_pShadowMapDSV)
	{
		m_pShadowMapDSV->Release();
		m_pShadowMapDSV = NULL;
	}
	if (m_pShadowMapSRV)
	{
		m_pShadowMapSRV->Release();
		m_pShadowMapSRV = NULL;
	}

	InitShadowRenderer(pd3dDevice, fShadowMapW, fShadowMapH);
}

// pass0���� ���δ�. pass1 ���� �� �����·� �������Ѿ� �Ѵ�.
void CShadowRenderer::BindDSVAndSetNullRenderTarget(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->RSSetViewports(1, &m_d3dxViewport);					// ������������� ����� ���� ����Ʈ ����
	pd3dDeviceContext->OMSetRenderTargets(1, NULL, m_pShadowMapDSV);		// ��������� �����Ѵ�.
	pd3dDeviceContext->ClearDepthStencilView(m_pShadowMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}