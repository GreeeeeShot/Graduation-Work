#pragma once
#include "Object.h"			// ���� ������ҷ� ������Ʈ���� �����Ѵ�.
#include "Camera.h"
#include "VoxelTerrain.h"
#include "LightResource.h"
#include "RespawnManager.h"		// �ΰ��ӿ��� �÷��̾� ������Ʈ�� �������� ����Ѵ�.

#define GRAVITATIONAL_ACCELERATION			-9.8f	// m/s^2
#define AIR_RESISTANCE_COEFFICIENT			12.5f//4.6f
#define SHIP_FRICTION						9.0f

class CPlayersMgrInform				// �ܺο��� ���õȴ�.
{
public:
	CPlayer **m_ppPlayers;					
	int m_iPlayersNum;
	int m_iMyPlayerID;

	CPlayersMgrInform();
	~CPlayersMgrInform();

	CPlayer* GetMyPlayer(void) { return m_ppPlayers[m_iMyPlayerID]; }
};

class CScene
{
private:
	/*CGameObject **m_ppObjects;*/
	CPlayersMgrInform *m_pPlayersMgrInform;
	
	CGameObject *m_pSkydom;

	char* m_cSceneTextFileName;
	CPlayer *m_pTreasureChest;
	CGameObject *m_pShips[2];

	int m_nObjects;

	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;
	FOG *m_pFog;
	ID3D11Buffer *m_pd3dcbFog;

	CWaveObject *m_pWaveEffect;
	
	
	float m_fGravitationalAcceleration;

public:
	CVoxelTerrain *m_pVoxelTerrain;
	CRespawnManager m_RespawnManager;
	float m_fAccumulatedTime;

	CScene(char* cSceneTextFileName);
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void InitScene(void);
	void SetPlayersMgrInform(CPlayersMgrInform *pPlayersMgrInform) { m_pPlayersMgrInform = pPlayersMgrInform; }
	void ReleaseObjects();

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables();

	bool ProcessInput();
	void MoveObjectUnderPhysicalEnvironment(CPlayer *pPlayer, float fTimeElapsed);
	void AnimateObjects(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed);
	void Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera);		// ī�޶� ����ü ����� ���Ͽ� �Ű������� pCamera�� �����Ѵ�.
};


