#pragma once

class CLightSource
{
public:
	CLightSource();
	~CLightSource();

	D3DXMATRIX m_d3dxmtxView;					// 위상 정의
	D3DXMATRIX m_d3dxmtxProjection;				// 광 정의

	D3DXVECTOR3 m_d3dxvPos;
	D3DXVECTOR3 m_d3dxvTarget;
	D3DXVECTOR3 m_d3dxvUp;

	void CreateViewMatrix(D3DXVECTOR3 d3dxvPos, D3DXVECTOR3 d3dxvTarget, D3DXVECTOR3 d3dxvUp);
	void CreateProjectionMatrix(float fFovDegree, float fLightEmitW, float fLightEmitH, float fLightStart, float fLightEnd);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition(void);
	void SetTarget(D3DXVECTOR3 d3dxvTarget);
	D3DXVECTOR3 GetTarget(void);
	void SetUp(D3DXVECTOR3 d3dxvUp);
	D3DXVECTOR3 GetUp(void);
};

class CShadowRenderer
{
public:
	CShadowRenderer();
	~CShadowRenderer();

public:
	CLightSource m_cLightSource;
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

	void InitShadowRenderer(ID3D11Device *pd3dDevice, float fShadowMapW = FRAME_BUFFER_WIDTH, float fShadowMapH = FRAME_BUFFER_HEIGHT);
	void ResizeShadowMap(ID3D11Device *pd3dDevice, float fShadowMapW, float fShadowMapH);
	void BindDSVAndSetNullRenderTarget(ID3D11DeviceContext *pd3dDeviceContext);
};

