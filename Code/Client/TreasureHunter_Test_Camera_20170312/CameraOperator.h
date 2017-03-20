#pragma once
#include "Object.h";
#include"Camera.h"

#define ZOOM_IN_LIMIT_DISTANCE		10.0f	// Meters
#define ZOOM_OUT_LIMIT_DISTANCE		50.0f	// Meters
#define ZOOM_AT_ONCE_DISTANCE		50.0f	// Meters

#define DEGREE_X_LOWER_LIMIT		20.0f	// Degree
#define DEGREE_X_UPPER_LIMIT		70.0f	// Degree

#define CAMERA_DISTANCE				100.0f	// Meters
#define ROTATION_DEGREE_PER_SEC		420.0f	// Degree
#define SOFT_MOVING_INTERVAL_NUM	230		// Num

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

	void MoveCameraOperator(const D3DXVECTOR3& d3dxvShift);						// ī�޶� ���۱⸦ �� ��ġ���� d3dxvShift��ŭ �̵���Ų��.
	void RotateLocalY(float angle, float fTimeElapsed);							// m_d3dxvCameraDir�� ���� Y�࿡ ���Ͽ� ȸ���Ѵ�. ; ��ǥ�� ��ü�� �����δ�.
	void RotateLocalX(float angle, float fTimeElapsed);							// m_d3dxvCameraDir�� ���� Z�࿡ ���Ͽ� ȸ���Ѵ�.
	void ZoomInAtOnce(float distance);											// �ٶ󺸰� �ִ� ������ ���������.
	void ZoomOutAtOnce(float distance);											// �ٶ󺸰� �ִ� �����κ��� �־�����.
	void OriginalZoomState(void);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);						// ī�޶�� �����Ͽ� ���̴��� ������ �ڿ� ������ �����Ѵ�.

	void GenerateViewMatrix(bool bSoftMoving = false);							// �ε巴�� �����̴� ǥ���� �ϰ� ������ true�� �����Ѵ�.
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);			// ī�޶� ��� ���۸� �����Ѵ�.
};
