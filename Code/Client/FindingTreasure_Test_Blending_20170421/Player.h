#pragma once
#include "Object.h"										// CPlayer는 Object의 일종이다.
#include "CameraOperator.h"								// 20170311 카메라 조작기이다.
#include "Scene.h"
#include "VoxelTerrain.h"

#define PLAYER_ACCELERATION_MAGNITUDE			15.0f		// m/s^2		
#define PLAYER_MAX_VELOCITY_MAGNITUDE			5.0f			// m/s
#define PLAYER_FRICTION							2.0f		// 0 <= ㅁ <= 1
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
	// 플레이어는 카메라 조작기를 통해 카메라를 초기화하고 조작할 수 있게 된다.
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
	
	//플레이어의 상수 버퍼를 생성하고 갱신하는 멤버 함수를 선언한다.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
};



