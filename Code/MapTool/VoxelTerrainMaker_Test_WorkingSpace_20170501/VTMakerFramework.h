#pragma once

#include "Timer.h"
#include "Scene.h"				// 게임 프레임워크 메서드로 케릭터를 조작하기 위함.
#include "Player.h"				// 플레이어는 게임 프레임워크가 직접적으로 관리하는 특별한 오브젝트이다.
#include "WorkingSpace.h"
#include "CameraOperator.h"

#define WORKINGSPACE_MAX_NUM		8

class CVTMakerFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

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

	//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다.
	_TCHAR m_pszBuffer[50];

	//다음은 게임 프레임워크에서 사용할 타이머이다.
	CTimer m_GameTimer;
	//다음은 게임의 장면(Scene)을 관리하는 객체에 대한 포인터를 나타낸다.
	//CScene *m_pScene;
	//CPlayer **m_ppPlayers;

	CCameraOperator m_CameraOperator;
	CVoxelTerrain *m_pVoxelTerrain;

	ID3D11Buffer *m_pd3dcbColor;

public:

	// CWorkingSpace *m_pWorkingSpace[WORKINGSPACE_MAX_NUM];
	std::vector<CWorkingSpace*> m_vecWorkingSpace;
	int m_iWorkingSpaceNum;
	int m_iActivedWorkingSpaceIdx;

	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;

	CVTMakerFramework();
	~CVTMakerFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	//디바이스, 스왑 체인, 디바이스 컨텍스트, 디바이스와 관련된 뷰를 생성하는 함수이다. 
	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	//렌더링할 메쉬, 객체를 생성하고 소멸하는 함수이다. 
	void BuildObjects();
	void CreateWorkingSpaceFromFile(HWND hList, const char* pFileName);
	void SaveActivedWorkingSpaceToFile(const char* pFileName);
	void SaveAllWorkingSpaceToFile(const char* pFileName);
	void SaveIndividualWorkingSpaceToFile(const char* pFileName);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables();

	//void SetPlayersToScene(CScene *pScene, CPlayer **ppPlayers);
	void PushWorkingSpace(CWorkingSpace *pWorkingSpace);
	void EraseActivedWorkingSpace(void);
	void SetVoxelTerrain(CVoxelTerrain *pVoxelTerrain);
	void CreateWorkingSpace(D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal);
	void TransformWorkingSpace(D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal);
	void RegenerateWorkingSpace(D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal);
	void DestroyWorkingSpace(void);
	void OptimizeWorkingSpace(void);
	
	CWorkingSpace* GetActivedWorkingSpace(void) { return m_vecWorkingSpace[m_iActivedWorkingSpaceIdx]; }

	void ReleaseObjects();

	void ChangeWorkingSpace(int iIdx);

	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다. 
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

