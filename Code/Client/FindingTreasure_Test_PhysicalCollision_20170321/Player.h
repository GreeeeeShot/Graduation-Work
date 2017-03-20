#pragma once
#include "Object.h"										// CPlayer는 Object의 일종이다.
#include "CameraOperator.h"								// 20170311 카메라 조작기이다.

#define PLAYER_ACCELERATION_MAGNITUDE			5.6		// m/s^2		// 1초 후 최대 속도에 도달한다.
#define PLAYER_MAX_VELOCITY_MAGNITUDE			5.6		// m/s
#define PLAYER_DAMPING_VALUE					0.8		// 0 <= ㅁ <= 1
#define PLAYER_ROTATION_DEGREE_PER_SEC			420.0f	// Degree
#define PLAYER_SOFT_ROTATION_INTERVAL_NUM		230		// num

class CPlayer : public CGameObject
{
public:
	//CCamera *m_pCamera;
	// 플레이어는 카메라 조작기를 통해 카메라를 초기화하고 조작할 수 있게 된다.
	CCameraOperator m_CameraOperator;
	float m_fAccelerationMagnitude;
	float m_fVelocityMagnitude;
	float m_fDamping;

public:
	CPlayer();
	virtual ~CPlayer();

	void InitCameraOperator(void);

	void GoForward(float fTimeElapsed);
	void GoBack(float fTimeElapsed);
	void GoRight(float fTimeElapsed);
	void GoLeft(float fTimeElapsed);
	void MovingByCamera(D3DXVECTOR3 d3dxvDirSum, float fTimeElapsed);

	void Moving(D3DXVECTOR3 d3dxvShift) { SetPosition(d3dxvShift + GetPosition()); }
	void RotateWorldY(float fAngle, float fTimeElapsed);

	//플레이어의 상수 버퍼를 생성하고 갱신하는 멤버 함수를 선언한다.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
};



