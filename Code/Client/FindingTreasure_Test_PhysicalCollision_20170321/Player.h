#pragma once
#include "Object.h"										// CPlayer�� Object�� �����̴�.
#include "CameraOperator.h"								// 20170311 ī�޶� ���۱��̴�.

#define PLAYER_ACCELERATION_MAGNITUDE			5.6		// m/s^2		// 1�� �� �ִ� �ӵ��� �����Ѵ�.
#define PLAYER_MAX_VELOCITY_MAGNITUDE			5.6		// m/s
#define PLAYER_DAMPING_VALUE					0.8		// 0 <= �� <= 1
#define PLAYER_ROTATION_DEGREE_PER_SEC			420.0f	// Degree
#define PLAYER_SOFT_ROTATION_INTERVAL_NUM		230		// num

class CPlayer : public CGameObject
{
public:
	//CCamera *m_pCamera;
	// �÷��̾�� ī�޶� ���۱⸦ ���� ī�޶� �ʱ�ȭ�ϰ� ������ �� �ְ� �ȴ�.
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

	//�÷��̾��� ��� ���۸� �����ϰ� �����ϴ� ��� �Լ��� �����Ѵ�.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
};



