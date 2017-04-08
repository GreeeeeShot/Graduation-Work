#pragma once
#include "Object.h"			// 씬의 구성요소로 오브젝트들이 존재한다.
#include "Camera.h"
#include "VoxelTerrain.h"
#include "LightResource.h"

#define GRAVITATIONAL_ACCELERATION			-9.8f	// m/s^2
#define AIR_RESISTANCE_COEFFICIENT			4.6f

class CScene
{
private:
	/*CGameObject **m_ppObjects;*/
	CPlayer **m_ppPlayers;

	int m_nObjects;

	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;

	
	CWaveObject *m_pWaveEffect;
	
	float m_fGravitationalAcceleration;

public:
	CVoxelTerrain *m_pVoxelTerrain;
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void SetPlayers(CPlayer **ppPlayers);
	void ReleaseObjects();

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables();

	bool ProcessInput();
	void AnimateObjects(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed);
	void Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera);		// 카메라 절두체 기법을 위하여 매개변수로 pCamera가 존재한다.
};


