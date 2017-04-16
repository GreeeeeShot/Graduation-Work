#pragma once

#include "Timer.h"
#include "Scene.h"				// ���� �����ӿ�ũ �޼���� �ɸ��͸� �����ϱ� ����.
#include "Player.h"				// �÷��̾�� ���� �����ӿ�ũ�� ���������� �����ϴ� Ư���� ������Ʈ�̴�.
#include "WorkingSpace.h"
#include "CameraOperator.h"

class CVTMakerFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	//����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11Device *m_pd3dDevice;
	//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	IDXGISwapChain *m_pDXGISwapChain;
	//����̽� ���ؽ�Ʈ�� ���� �������̴�. �ַ� ���������� ������ �ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11DeviceContext *m_pd3dDeviceContext;
	//���� Ÿ�� �� �������̽��� ���� �������̴�. 
	ID3D11RenderTargetView *m_pd3dRenderTargetView;

	ID3D11Texture2D *m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;

	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
	_TCHAR m_pszBuffer[50];

	//������ ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�.
	CTimer m_GameTimer;
	//������ ������ ���(Scene)�� �����ϴ� ��ü�� ���� �����͸� ��Ÿ����.
	//CScene *m_pScene;
	//CPlayer **m_ppPlayers;

	CCameraOperator m_CameraOperator;
	CVoxelTerrain *m_pVoxelTerrain;

	ID3D11Buffer *m_pd3dcbColor;

public:

	CWorkingSpace *m_pWorkingSpace;

	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;

	CVTMakerFramework();
	~CVTMakerFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	//����̽�, ���� ü��, ����̽� ���ؽ�Ʈ, ����̽��� ���õ� �並 �����ϴ� �Լ��̴�. 
	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	//�������� �޽�, ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 
	void BuildObjects();
	void CreateWorkingSpaceFromFile(const char* pFileName);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables();

	void SetPlayersToScene(CScene *pScene, CPlayer **ppPlayers);
	void SetWorkingSpace(CWorkingSpace *pWorkingSpace);
	void SetVoxelTerrain(CVoxelTerrain *pVoxelTerrain);
	void CreateWorkingSpace(D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal);
	void ReleaseObjects();

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

