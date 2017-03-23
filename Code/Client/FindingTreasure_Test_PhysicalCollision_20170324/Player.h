#pragma once
#include "Object.h"										// CPlayer�� Object�� �����̴�.
#include "CameraOperator.h"								// 20170311 ī�޶� ���۱��̴�.
#include "Scene.h"
#include "VoxelTerrain.h"

#define PLAYER_ACCELERATION_MAGNITUDE			987654321.0f		// m/s^2		
#define PLAYER_MAX_VELOCITY_MAGNITUDE			5.5f			// m/s
#define PLAYER_DAMPING_VALUE					1.0f		// 0 <= �� <= 1
#define PLAYER_ROTATION_DEGREE_PER_SEC			420.0f	// Degree
#define PLAYER_SOFT_ROTATION_INTERVAL_NUM		50.0f		// num
#define PLAYER_MASS								60.0f		// kg

class CPlayer : public CGameObject
{
public:
	//CCamera *m_pCamera;
	// �÷��̾�� ī�޶� ���۱⸦ ���� ī�޶� �ʱ�ȭ�ϰ� ������ �� �ְ� �ȴ�.
	CCameraOperator m_CameraOperator;
	D3DXVECTOR3 m_d3dxvMoveDir;
	D3DXVECTOR3 m_d3dxvVelocity;
	float m_fDamping;

public:
	CPlayer();
	virtual ~CPlayer();

	void InitCameraOperator(void);

	/*void GoForward(float fTimeElapsed);
	void GoBack(float fTimeElapsed);
	void GoRight(float fTimeElapsed);
	void GoLeft(float fTimeElapsed);
	void MovingByCamera(D3DXVECTOR3 d3dxvDirSum, float fTimeElapsed);*/

	void Moving(D3DXVECTOR3 d3dxvShift) { SetPosition(d3dxvShift + GetPosition()); }
	void RotateMoveDir(void);
	void MovingUnderPhysicalEnvironment(CScene *pScene, CVoxelTerrain *pVoxelTerrain, float fTimeElapsed);

	//�÷��̾��� ��� ���۸� �����ϰ� �����ϴ� ��� �Լ��� �����Ѵ�.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
};



