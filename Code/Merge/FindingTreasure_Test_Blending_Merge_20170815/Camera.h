#pragma once
struct VS_CB_CAMERA
{
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;
};

//"CPlayer"는 플레이어를 나타내는 클래스이다.
class CPlayer;

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

protected:
	//카메라 변환 행렬과 투영 변환 행렬을 나타내는 멤버 변수를 선언한다.
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;

	//뷰-포트를 나타내는 멤버 변수를 선언한다.
	D3D11_VIEWPORT m_d3dViewport;
	D3DXVECTOR3 m_d3dxvEyePosition;

	// 절두체의 6개 평면(월드 좌표계)를 나타낸다.
	D3DXPLANE m_d3dxFrustumPlanes[6];
	

	//카메라 변환 행렬과 투영 변환 행렬을 위한 상수 버퍼 인터페이스 포인터를 선언한다.
	ID3D11Buffer *m_pd3dcbCamera;

	//카메라에 연결된 플레이어 객체에 대한 포인터를 선언한다.
	CPlayer *m_pPlayer;

public:
	bool m_bIsInnerCulling;
	
	void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }

	//뷰-포트를 설정하는 멤버 함수를 선언한다.
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetPosition(D3DXVECTOR3 d3dxvPosition) { m_d3dxvEyePosition = d3dxvPosition; }
	D3DXVECTOR3 GetPosition() const { return m_d3dxvEyePosition; }
	D3DXVECTOR3 GetLook() { return D3DXVECTOR3(m_d3dxmtxView._13, m_d3dxmtxView._23, m_d3dxmtxView._33); }

	//카메라 변환 행렬과 투영 변환 행렬을 생성하는 멤버 함수를 선언한다.
	void GenerateViewMatrix(D3DXVECTOR3 d3dxvEyePosition, D3DXVECTOR3 d3dxvLookAt, D3DXVECTOR3 d3dxvUp);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	void SetViewMatrix(D3DXMATRIX* pd3dxmtxViewMatrix);
	void SetProjectionMatrix(D3DXMATRIX* pd3dxmtxProjMatrix);

	D3DXMATRIX GetViewMatrix(void);
	D3DXMATRIX GetProjectionMatrix(void);
	D3D11_VIEWPORT GetViewport(void);
	//절두체의 6개 평면을 계산한다.
	void CalculateFrustumPlanes();
	//바운딩 박스가 절두체에 완전히 포함되거나 일부라도 포함되는 가를 검사한다.
	bool CCamera::IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	bool IsInFrustum(AABB *pAABB);

	//상수 버퍼를 생성하고 내용을 갱신하는 멤버 함수를 선언한다.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);				// 카메라와 관련하여 셰이더에 제공할 자원 변수를 생성한다.
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);	// 카메라 상수 버퍼를 갱신한다.
};

