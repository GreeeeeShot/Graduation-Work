#pragma once
#include "Object.h"			// ���� ������ҷ� ������Ʈ���� �����Ѵ�.
#include "Camera.h"
#include "VoxelTerrain.h"
#include "LightResource.h"

class CScene
{
private:
	CGameObject **m_ppObjects;
	int m_nObjects;

	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;

	CVoxelTerrain *m_pVoxelTerrain;

public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera);		// ī�޶� ����ü ����� ���Ͽ� �Ű������� pCamera�� �����Ѵ�.
};


