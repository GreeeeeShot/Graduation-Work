#pragma once

#include "Timer.h"				// ���� �����ӿ�ũ�� '��� �ð�'�� �����ϱ� ����.
#include "Scene.h"				// ���� �����ӿ�ũ �޼���� �ɸ��͸� �����ϱ� ����.
#include "Player.h"				// �÷��̾�� ���� �����ӿ�ũ�� ���������� �����ϴ� Ư���� ������Ʈ�̴�.
#include "GameUI.h"				// �ΰ��ӿ��� ���� ���¸� �˷��ִ� UI�̴�.
#include "LightShaftRenderer.h"
#include "TimeManager.h"

class CGameFramework
{
public:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	int m_PastXMove;
	int m_PastZMove;

	int m_PastCameraYRotate;
	int m_PastCameraXRotate;
	bool m_Far;

	bool m_Isthrow;
	//float current_time;

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

	//������ ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�.
	CTimer m_GameTimer;
	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
	_TCHAR m_pszBuffer[50];
	//������ ������ ���(Scene)�� �����ϴ� ��ü�� ���� �����͸� ��Ÿ����.

	SWaitingRoomInformDesc* m_pWaitingRoomInformDesc;
	CLightSource *m_pLightSource;
	CShadowRenderer *m_pShadowRenderer;
	bool m_bIsShadow;
	CLightShaftRenderer *m_pLightShaftRenderer;
	bool m_bIsLightShaft;
	CScene *m_pScene;
	CPlayersMgrInform *m_pPlayersMgrInform;
	CGameUI *m_pUI;
	CTimeManager m_TimeManager;

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	//����̽�, ���� ü��, ����̽� ���ؽ�Ʈ, ����̽��� ���õ� �並 �����ϴ� �Լ��̴�. 
	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	//�������� �޽�, ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 
	CScene* GetSceneForType(SCENE_TYPE eSceneType);
	void BuildObjects();
	void SetPlayersToScene(CScene *pScene, CPlayer **ppPlayers);
	void ReleaseObjects();

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
	void ProcessInput();
	void AnimateObjects();
	/*void Update(void);
	void Render(void);*/
	void FrameAdvance();

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};


