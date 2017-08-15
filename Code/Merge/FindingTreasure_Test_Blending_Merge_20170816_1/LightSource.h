#pragma once
struct VS_CB_LIGHT_EMIT
{
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;
};


class CLightSource
{
public:
	CLightSource();
	~CLightSource();

	// 광원의 형태를 정의한다.
	CMesh *m_pMesh;
	D3DXMATRIX m_d3dxmtxWorld;
	CShader *m_pShader;

	D3DXMATRIX m_d3dxmtxView;					// 위상 정의
	D3DXMATRIX m_d3dxmtxProjection;				// 광 정의

	D3DXVECTOR3 m_d3dxvPos;
	D3DXVECTOR3 m_d3dxvTarget;
	D3DXVECTOR3 m_d3dxvUp;

	CCamera m_CameraForCulling;

	void SetMesh(CMesh *pMesh);
	void SetShader(CShader *pShader);
	void Render(ID3D11DeviceContext *pd3dDeviceContext);
	void CreateViewMatrix(D3DXVECTOR3 d3dxvPos, D3DXVECTOR3 d3dxvTarget, D3DXVECTOR3 d3dxvUp);
	void CreateProjectionMatrix(float fFovDegree, float fLightEmitW, float fLightEmitH, float fLightStart, float fLightEnd);
	void CalculateFrustumPlanes(void);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition(void);
	void SetTarget(D3DXVECTOR3 d3dxvTarget);
	D3DXVECTOR3 GetTarget(void);
	void SetUp(D3DXVECTOR3 d3dxvUp);
	D3DXVECTOR3 GetUp(void);
};