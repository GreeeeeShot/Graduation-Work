#pragma once
//#include "LightSource.h"

// static CShadowRenderer* cShadowRendererInst = new CShadowRenderer();
class CShader;
class CLightSource;

struct VS_CB_SHADOW_MATRIX
{
	D3DXMATRIX m_d3dxmtxShadowMatrix;
};


class CShadowRenderer
{
public:
	CShadowRenderer();
	~CShadowRenderer();

public:
	CLightSource *m_pLightSource;
	D3D11_VIEWPORT m_d3dxViewport;
	float m_fShadowMapW;
	float m_fShadowMapH;
	ID3D11DepthStencilView* m_pShadowMapDSV;		// pass0 -> 상대적 깊이를 기록하는데 쓰인다.
	ID3D11ShaderResourceView* m_pShadowMapSRV;	// pass1 -> 실제 쉐도우매핑을 하는데 쓰인다.
	const D3DXMATRIX m_d3dxmtxTextureMatrix =
	{
		0.5f,	0.0f,	0.0f,	0.0f,
		0.0f,	-0.5f,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,	0.0f,	1.0f
	};

	D3DXMATRIX m_d3dxmtxShadowMatrix;
	ID3D11Buffer *m_pd3dcbLightEmit;
	ID3D11Buffer *m_pd3dcbShadowMatrix;
	ID3D11SamplerState *m_pSamplerPCF;

	void CreateShadowMapAndView(ID3D11Device *pd3dDevice, float fShadowMapW = FRAME_BUFFER_WIDTH, float fShadowMapH = FRAME_BUFFER_HEIGHT);
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void SetLightSource(CLightSource* pLightSource);
	void UpdateShaderPass0Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void UpdateShaderPass1Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void ResizeShadowMapAndView(ID3D11Device *pd3dDevice, float fShadowMapW, float fShadowMapH);
	void BindDSVAndSetNullRenderTarget(ID3D11DeviceContext *pd3dDeviceContext);
};

