#pragma once
#include "Object.h";
#include"Camera.h"

#define ZOOM_IN_LIMIT_DISTANCE				1.5f	// Meters
#define ZOOM_OUT_LIMIT_DISTANCE				17.0f	// Meters
#define ZOOM_AT_ONCE_DISTANCE				5.0f	// Meters

#define DEGREE_X_LOWER_LIMIT				10.0f	// Degree
// #define DEGREE_INIT_VALUE					40.0f	// Degree
#define DEGREE_X_UPPER_LIMIT				70.0f	// Degree

#define CAMERA_DISTANCE						7.0f	// Meters
#define CAMERA_ROTATION_DEGREE_PER_SEC		170.0f	// Degree
#define CAMERA_SOFT_MOVING_SPLIT_FRACTION	0.27f	// split fraction/fps

class CCameraOperator
{
public:
	CCamera m_Camera;

protected:
	CGameObject* m_pObject;
	// ī�޶� ���۱� ��ġ ��ǥ
	D3DXVECTOR3 m_d3dxvPosition;

	// ī�޶� ���� ��ġ ��ǥ
	D3DXVECTOR3 m_d3dxvCameraPos;

	// ī�޶� ��ġ ���� ; ī�޶� ���۱� ��ǥ�� �������� ī�޶���� ���� ����
	D3DXVECTOR3 m_d3dxvCameraDir;
	// �� ���� ����
	D3DXVECTOR3 m_d3dxvZoomDir;
	// �Ÿ� ��Į�� �� ; ī�޶� ���۱� ��ǥ�� ������������ ī�޶������ �Ÿ�
	float m_fDistance;

	// ī�޶� ���۱��� ���� ��ǥ��
	D3DXVECTOR3 m_d3dxvRight;
	D3DXVECTOR3 m_d3dxvUp;
	D3DXVECTOR3 m_d3dxvLook;
	float m_fAngleX;

public:
	float m_pastYpos;

public:
	CCameraOperator();
	~CCameraOperator();

	void InitCameraOperator(CGameObject* pObject);								// ��� �������� �ʱ�ȭ�Ѵ�.

	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	void SetPosition(D3DXVECTOR3 pos) { m_d3dxvCameraPos = pos; }
	void SetOperatorPosition(D3DXVECTOR3 pos) { m_d3dxvPosition = pos; }
	D3DXVECTOR3 GetPosition() {return m_d3dxvCameraPos;}
	D3DXVECTOR3 GetOperatorPosition() { return m_d3dxvPosition; }

	D3DXVECTOR3 GetLook(void) { return m_d3dxvLook; }
	D3DXVECTOR3 GetUp(void) { return m_d3dxvUp; }
	D3DXVECTOR3 GetRight(void) { return m_d3dxvRight; }

	void MoveCameraOperator(const D3DXVECTOR3& d3dxvShift);						// ī�޶� ���۱⸦ �� ��ġ���� d3dxvShift��ŭ �̵���Ų��.
	void RotateLocalY(float fRotationDegreePerSec, float fTimeElapsed);							// m_d3dxvCameraDir�� ���� Y�࿡ ���Ͽ� ȸ���Ѵ�. ; ��ǥ�� ��ü�� �����δ�.
	void RotateLocalX(float fRotationDegreePerSec, float fTimeElapsed);							// m_d3dxvCameraDir�� ���� Z�࿡ ���Ͽ� ȸ���Ѵ�.
	void ZoomInAtOnce(float distance);											// �ٶ󺸰� �ִ� ������ ���������.
	void ZoomOutAtOnce(float distance);											// �ٶ󺸰� �ִ� �����κ��� �־�����.
	void OriginalZoomState(void);
	void ProofreadLocalAxis(void);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);						// ī�޶�� �����Ͽ� ���̴��� ������ �ڿ� ������ �����Ѵ�.

	void GenerateViewMatrix(float fTimeElapsed, bool bSoftMoving = false);							// �ε巴�� �����̴� ǥ���� �ϰ� ������ true�� �����Ѵ�.
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);			// ī�޶� ��� ���۸� �����Ѵ�.
	
};

