#pragma once
#include "Object.h"			// 씬의 구성요소로 오브젝트들이 존재한다.
#include "Player.h"
#include "Camera.h"
#include "VoxelTerrain.h"
#include "LightResource.h"
#include "RespawnManager.h"		// 인게임에서 플레이어 오브젝트의 리스폰을 담당한다.

#define GRAVITATIONAL_ACCELERATION			-9.8f	// m/s^2
#define AIR_RESISTANCE_COEFFICIENT			12.5f//4.6f
#define SHIP_FRICTION						9.0f

enum SCENE_TYPE { SCENE_TYPE_JUNGLE, SCENE_TYPE_SNOW };

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
	virtual ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void DestroyScene(void);
	void InitScene(ID3D11Device *pd3dDevice);
	void SetPlayersMgrInform(CPlayersMgrInform *pPlayersMgrInform) { m_pPlayersMgrInform = pPlayersMgrInform; }
	void ReleaseObjects();

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables();

	bool ProcessInput();
	void MoveObjectUnderPhysicalEnvironment(CPlayer *pPlayer, float fTimeElapsed);
	void AnimateObjects(ID3D11Device *pd3dDevice, ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed);
	void Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera);		// 카메라 절두체 기법을 위하여 매개변수로 pCamera가 존재한다.
};

class CJungleScene : public CScene
{
public:
	CJungleScene();
	~CJungleScene();
};

class CSnowScene : public CScene
{
public:
	CSnowScene();
	~CSnowScene();
};


