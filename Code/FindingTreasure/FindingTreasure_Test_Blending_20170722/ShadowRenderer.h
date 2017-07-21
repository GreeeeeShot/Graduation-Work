#pragma once

// static CShadowRenderer* cShadowRendererInst = new CShadowRenderer();

struct VS_CB_LIGHT_EMIT
{
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;
};

struct VS_CB_SHADOW_MATRIX
{
	D3DXMATRIX m_d3dxmtxShadowMatrix;
};

class CLightSource
{
public:
	CLightSource();
	~CLightSource();

	D3DXMATRIX m_d3dxmtxView;					// ���� ����
	D3DXMATRIX m_d3dxmtxProjection;				// �� ����

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
	ID3D11DepthStencilView* m_pShadowMapDSV;		// pass0 -> ����� ���̸� ����ϴµ� ���δ�.
	ID3D11ShaderResourceView* m_pShadowMapSRV;	// pass1 -> ���� ����������� �ϴµ� ���δ�.
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
	void UpdateShaderPass0Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void UpdateShaderPass1Variables(ID3D11DeviceContext *pd3dDeviceContext);
	void ResizeShadowMapAndView(ID3D11Device *pd3dDevice, float fShadowMapW, float fShadowMapH);
	void BindDSVAndSetNullRenderTarget(ID3D11DeviceContext *pd3dDeviceContext);
};

