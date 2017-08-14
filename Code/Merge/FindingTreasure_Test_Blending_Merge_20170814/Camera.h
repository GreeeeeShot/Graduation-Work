#pragma once
struct VS_CB_CAMERA
{
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;
};

//"CPlayer"�� �÷��̾ ��Ÿ���� Ŭ�����̴�.
class CPlayer;

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

protected:
	//ī�޶� ��ȯ ��İ� ���� ��ȯ ����� ��Ÿ���� ��� ������ �����Ѵ�.
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;

	//��-��Ʈ�� ��Ÿ���� ��� ������ �����Ѵ�.
	D3D11_VIEWPORT m_d3dViewport;
	D3DXVECTOR3 m_d3dxvEyePosition;

	// ����ü�� 6�� ���(���� ��ǥ��)�� ��Ÿ����.
	D3DXPLANE m_d3dxFrustumPlanes[6];
	

	//ī�޶� ��ȯ ��İ� ���� ��ȯ ����� ���� ��� ���� �������̽� �����͸� �����Ѵ�.
	ID3D11Buffer *m_pd3dcbCamera;

	//ī�޶� ����� �÷��̾� ��ü�� ���� �����͸� �����Ѵ�.
	CPlayer *m_pPlayer;

public:
	bool m_bIsInnerCulling;
	
	void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }

	//��-��Ʈ�� �����ϴ� ��� �Լ��� �����Ѵ�.
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetPosition(D3DXVECTOR3 d3dxvPosition) { m_d3dxvEyePosition = d3dxvPosition; }
	D3DXVECTOR3 GetPosition() const { return m_d3dxvEyePosition; }
	D3DXVECTOR3 GetLook() { return D3DXVECTOR3(m_d3dxmtxView._13, m_d3dxmtxView._23, m_d3dxmtxView._33); }

	//ī�޶� ��ȯ ��İ� ���� ��ȯ ����� �����ϴ� ��� �Լ��� �����Ѵ�.
	void GenerateViewMatrix(D3DXVECTOR3 d3dxvEyePosition, D3DXVECTOR3 d3dxvLookAt, D3DXVECTOR3 d3dxvUp);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	void SetViewMatrix(D3DXMATRIX* pd3dxmtxViewMatrix);
	void SetProjectionMatrix(D3DXMATRIX* pd3dxmtxProjMatrix);

	D3DXMATRIX GetViewMatrix(void);
	D3DXMATRIX GetProjectionMatrix(void);
	D3D11_VIEWPORT GetViewport(void);
	//����ü�� 6�� ����� ����Ѵ�.
	void CalculateFrustumPlanes();
	//�ٿ�� �ڽ��� ����ü�� ������ ���Եǰų� �Ϻζ� ���ԵǴ� ���� �˻��Ѵ�.
	bool CCamera::IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	bool IsInFrustum(AABB *pAABB);

	//��� ���۸� �����ϰ� ������ �����ϴ� ��� �Լ��� �����Ѵ�.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);				// ī�޶�� �����Ͽ� ���̴��� ������ �ڿ� ������ �����Ѵ�.
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);	// ī�޶� ��� ���۸� �����Ѵ�.
};

