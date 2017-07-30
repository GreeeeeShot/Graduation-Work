#pragma once
//#include "LightResource.h"
class CLightSource;

class CLightShaftRenderer
{
public:
	CLightShaftRenderer();
	~CLightShaftRenderer();

public:
	CLightSource *m_pLightSource;
	CMesh *m_pLightShaftProjMesh;
	CShader *m_pLightShaftPassShader;
	CCamera *m_pCamera;

	float m_fLightAndShaftMapW;
	float m_fLightAndShaftMapH;
	
	ID3D11Buffer *m_pd3dcbLightSourcePosT;
	ID3D11RenderTargetView* m_pLightMapRTV;				// pass0 
	ID3D11ShaderResourceView* m_pLightMapSRV;			// pass1
	ID3D11RenderTargetView* m_pLightShaftMapRTV;		// pass1
	ID3D11ShaderResourceView* m_pLightShaftMapSRV;		// pass2

	const D3DXMATRIX m_d3dxmtxTextureMatrix =
	{
		0.5f,	0.0f,	0.0f,	0.0f,
		0.0f,	-0.5f,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,	0.0f,	1.0f
	};

	void CreateLightShaftSRVAndRTV(ID3D11Device *pd3dDevice, float fLightShaftMapW = FRAME_BUFFER_WIDTH, float fLightShaftMapH = FRAME_BUFFER_HEIGHT);
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void SetLightSource(CLightSource* pLightSource);
	void SetLightShaftProjMesh(CMesh *pMesh);
	void SetLightShaftPassShader(CShader *pShader);
	void SetCamera(CCamera *pCamera);	
	void UpdateLightShaftPass0Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void UpdateLightShaftPass1Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void UpdateLightShaftPass2Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void ResizeLightShaftMapAndView(ID3D11Device *pd3dDevice, float fLightShaftMapW, float fLightShaftMapH);
	void BindLightShaftMapSRVToRenderTarget(ID3D11DeviceContext *pd3dDeviceContext);
	void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

