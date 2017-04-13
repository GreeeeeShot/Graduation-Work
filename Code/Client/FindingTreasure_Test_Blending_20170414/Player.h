#pragma once
#include "Object.h"										// CPlayer�� Object�� �����̴�.
#include "CameraOperator.h"								// 20170311 ī�޶� ���۱��̴�.
#include "Scene.h"
#include "VoxelTerrain.h"

#define PLAYER_ACCELERATION_MAGNITUDE			15.0f		// m/s^2		
#define PLAYER_MAX_VELOCITY_MAGNITUDE			5.0f			// m/s
#define PLAYER_FRICTION							2.0f		// 0 <= �� <= 1
#define PLAYER_ROTATION_DEGREE_PER_SEC			420.0f	// Degree
#define PLAYER_SOFT_ROTATION_SPLIT_FRACTION		0.17f		// split fraction/fps
#define PLAYER_MASS								40.0f		// kg

class CVoxelPocket
{
public:
	CVoxel* m_pVoxel;
	bool m_bIsActive;

	CVoxelPocket() 
	{
		m_pVoxel = NULL;
		m_bIsActive = false;
	}
	~CVoxelPocket() {}
};

class CPlayer : public CGameObject
{
public:
	//CCamera *m_pCamera;
	// �÷��̾�� ī�޶� ���۱⸦ ���� ī�޶� �ʱ�ȭ�ϰ� ������ �� �ְ� �ȴ�.
	CCameraOperator m_CameraOperator;
	D3DXVECTOR3 m_d3dxvMoveDir;
	D3DXVECTOR3 m_d3dxvVelocity;
	float m_fFriction;

	CVoxelPocket m_VoxelPocket[5];
	int m_iVoxelPocketSlotIdx;
	
public:
	CPlayer();
	virtual ~CPlayer();

	void InitCameraOperator(void);

	/*void GoForward(float fTimeElapsed);
	void GoBack(float fTimeElapsed);
	void GoRight(float fTimeElapsed);
	void GoLeft(float fTimeElapsed);
	void MovingByCamera(D3DXVECTOR3 d3dxvDirSum, float fTimeElapsed);*/
	void DigInVoxelTerrain(CVoxelTerrain *pVoxelTerrain, bool bIsDigging, float fTimeElapsed);
	void InstallVoxel(CVoxelTerrain *pVoxelTerrain, bool bIsInstalling, float fTimeElapsed);
	void PushPlayers(CPlayer **m_ppPlayers, int nConnectedPlayersNum);

	void Moving(D3DXVECTOR3 d3dxvShift) { SetPosition(d3dxvShift + GetPosition()); }
	void RotateMoveDir(float fTimeElapsed);
	/*void Jump(CVoxelTerrain *pVoxelTerrain);*/
	void MovingUnderPhysicalEnvironment(CScene *pScene, CVoxelTerrain *pVoxelTerrain, float fTimeElapsed);
	void ProofreadLocalAxis(void);
	
	//�÷��̾��� ��� ���۸� �����ϰ� �����ϴ� ��� �Լ��� �����Ѵ�.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
};



