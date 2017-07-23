#pragma once

#include "Timer.h"				// 게임 프레임워크에 '경과 시간'을 전달하기 위함.
#include "Scene.h"				// 게임 프레임워크 메서드로 케릭터를 조작하기 위함.
#include "Player.h"				// 플레이어는 게임 프레임워크가 직접적으로 관리하는 특별한 오브젝트이다.
#include "GameUI.h"				// 인게임에서 게임 상태를 알려주는 UI이다.

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

	//float current_time;

	//디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ID3D11Device *m_pd3dDevice;
	//스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	IDXGISwapChain *m_pDXGISwapChain;
	//디바이스 컨텍스트에 대한 포인터이다. 주로 파이프라인 설정을 하기 위하여 필요하다.
	ID3D11DeviceContext *m_pd3dDeviceContext;
	//렌더 타겟 뷰 인터페이스에 대한 포인터이다. 
	ID3D11RenderTargetView *m_pd3dRenderTargetView;

	ID3D11Texture2D *m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;

	//다음은 게임 프레임워크에서 사용할 타이머이다.
	CTimer m_GameTimer;
	//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다.
	_TCHAR m_pszBuffer[50];
	//다음은 게임의 장면(Scene)을 관리하는 객체에 대한 포인터를 나타낸다.
	CShadowRenderer *m_pShadowRenderer;
	CScene *m_pScene;
	CPlayersMgrInform *m_pPlayersMgrInform;
	CGameUI *m_pUI;

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	//디바이스, 스왑 체인, 디바이스 컨텍스트, 디바이스와 관련된 뷰를 생성하는 함수이다. 
	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	//렌더링할 메쉬, 객체를 생성하고 소멸하는 함수이다. 
	void BuildObjects();
	void SetPlayersToScene(CScene *pScene, CPlayer **ppPlayers);
	void ReleaseObjects();

	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다. 
	void ProcessInput();
	void AnimateObjects();
	/*void Update(void);
	void Render(void);*/
	void FrameAdvance();

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};


