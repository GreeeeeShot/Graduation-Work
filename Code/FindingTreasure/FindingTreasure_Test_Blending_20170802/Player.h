#pragma once
#include "Object.h"										// CPlayer�� Object�� �����̴�.
#include "CameraOperator.h"								// 20170311 ī�޶� ���۱��̴�.
#include "Scene.h"
#include "VoxelTerrain.h"
//#include "FBXAnim.h"

#define PLAYER_ACCELERATION_MAGNITUDE				15.0f		// m/s^2		
#define PLAYER_MAX_VELOCITY_MAGNITUDE				5.0f			// m/s
#define PLYAER_MAX_JUMP_VELOCITY_MAGNITUDE			20.0f		// m/s
#define PLAYER_FRICTION								2.0f		// 0 <= �� <= 1
#define PLAYER_ROTATION_DEGREE_PER_SEC				420.0f	// Degree
#define PLAYER_SOFT_ROTATION_SPLIT_FRACTION			0.17f		// split fraction/fps
#define PLAYER_MASS									40.0f		// kg

#define TREASURE_CHEST_MASS							30.0f		//kg
#define TREASURE_CHEST_REDUCTION_MAX_RUN_VEL_M		1.5f	// �޸��� �ִ� �ӵ� 3 �� ����
#define TREASURE_CHEST_REDUCTION_JUMP_D_VEL_Y_M		2.0f	// ���� ���� �ӵ� 2 �� ����

#define PLAYER_MAX_STAMINA							100.0f

enum PLAYER_TYPE { PLAYER_TYPE_PIRATE, PLAYER_TYPE_SNOW_GIRL, PLAYER_TYPE_COW_GIRL };
enum BELONG_TYPE { BELONG_TYPE_RED, BELONG_TYPE_BLUE, BELONG_TYPE_INDIVIDUAL };
enum MAP_TYPE { MAP_TYPE_JUNGLE, MAP_TYPE_ICE };

enum SCENE_TYPE;

#define MAX_CONNECTED_PLAYERS_NUM 8

struct SPlayerInformDesc
{
	bool m_bIsSlotActive;
	PLAYER_TYPE m_PlayerType;
	BELONG_TYPE m_BelongType;
};

struct SWaitingRoomInformDesc
{
	SCENE_TYPE m_eSceneType;
	int m_iMyPlayerID;
	SPlayerInformDesc m_PlayerInformDesc[MAX_CONNECTED_PLAYERS_NUM];
};

class CPlayersMgrInform				// �ܺο��� ���õȴ�.
{
public:
	SWaitingRoomInformDesc m_WaitingRoomInformDesc;			// 
	CPlayer **m_ppPlayers;
	int m_iPlayersNum;
	int m_iMyPlayerID;

	CShader *m_pShader;
	CMaterial *m_pMaterial;

	CPlayersMgrInform();
	~CPlayersMgrInform();

	CPlayer* GetMyPlayer(void) { return m_ppPlayers[m_iMyPlayerID]; }
	void SetShader(CShader *pShader);
	void SetMaterial(CMaterial *pMaterial);
	CPlayer* CreatePlayerForType(ID3D11Device *pd3dDevice, SPlayerInformDesc *pPlayerInformDesc);		// ������, �޽�, �ؽ���(��1) � �������?
	
	void InitPlayersMgrInform(ID3D11Device *pd3dDevice, SWaitingRoomInformDesc WaitingRoomInformDesc); // CreatePlayerForType�� ȣ���.
	void ReleaseGamePlayers(void);
	void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);
};

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
	PLAYER_TYPE m_PlayerType;
	BELONG_TYPE m_BelongType;
	CCameraOperator m_CameraOperator;
	D3DXVECTOR3 m_d3dxvMoveDir;
	D3DXVECTOR3 m_d3dxvVelocity;
	float m_fFriction;

	float m_fJumpdVelYM;		//Y�࿡ ���� ���� ���� �ӵ� ũ�� �̰� �޼��常�� �ǵ帰��.
	float m_fMaxRunVelM;		// �޸��� �ӵ��� ���� ũ��
	float m_fMass;              // ����

	//float m_fMaxStamina;
	float m_fCurStamina;
	float m_fRecoveryStaminaPerSec;		
	float m_fDecrementStaminaPerSec;

	CVoxelPocket m_VoxelPocket[5];
	int m_iVoxelPocketSlotIdx;

	bool m_bIsPushed;
	CPlayer *m_pLiftingPlayer;
	CPlayer *m_pLiftedPlayer;
	bool m_bIsDigOrInstall;
	
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
	void SetPlayerType(PLAYER_TYPE ePlayerType) { m_PlayerType = ePlayerType; }
	void DigInVoxelTerrain(CVoxelTerrain *pVoxelTerrain, bool bIsDigging, float fTimeElapsed);
	void InstallVoxel(CVoxelTerrain *pVoxelTerrain, bool bIsInstalling, float fTimeElapsed);
	void ChangeVoxelPocketSlotIdx(int iIdx);
	void PushPlayer(CPlayer *pPlayer);
	void LiftPlayer(CPlayer *pPlayer, CVoxelTerrain *pVoxelTerrain);			// ���ø��� �۾� (�÷��̾� ����)
	void BeRelievedFromLiftingPlayer(void);										// �����. (�������� ����)
	void MissPlayer(void);														// ��ġ��. (�÷��̾� ����)
	void BeDraggedAwayByLiftingPlayer(float fTimeElapsed);						// �����ٴϴ�. (�������� ����)
	void MoveLiftedPlayer(float fTimeElapsed);												// �����̴�. (�÷��̾� ����)
	void PushPlayers(CPlayer **ppPlayers, int nConnectedPlayersNum);

	void IncreaseStamina(float fElapsedTime);
	void DecreaseStamina(float fElapsedTime);

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
	void Render(ID3D11DeviceContext *pd3dDeviceContext, CShader *pShader);			// ��ġ ���μ��� ����
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

class CTreasureChest : public CPlayer
{
public:
	CTreasureChest()
	{
		m_fJumpdVelYM = 0.0f;			//Y�࿡ ���� ���� ���� �ӵ� ũ�� // �������ڴ� ������ �������� ���Ѵ�.
		m_fMaxRunVelM = 5.0f;			// �и��� �ӵ��� ���� �ִ� ũ��
		m_fMass = 40.0f;               // ����

		SetMaterial(CMaterialResource::pStandardMaterial);
		SetTexture(CTextureResource::pWoodTexture);
		SetMesh(CMeshResource::pTreasureChestMesh);
		SetShader(CShaderResource::pTexturedLightingShader);
		SetPosition(0.0f, 10.f, 0.0f);
	}
	~CTreasureChest() {}
};

class CPirate : public CPlayer
{
public:
	CPirate(ID3D11Device *pd3dDevice)
	{
		m_fJumpdVelYM = 4.97f;   //Y�࿡ ���� ���� ���� �ӵ� ũ��
		m_fMaxRunVelM = 5.0f;   // �޸��� �ӵ��� ���� ũ��
		m_fMass = 40.0f;               // ����

		m_fRecoveryStaminaPerSec = 40.0f;
		m_fDecrementStaminaPerSec = 12.0f;

		SetMesh(new CTexturedLightingPirateMesh(pd3dDevice));
	}
	~CPirate() {}
};

class CSnowGirl : public CPlayer
{

};

class CCowGirl : public CPlayer
{

};