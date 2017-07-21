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
	if (fFovDegree != 0.0f)		// 원근투영을 적용한다.
	{
		D3DXMatrixPerspectiveFovLH(&m_d3dxmtxProjection, (float)D3DXToRadian(fFovDegree), fLightEmitW/fLightEmitH, fLightStart, fLightEnd);
	}
	else						// 직교투영을 적용한다.
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
	m_pd3dcbLightEmit = NULL;
	m_pd3dcbShadowMatrix = NULL;
	m_pSamplerPCF = NULL;
	D3DXMatrixIdentity(&m_d3dxmtxShadowMatrix);
}


CShadowRenderer::~CShadowRenderer()
{
}

void CShadowRenderer::CreateShadowMapAndView(ID3D11Device *pd3dDevice, float fShadowMapW, float fShadowMapH)
{
	// 화면 크기와 시스템적으로 연관된 자원은 이 메서드가 생성한다.
	m_fShadowMapW = fShadowMapW;
	m_fShadowMapH = fShadowMapH;

	// 쉐도우맵으로의 출력에 쓰일 뷰포트를 설정한다.
	m_d3dxViewport.TopLeftX = 0.0f;
	m_d3dxViewport.TopLeftY = 0.0f;
	m_d3dxViewport.Width = m_fShadowMapW;
	m_d3dxViewport.Height = m_fShadowMapH;
	m_d3dxViewport.MinDepth = 0.0f;
	m_d3dxViewport.MaxDepth = 1.0f;

	// 쉐도우맵 텍스쳐를 생성한다.
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	shadowMapDesc.Width = m_fShadowMapW;
	shadowMapDesc.Height = m_fShadowMapH;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;			// 셰이더 리소스 및 깊이스텐실 뷰로 쓰일 것이므로 무형식으로 지정
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;
	shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowMapDesc.CPUAccessFlags = 0;
	shadowMapDesc.MiscFlags = 0;

	ID3D11Texture2D* shadowMap = 0;
	pd3dDevice->CreateTexture2D(&shadowMapDesc, 0, &shadowMap);

	// 쉐도우맵에 대한 두 가지 뷰를 설정한다.
	// 1. 뎁스 스텐실 뷰 (pass0 에서 사용된다.)
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	pd3dDevice->CreateDepthStencilView(shadowMap, &dsvDesc, &m_pShadowMapDSV);

	// 2. 셰이더 리소스 뷰 (pass1 에서 사용된다.)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = shadowMapDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	pd3dDevice->CreateShaderResourceView(shadowMap, &srvDesc, &m_pShadowMapSRV);

	if (shadowMap) shadowMap->Release();
}

void CShadowRenderer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	// pass0 resource
	// 1. 광원의 View와 Projection matrix자원
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_LIGHT_EMIT);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbLightEmit);

	// pass1 resource
	// 1. 광원의 View와 Projection 그리고 texture matrix자원
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_SHADOW_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbShadowMatrix);

	// 2. 쉐도우맵 자원
	// CreateShadowMapAndView 메서드가 생성

	// 3. PCF
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &m_pSamplerPCF);
}

void CShadowRenderer::UpdateShaderPass0Variables(ID3D11DeviceContext *pd3dDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	/*상수 버퍼의 메모리 주소를 가져와서 카메라 변환 행렬과 투영 변환 행렬을 복사한다. 쉐이더에서 행렬의 행과 열이 바뀌는 것에 주의하라.*/
	pd3dDeviceContext->Map(m_pd3dcbLightEmit, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_LIGHT_EMIT *pcbViewProjection = (VS_CB_LIGHT_EMIT *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxView, &m_cLightSource.m_d3dxmtxView);
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxProjection, &m_cLightSource.m_d3dxmtxProjection);
	pd3dDeviceContext->Unmap(m_pd3dcbLightEmit, 0);

	//상수 버퍼를 슬롯(VS_SLOT_CAMERA)에 설정한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_LIGHT_EMIT, 1, &m_pd3dcbLightEmit);
}

void CShadowRenderer::UpdateShaderPass1Variables(ID3D11DeviceContext *pd3dDeviceContext)
{
	D3DXMatrixIdentity(&m_d3dxmtxShadowMatrix);

	D3DXMatrixMultiply(&m_d3dxmtxShadowMatrix, &m_cLightSource.m_d3dxmtxView, &m_cLightSource.m_d3dxmtxProjection);
	D3DXMatrixMultiply(&m_d3dxmtxShadowMatrix, &m_d3dxmtxShadowMatrix, &m_d3dxmtxTextureMatrix);
	//월드 변환 행렬을 상수 버퍼에 복사한다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbShadowMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_SHADOW_MATRIX *pcbShadowMatrix = (VS_CB_SHADOW_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbShadowMatrix->m_d3dxmtxShadowMatrix, &m_d3dxmtxShadowMatrix);
	pd3dDeviceContext->Unmap(m_pd3dcbShadowMatrix, 0);
	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_SHADOW_MATRIX, 1, &m_pd3dcbShadowMatrix);

	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_SHADOW_TEXTURE, 1, &m_pShadowMapSRV);
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_PCF_SAMPLER_STATE, 1, &m_pSamplerPCF);
}

void CShadowRenderer::ResizeShadowMapAndView(ID3D11Device *pd3dDevice, float fShadowMapW, float fShadowMapH)
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

	CreateShadowMapAndView(pd3dDevice, fShadowMapW, fShadowMapH);
}

// pass0에서 쓰인다. pass1 진행 시 원상태로 복구시켜야 한다.
void CShadowRenderer::BindDSVAndSetNullRenderTarget(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->RSSetViewports(1, &m_d3dxViewport);					// 쉐도우맵으로의 출력을 위한 뷰포트 세팅
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	pd3dDeviceContext->OMSetRenderTargets(1, renderTargets, m_pShadowMapDSV);		// 쉐도우맵을 장착한다.
	pd3dDeviceContext->ClearDepthStencilView(m_pShadowMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}