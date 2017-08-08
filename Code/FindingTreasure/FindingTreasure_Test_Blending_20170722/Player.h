#pragma once
#include "Object.h"										// CPlayer�� Object�� �����̴�.
#include "CameraOperator.h"								// 20170311 ī�޶� ���۱��̴�.
#include "Scene.h"
#include "VoxelTerrain.h"
//#include "FBXAnim.h"

#define PLAYER_ACCELERATION_MAGNITUDE			15.0f		// m/s^2		
#define PLAYER_MAX_VELOCITY_MAGNITUDE			5.0f			// m/s
#define PLYAER_MAX_JUMP_VELOCITY_MAGNITUDE		20.0f		// m/s
#define PLAYER_FRICTION							2.0f		// 0 <= �� <= 1
#define PLAYER_ROTATION_DEGREE_PER_SEC			420.0f	// Degree
#define PLAYER_SOFT_ROTATION_SPLIT_FRACTION		0.17f		// split fraction/fps
#define PLAYER_MASS								40.0f		// kg

enum BELONG_TYPE { BELONG_TYPE_RED, BELONG_TYPE_BLUE, BELONG_TYPE_INDIVIDUAL };
//enum ACTION {IDLE,WALK,ATK,MAKEAHOLE,HIT};

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

//template <typename AnimType>
class CPlayer : public CGameObject
{
public:
	//CCamera *m_pCamera;
	// �÷��̾�� ī�޶� ���۱⸦ ���� ī�޶� �ʱ�ȭ�ϰ� ������ �� �ְ� �ȴ�.
	bool m_bIsActive;
	BELONG_TYPE m_BelongType;
	CCameraOperator m_CameraOperator;
	D3DXVECTOR3 m_d3dxvMoveDir;
	D3DXVECTOR3 m_d3dxvVelocity;
	float m_fFriction;

	CVoxelPocket m_VoxelPocket[5];
	int m_iVoxelPocketSlotIdx;

	bool m_bIsPushed;
	bool m_bIsDigOrInstall;

	int m_MoveX;
	int m_MoveZ;
	int m_CameraY;
	D3DXVECTOR3 m_SyncPosition;
	bool m_Delvoxel;
	bool m_Insvoxel;
	//ACTION m_action;

public:
	CPlayer();
	virtual ~CPlayer();

	void InitPlayer(D3DXVECTOR3 d3dxvPos);
	void InitCameraOperator(void);

	/*void GoForward(float fTimeElapsed);
	void GoBack(float fTimeElapsed);
	void GoRight(float fTimeElapsed);
	void GoLeft(float fTimeElapsed);
	void MovingByCamera(D3DXVECTOR3 d3dxvDirSum, float fTimeElapsed);*/
	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	void SetBelongType(BELONG_TYPE eBelongType) { m_BelongType = eBelongType; }
	void DigInVoxelTerrain(CVoxelTerrain *pVoxelTerrain, bool bIsDigging, float fTimeElapsed, int id);
	void InstallVoxel(CVoxelTerrain *pVoxelTerrain, bool bIsInstalling, float fTimeElapsed, int id);
	void ChangeVoxelPocketSlotIdx(int iIdx);
	void PushPlayer(CPlayer *pPlayer);
	void PushPlayers(CPlayer **ppPlayers, int nConnectedPlayersNum);

	void Moving(D3DXVECTOR3 d3dxvShift) { SetPosition(d3dxvShift + GetPosition()); }
	void RotateMoveDir(float fTimeElapsed);
	/*void Jump(CVoxelTerrain *pVoxelTerrain);*/
	//void MovingUnderPhysicalEnvironment(CScene *pScene, CVoxelTerrain *pVoxelTerrain, float fTimeElapsed);
	void ProofreadLocalAxis(void);
	void SetFBXAnimForType(int);
	void Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed);
	//�÷��̾��� ��� ���۸� �����ϰ� �����ϴ� ��� �Լ��� �����Ѵ�.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
	void Render(ID3D11DeviceContext *pd3dDeviceContext);
	void SetMove(float x, float y, float z);
	void FramePerMove();
};

//class CPiratePlayer : public CPlayer<PIRATE_ANIM_TYPE>
//{
//public:
//	CPiratePlayer();
//	~CPiratePlayer();
//
//	void SetFBXAnimForType(PIRATE_ANIM_TYPE);
//};