#pragma once
class CLightShaftRenderer
{
public:
	CLightShaftRenderer();
	~CLightShaftRenderer();

public:
	CLightSource *m_pLightSource;
	float m_fLightAndShaftMapW;
	float m_fLightAndShaftMapH;
	
	ID3D11RenderTargetView* m_pLightMapRTV;				// pass0 
	ID3D11ShaderResourceView* m_pLightMapSRV;			// pass1
	ID3D11RenderTargetView* m_pLightShaftMapRTV;		// pass1
	ID3D11ShaderResourceView* m_pLightShaftMapSRV;		// pass2

	void CreateLightShaftSRVAndRTV(ID3D11Device *pd3dDevice, float fLightShaftMapW = FRAME_BUFFER_WIDTH, float fLightShaftMapH = FRAME_BUFFER_HEIGHT);
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void SetLightSource(CLightSource* pLightSource);
	void UpdateLightShaftPass0Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void UpdateLightShaftPass1Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void UpdateLightShaftPass2Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void ResizeLightShaftMapAndView(ID3D11Device *pd3dDevice, float fLightShaftMapW, float fLightShaftMapH);
	void BindLightShaftMapSRVToRenderTarget(ID3D11DeviceContext *pd3dDeviceContext);
};

