#pragma once
#include "Object.h";
#include"Camera.h"

#define ZOOM_IN_LIMIT_DISTANCE				1.5f	// Meters
#define ZOOM_OUT_LIMIT_DISTANCE				17.0f	// Meters
#define ZOOM_AT_ONCE_DISTANCE				5.0f	// Meters

#define DEGREE_X_LOWER_LIMIT				20.0f	// Degree
// #define DEGREE_INIT_VALUE					40.0f	// Degree
#define DEGREE_X_UPPER_LIMIT				45.0f	// Degree

#define CAMERA_DISTANCE						7.0f	// Meters
#define CAMERA_ROTATION_DEGREE_PER_SEC		170.0f	// Degree
#define CAMERA_SOFT_MOVING_INTERVAL_NUM		80		// Num

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
	CCameraOperator();
	~CCameraOperator();

	void InitCameraOperator(CGameObject* pObject);								// ��� �������� �ʱ�ȭ�Ѵ�.

	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	D3DXVECTOR3 GetLook(void) { return m_d3dxvLook; }
	D3DXVECTOR3 GetUp(void) { return m_d3dxvUp; }
	D3DXVECTOR3 GetRight(void) { return m_d3dxvRight; }

	void MoveCameraOperator(const D3DXVECTOR3& d3dxvShift);						// ī�޶� ���۱⸦ �� ��ġ���� d3dxvShift��ŭ �̵���Ų��.
	void RotateLocalY(float fRotationDegreePerSec, float fTimeElapsed);							// m_d3dxvCameraDir�� ���� Y�࿡ ���Ͽ� ȸ���Ѵ�. ; ��ǥ�� ��ü�� �����δ�.
	void RotateLocalX(float fRotationDegreePerSec, float fTimeElapsed);							// m_d3dxvCameraDir�� ���� Z�࿡ ���Ͽ� ȸ���Ѵ�.
	void ZoomInAtOnce(float distance);											// �ٶ󺸰� �ִ� ������ ���������.
	void ZoomOutAtOnce(float distance);											// �ٶ󺸰� �ִ� �����κ��� �־�����.
	void OriginalZoomState(void);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);						// ī�޶�� �����Ͽ� ���̴��� ������ �ڿ� ������ �����Ѵ�.

	void GenerateViewMatrix(bool bSoftMoving = false);							// �ε巴�� �����̴� ǥ���� �ϰ� ������ true�� �����Ѵ�.
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);			// ī�޶� ��� ���۸� �����Ѵ�.
};

