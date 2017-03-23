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
	// 카메라 조작기 위치 좌표
	D3DXVECTOR3 m_d3dxvPosition;

	// 카메라 실제 위치 좌표
	D3DXVECTOR3 m_d3dxvCameraPos;

	// 카메라 위치 벡터 ; 카메라 조작기 좌표계 원점에서 카메라로의 방향 벡터
	D3DXVECTOR3 m_d3dxvCameraDir;
	// 줌 방향 벡터
	D3DXVECTOR3 m_d3dxvZoomDir;
	// 거리 스칼라 값 ; 카메라 조작기 좌표계 원점에서부터 카메라까지의 거리
	float m_fDistance;

	// 카메라 조작기의 로컬 좌표계
	D3DXVECTOR3 m_d3dxvRight;
	D3DXVECTOR3 m_d3dxvUp;
	D3DXVECTOR3 m_d3dxvLook;

	float m_fAngleX;

public:
	CCameraOperator();
	~CCameraOperator();

	void InitCameraOperator(CGameObject* pObject);								// 멤버 변수들을 초기화한다.

	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	D3DXVECTOR3 GetLook(void) { return m_d3dxvLook; }
	D3DXVECTOR3 GetUp(void) { return m_d3dxvUp; }
	D3DXVECTOR3 GetRight(void) { return m_d3dxvRight; }

	void MoveCameraOperator(const D3DXVECTOR3& d3dxvShift);						// 카메라 조작기를 현 위치에서 d3dxvShift만큼 이동시킨다.
	void RotateLocalY(float fRotationDegreePerSec, float fTimeElapsed);							// m_d3dxvCameraDir을 로컬 Y축에 대하여 회전한다. ; 좌표계 전체도 움직인다.
	void RotateLocalX(float fRotationDegreePerSec, float fTimeElapsed);							// m_d3dxvCameraDir을 로컬 Z축에 대하여 회전한다.
	void ZoomInAtOnce(float distance);											// 바라보고 있는 곳으로 가까워진다.
	void ZoomOutAtOnce(float distance);											// 바라보고 있는 곳으로부터 멀어진다.
	void OriginalZoomState(void);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);						// 카메라와 관련하여 셰이더에 제공할 자원 변수를 생성한다.

	void GenerateViewMatrix(bool bSoftMoving = false);							// 부드럽게 움직이는 표현을 하고 싶으면 true로 설정한다.
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);			// 카메라 상수 버퍼를 갱신한다.
};

