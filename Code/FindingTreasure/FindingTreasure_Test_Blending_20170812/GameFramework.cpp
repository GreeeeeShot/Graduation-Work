#include "stdafx.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "ShaderResource.h"
#include "BlendingResource.h"
#include "EffectManager.h"
//#include "Scene.h"
#include "FBXAnim.h"
#include "GameFramework.h"
#include "IOCP_Client.h"
#include "FindingTreasure_Test_Blending.h"

int charac;

//static bool bIsPushed = false;
//static bool bIsDigOrInstall = false;

CGameFramework::CGameFramework()
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDeviceContext = NULL;
	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDepthStencilView = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pWaitingRoomInformDesc = NULL;

	m_pLightSource = NULL;
	m_pShadowRenderer = NULL;
	m_bIsShadow = false;
	m_pLightShaftRenderer = NULL;
	m_bIsLightShaft = false;
	m_pScene = NULL;
	_tcscpy_s(m_pszBuffer, _T("TreasureHunter ("));
	m_pPlayersMgrInform = NULL;
	m_pUI = NULL;
	m_PastXMove = 0;
	m_PastZMove = 0;
	m_PastCameraYRotate = 0;

	//current_time = 0.0f;
}


CGameFramework::~CGameFramework()
{
}


//다음 함수는 응용 프로그램이 실행되면 호출된다는 것에 유의하라. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D 디바이스, 디바이스 컨텍스트, 스왑 체인 등을 생성하는 함수를 호출한다. 
	if (!CreateDirect3DDisplay()) return(false);

	//렌더링할 객체(게임 월드 객체)를 생성한다. 
	//BuildObjects();

	return(true);
}

bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();

	//렌더 타겟과 같은 크기의 깊이 버퍼(Depth Buffer)를 생성한다.
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dDepthStencilBufferDesc.Width = m_nWndClientWidth;
	d3dDepthStencilBufferDesc.Height = m_nWndClientHeight;
	d3dDepthStencilBufferDesc.MipLevels = 1;
	d3dDepthStencilBufferDesc.ArraySize = 1;
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer))) return(false);

	//생성한 깊이 버퍼(Depth Buffer)에 대한 뷰를 생성한다.
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dViewDesc;
	ZeroMemory(&d3dViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dViewDesc, &m_pd3dDepthStencilView))) return(false);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
	printf("디바이스 초기화\n");
	printf("m_pd3dRenderTargetView : %x,m_pd3dDepthStencilView : %x \n", m_pd3dRenderTargetView, m_pd3dDepthStencilView);
	return(true);
}

bool CGameFramework::CreateDirect3DDisplay()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	UINT dwCreateDeviceFlags = 0;
#ifdef _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//디바이스를 생성하기 위하여 시도할 드라이버 유형의 순서를 나타낸다.
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D10_DRIVER_TYPE);

	//디바이스를 생성하기 위하여 시도할 특성 레벨의 순서를 나타낸다.
	D3D_FEATURE_LEVEL d3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(d3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	//생성할 스왑 체인을 서술하는 구조체이다.
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dxgiSwapChainDesc.Windowed = true;
	dxgiSwapChainDesc.Flags = 0;

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hResult = S_OK;
	//디바이스의 드라이버 유형과 특성 레벨을 지원하는 디바이스와 스왑 체인을 생성한다. 고수준의 디바이스 생성을 시도하고 실패하면 다음 수준의 디바이스를 생성한다.
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, d3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	if (!m_pDXGISwapChain || !m_pd3dDevice || !m_pd3dDeviceContext) return(false);

	//렌더 타겟 뷰를 생성하는 함수를 호출한다.
	if (!CreateRenderTargetDepthStencilView()) return(false);

	return(true);
}

CScene* CGameFramework::GetSceneForType(SCENE_TYPE eSceneType)
{
	switch (eSceneType)
	{
	case SCENE_TYPE_JUNGLE:
		return new CJungleScene();
		break;
	case SCENE_TYPE_SNOW:
		return new CSnowScene();
		break;
	}
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	//switch (nMessageID)
	//{
	//case WM_LBUTTONDOWN:
	//	//m_pPlayer->m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
	//	break;
	//case WM_RBUTTONDOWN:
	//	//m_pPlayer->m_CameraOperator.ZoomOutAtOnce(ZOOM_AT_ONCE_DISTANCE);
	//	break;
	//case WM_LBUTTONUP:
	//case WM_RBUTTONUP:
	//	//m_pPlayer->m_CameraOperator.OriginalZoomState();
	//	break;
	//case WM_MOUSEMOVE:
	//	break;
	//default:
	//	break;
	//}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case 'q': case 'Q': case 'e': case'E':
			SetPacket(CAMERAMOVE, 0, 0, 0);
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '9': 
			m_bIsShadow = m_bIsShadow ? false : true;
			break;
		case '0':
			m_bIsLightShaft = m_bIsLightShaft ? false : true;
			break;
		}
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		/*윈도우의 크기가 변경될 때(현재는 “Alt+Enter“ 전체 화면 모드와 윈도우 모드로 전환될 때) 스왑 체인의 후면버퍼 크기를 조정하고 후면버퍼에 대한 렌더 타겟 뷰를 다시 생성한다. */
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);

		if (m_pd3dRenderTargetView) m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

		if (m_pd3dRenderTargetView)
		{
			m_pd3dRenderTargetView->Release();
			m_pd3dRenderTargetView = NULL;
		}
		if (m_pd3dDepthStencilView)
		{
			m_pd3dDepthStencilView->Release();
			m_pd3dDepthStencilView = NULL;
		}
		if (m_pd3dDepthStencilBuffer)
		{
			m_pd3dDepthStencilBuffer->Release();
			m_pd3dDepthStencilBuffer = NULL;
		}

		m_pDXGISwapChain->ResizeBuffers(2, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		CreateRenderTargetDepthStencilView();

		if (m_pShadowRenderer) m_pShadowRenderer->ResizeShadowMapAndView(m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight);
		if (m_pLightShaftRenderer) m_pLightShaftRenderer->ResizeLightShaftMapAndView(m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight);

		m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
		m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.GenerateProjectionMatrix(2.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 60.0f);

		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

//다음 함수는 응용 프로그램이 종료될 때 호출된다는 것에 유의하라. 
void CGameFramework::OnDestroy()
{

	CTextureResource::DestroyTextureResource();
	/*CFBXAnimResource::CreateFBXAnimResource();*/
	CShaderResource::DestroyShaderResource();
	CMeshResource::DestroyMeshResource();			// 메시 리소스를 생성하기 이전에 셰이더 리소스와 FBX 리소스가 마련되어야 한다.
	CMaterialResource::DestroyMaterialResource();
	CBlendingResource::DestroyBlendingResource();

	if (m_pShadowRenderer)
	{
		m_pShadowRenderer->DestroyShadowRenderer();
		delete m_pShadowRenderer;
	}

	if (m_pLightShaftRenderer)
	{
		m_pLightShaftRenderer->DestroyLightShaftRenderer();
		delete m_pLightShaftRenderer;
	}

	if (m_pLightSource)
	{
		delete m_pLightSource;
	}

	if (m_pWaitingRoomInformDesc) delete m_pWaitingRoomInformDesc;				// 완전 소멸
	//게임 객체를 소멸한다. 
	ReleaseObjects();

	m_pPlayersMgrInform->DestroyPlayersMgrInform();

	//Direct3D와 관련된 객체를 소멸한다. 
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->ClearState();			// 완전 소멸 가정
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();		// 완전 소멸 가정
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();	// 완전 소멸 가정
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();		// 완전 소멸 가정
	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();					// 완전 소멸 가정
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();			// 완전 소멸 가정
	if (m_pd3dDevice) m_pd3dDevice->Release();							// 완전 소멸 가정

	_CrtDumpMemoryLeaks();
}

void CGameFramework::BuildObjects()
{
	CTextureResource::CreateTextureResource(m_pd3dDevice, m_pd3dDeviceContext);
	/*CFBXAnimResource::CreateFBXAnimResource();*/
	CShaderResource::CreateShaderResource(m_pd3dDevice);
	CMeshResource::CreateMeshResource(m_pd3dDevice);			// 메시 리소스를 생성하기 이전에 셰이더 리소스와 FBX 리소스가 마련되어야 한다.
	CMaterialResource::CreateMaterialResource(m_pd3dDevice);
	CBlendingResource::CreateBlendingResource(m_pd3dDevice);

	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pd3dDeviceContext->OMSetBlendState(CBlendingResource::pTransparentBlending, blendFactors, 0xffffffff);
	
	/*CShader *pTexturedLightingShader = new CTexturedLightingShader();
	pTexturedLightingShader->CreateShaderVariables(m_pd3dDevice);*/
	
	// 쉐도우 렌더러를 생성한다.
	m_pShadowRenderer = new CShadowRenderer();
	m_pShadowRenderer->CreateShadowMapAndView(m_pd3dDevice);
	m_pShadowRenderer->CreateShaderVariables(m_pd3dDevice);

	// 쉐도우 렌더러에 쓰일 광원을 정의한다.
	m_pLightSource = new CLightSource();
	m_pLightSource->SetMesh(new CSphereMesh(m_pd3dDevice, 20.0f, 20, 20, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)));
	m_pLightSource->SetShader(CShaderResource::pDiffusedShader);
	m_pLightSource->CreateViewMatrix(
		D3DXVECTOR3(-4.0f, 80.0f, -300.0f), D3DXVECTOR3(-0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_pLightSource->CreateProjectionMatrix(10.0f, m_nWndClientWidth, m_nWndClientHeight, 200.0f, 400.0f);/**/

	m_pShadowRenderer->SetLightSource(m_pLightSource);

	// 라이트 셰프트 렌더러를 생성한다.
	m_pLightShaftRenderer = new CLightShaftRenderer();
	m_pLightShaftRenderer->CreateLightShaftSRVAndRTV(m_pd3dDevice);
	m_pLightShaftRenderer->CreateShaderVariables(m_pd3dDevice);							// 광원의 자리를 등록하는 셰이더자원이 할당된다.
	m_pLightShaftRenderer->SetLightSource(m_pLightSource);
	m_pLightShaftRenderer->SetLightShaftProjMesh(CMeshResource::pTexturedRectMesh);
	//플레이어 객체를 생성한다.
	/*
	m_pWaitingRoomInformDesc->m_iMyPlayerID = my_id;

	m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_bIsSlotActive = true;
	m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_BelongType = (BELONG_TYPE)my_team;
	m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_PlayerType = (PLAYER_TYPE)charac;

	for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; ++i)
	{
		if (waitingplayer[i].connect)
		{
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_bIsSlotActive=true;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_BelongType = waitingplayer[i].team;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_PlayerType = (PLAYER_TYPE)waitingplayer[i].charac;
		}
	}*/
	m_pUI = new CGameUI();
	m_pUI->BuildUIObjects(m_pd3dDevice);
	
	//CScene 클래스 객체를 생성하고 CScene 클래스 객체의 BuildObjects() 멤버 함수를 호출한다.
	m_pScene = GetSceneForType(m_pWaitingRoomInformDesc->m_eSceneType);//sWaitingRoomInformDesc.m_eSceneType);//new CScene("new mapfile test/snow0.txt");

	m_pPlayersMgrInform = new CPlayersMgrInform();
	m_pPlayersMgrInform->InitPlayersMgrInform(m_pd3dDevice, *m_pWaitingRoomInformDesc);		// 게임 프레임워크는 대기방에서 얻은 정보로 게임 정보를 세팅한다.
	m_pPlayersMgrInform->SetMaterial(CMaterialResource::pStandardMaterial);
	m_pPlayersMgrInform->SetShader(CShaderResource::pTexturedLightingShader);

	m_pPlayersMgrInform->GetMyPlayer()->InitCameraOperator();
	m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.GenerateProjectionMatrix(2.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 60.0f);
	m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.CreateShaderVariables(m_pd3dDevice);

	m_pLightShaftRenderer->SetCamera(&m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.m_Camera);	// 광원의 위치를 텍스쳐 좌표로 표현하기 위해 카메라를 세팅한다.

	if (m_pScene)
	{
		m_pScene->SetPlayersMgrInform(m_pPlayersMgrInform);
		m_pScene->BuildObjects(m_pd3dDevice);
	}
	WaitingPacket(GAMESET);
}

//void CGameFramework::SetPlayersToScene(CScene *pScene, CPlayer **ppPlayers)
//{
//	pScene->SetPlayers(ppPlayers);
//}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}
	//if (m_ppPlayers) delete []m_ppPlayers;
	if (m_pUI) delete m_pUI;

	//if (m_pd3dcbColor) m_pd3dcbColor->Release();
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	CPlayer *pMyPlayer = m_pPlayersMgrInform->GetMyPlayer();
	if (m_pPlayersMgrInform->m_iMyPlayerID == -1)
		return;
	static bool voxing = false;
	if (m_pPlayersMgrInform->GetMyPlayer()->m_bIsActive)
	{
		if (GetKeyboardState(pKeyBuffer))
		{
			if (pKeyBuffer['A'] & 0xF0)
			{
				if (pKeyBuffer['S'] & 0xF0 || pKeyBuffer['D'] & 0xF0) pMyPlayer->ThrowPlayer();

				if (pKeyBuffer[VK_SPACE] & 0xF0) {
					pMyPlayer->m_d3dxvMoveDir.y = 5.0f;
				}
				if (pKeyBuffer[VK_UP] & 0xF0) {
					pMyPlayer->m_d3dxvMoveDir.z += 2.0f;
				}
				if (pKeyBuffer[VK_DOWN] & 0xF0) {
					pMyPlayer->m_d3dxvMoveDir.z -= 2.0f;
				}
				if (pKeyBuffer[VK_LEFT] & 0xF0) {
					pMyPlayer->m_d3dxvMoveDir.x -= 2.0f;
				}
				if (pKeyBuffer[VK_RIGHT] & 0xF0) {
					pMyPlayer->m_d3dxvMoveDir.x += 2.0f;
				}
				if (m_PastXMove != (int)pMyPlayer->m_d3dxvMoveDir.x || m_PastZMove != (int)pMyPlayer->m_d3dxvMoveDir.z)
				{
					m_PastXMove = (int)pMyPlayer->m_d3dxvMoveDir.x;
					m_PastZMove = (int)pMyPlayer->m_d3dxvMoveDir.z;
					SetPacket(POSMOVE, m_PastXMove, m_PastZMove, 0);
				}
			}
			else
			{
				if (pKeyBuffer['S'] & 0xF0 /*&& !m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed*/ && !pMyPlayer->m_VoxelPocket[pMyPlayer->m_iVoxelPocketSlotIdx].m_bIsActive) {
					pMyPlayer->m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
					pMyPlayer->DigInVoxelTerrain(m_pScene->m_pVoxelTerrain, true, fTimeElapsed, m_pPlayersMgrInform->m_iMyPlayerID);
					pMyPlayer->m_bIsDigOrInstall = true;
					switch (m_pPlayersMgrInform->GetMyPlayer()->m_PlayerType)
					{
					case PLAYER_TYPE_PIRATE:
						m_pPlayersMgrInform->GetMyPlayer()->SetFBXAnimForType(PIRATE_ANIM_TYPE_DIG);
						break;
					default:
						break;
					}
					if (!voxing)
					{
						voxing = true;
						SetPacket(VOXELDEL, 0, 0, 0);
					}
				}
				else if (pKeyBuffer['D'] & 0xF0 /*&& !m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed*/ && pMyPlayer->m_VoxelPocket[pMyPlayer->m_iVoxelPocketSlotIdx].m_bIsActive)
				{
					pMyPlayer->m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
					pMyPlayer->InstallVoxel(m_pScene->m_pVoxelTerrain, true, fTimeElapsed, m_pPlayersMgrInform->m_iMyPlayerID);
					pMyPlayer->m_bIsDigOrInstall = true;
					switch (m_pPlayersMgrInform->GetMyPlayer()->m_PlayerType)
					{
					case PLAYER_TYPE_PIRATE:
						m_pPlayersMgrInform->GetMyPlayer()->SetFBXAnimForType(PIRATE_ANIM_TYPE_DIG);
						break;
					default:
						break;
					}
					if (!voxing)
					{
						voxing = true;
						SetPacket(VOXELINS, 0, 0, 0);
					}
				}
				else
				{
					switch (m_pPlayersMgrInform->GetMyPlayer()->m_PlayerType)
					{
					case PLAYER_TYPE_PIRATE:
						m_pPlayersMgrInform->GetMyPlayer()->SetFBXAnimForType(PIRATE_ANIM_TYPE_DIG);
						break;
					default:
						break;
					}
					if (pKeyBuffer[VK_SPACE] & 0xF0) {
						pMyPlayer->m_d3dxvMoveDir.y = 5.0f;
						SetPacket(JUMP, 0, 0, 0);
					}
					if (pKeyBuffer[VK_UP] & 0xF0) {
						pMyPlayer->m_d3dxvMoveDir.z += 2.0f;
					}
					if (pKeyBuffer[VK_DOWN] & 0xF0) {
						pMyPlayer->m_d3dxvMoveDir.z -= 2.0f;
					}
					if (pKeyBuffer[VK_LEFT] & 0xF0) {
						pMyPlayer->m_d3dxvMoveDir.x -= 2.0f;
					}
					if (pKeyBuffer[VK_RIGHT] & 0xF0) {
						pMyPlayer->m_d3dxvMoveDir.x += 2.0f;
					}
					if (m_PastXMove != (int)pMyPlayer->m_d3dxvMoveDir.x || m_PastZMove != (int)pMyPlayer->m_d3dxvMoveDir.z)
					{
						m_PastXMove = (int)pMyPlayer->m_d3dxvMoveDir.x;
						m_PastZMove = (int)pMyPlayer->m_d3dxvMoveDir.z;
						SetPacket(POSMOVE, m_PastXMove, m_PastZMove, 0);
					}

					pMyPlayer->DigInVoxelTerrain(m_pScene->m_pVoxelTerrain, false, fTimeElapsed, m_pPlayersMgrInform->m_iMyPlayerID);
					pMyPlayer->InstallVoxel(m_pScene->m_pVoxelTerrain, false, fTimeElapsed, m_pPlayersMgrInform->m_iMyPlayerID); 
					pMyPlayer->m_bIsDigOrInstall = false;
					
					if (voxing && (!(pKeyBuffer['S'] & 0xF0) && !(pKeyBuffer['D'] & 0xF0)))
					{
						voxing = false;
						SetPacket(VOXELCANCLE, 0, 0, 0);
					}
				}
			}
			
			// 'S'나 'D'버튼을 떼는 순간에 아래 else문이 작동한다는 것을 감안할 것.
			
			if (pKeyBuffer['F'] & 0xF0)			pMyPlayer->m_CameraOperator.RotateLocalX(CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
			if (pKeyBuffer['R'] & 0xF0)			pMyPlayer->m_CameraOperator.RotateLocalX(-CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
			int currentCameraYRotate = 0;
			if (pKeyBuffer['Q'] & 0xF0) {
				pMyPlayer->m_CameraOperator.RotateLocalY(-CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
				currentCameraYRotate = -1;
			}
			if (pKeyBuffer['E'] & 0xF0) {
				pMyPlayer->m_CameraOperator.RotateLocalY(CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
				currentCameraYRotate = 1;
			}
			if (pKeyBuffer['W'] & 0xF0)			pMyPlayer->m_CameraOperator.ZoomOutAtOnce(ZOOM_AT_ONCE_DISTANCE);
			pMyPlayer->ProofreadLocalAxis();

			if (currentCameraYRotate != m_PastCameraYRotate)
			{
				SetPacket(CAMERAMOVE, 0, 0, currentCameraYRotate);
				m_PastCameraYRotate = currentCameraYRotate;
			}
		}
		pMyPlayer->m_CameraOperator.GenerateViewMatrix(fTimeElapsed, true);
		pMyPlayer->m_CameraOperator.OriginalZoomState();

	}
	//m_pPlayersMgrInform->m_ppPlayers[2]->m_d3dxvMoveDir.x += 2.0f;
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_pd3dDevice, m_pd3dDeviceContext, m_GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	
	 CPlayer *pMyPlayer = NULL;
	if (m_pPlayersMgrInform) pMyPlayer = m_pPlayersMgrInform->GetMyPlayer();
	//타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산한다. 
	m_GameTimer.Tick(/*30.0f*/);

	ProcessInput();

	for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; ++i)
	{
		if (i == m_pPlayersMgrInform->m_iMyPlayerID)
			continue;
		if (!m_pPlayersMgrInform->m_ppPlayers[i])
			continue;
		if (!m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsActive)
			continue;
		if (m_pPlayersMgrInform->m_ppPlayers[i]->m_Insvoxel)
		{
			m_pPlayersMgrInform->m_ppPlayers[i]->m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
			m_pPlayersMgrInform->m_ppPlayers[i]->InstallVoxel(m_pScene->m_pVoxelTerrain, true, m_GameTimer.GetTimeElapsed(), i);
			m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsDigOrInstall = true;
			m_pPlayersMgrInform->m_ppPlayers[i]->SetFBXAnimForType(PIRATE_ANIM_TYPE_DIG);
		}
		else if (m_pPlayersMgrInform->m_ppPlayers[i]->m_Delvoxel)
		{
			m_pPlayersMgrInform->m_ppPlayers[i]->m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
			m_pPlayersMgrInform->m_ppPlayers[i]->DigInVoxelTerrain(m_pScene->m_pVoxelTerrain, true, m_GameTimer.GetTimeElapsed(), i);
			m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsDigOrInstall = true;
			m_pPlayersMgrInform->m_ppPlayers[i]->SetFBXAnimForType(PIRATE_ANIM_TYPE_DIG);
		}
		else
		{
			m_pPlayersMgrInform->m_ppPlayers[i]->DigInVoxelTerrain(m_pScene->m_pVoxelTerrain, false, m_GameTimer.GetTimeElapsed(), i);
			m_pPlayersMgrInform->m_ppPlayers[i]->InstallVoxel(m_pScene->m_pVoxelTerrain, false, m_GameTimer.GetTimeElapsed(), i);
			m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsDigOrInstall = false;
		}
	}

	float fClearColor[4] = { 0.1f, 0.2f, 0.25f, 1.0f };
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	CCamera *pCamera = &(pMyPlayer->m_CameraOperator.m_Camera);//(m_pPlayer) ? m_pPlayer->GetCamera() : NULL;

	//m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	if (!m_bIsShadow)                   // 쉐도우 렌더러가 없다면 일반적인 렌더링을 수행한다.
	{
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
		pMyPlayer->UpdateShaderVariables(m_pd3dDeviceContext);				// 카메라 세팅
		m_pPlayersMgrInform->SetShader(CShaderResource::pTexturedLightingShader);
		m_pScene->m_pVoxelTerrain->SetShader(CShaderResource::pInstTexturedLightingShader);
		m_pLightSource->Render(m_pd3dDeviceContext);
		m_pPlayersMgrInform->Render(m_pd3dDeviceContext);
		if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, pCamera);
	}

	if (m_pShadowRenderer && m_bIsShadow)    // 쉐도우 렌더러가 있다면 그림자 매핑을 수행한다.
	{
		// pass0
		m_pShadowRenderer->BindDSVAndSetNullRenderTarget(m_pd3dDeviceContext);
		m_pShadowRenderer->UpdateShaderPass0Variables(m_pd3dDeviceContext);

		m_pPlayersMgrInform->SetShader(CShaderResource::pShadowMappingPass0Shader);
		m_pScene->m_pVoxelTerrain->SetShader(CShaderResource::pInstShadowMappingPass0Shader);
	
		m_pPlayersMgrInform->Render(m_pd3dDeviceContext);
		m_pScene->Render(m_pd3dDeviceContext, &m_pShadowRenderer->m_pLightSource->m_CameraForCulling);

		// pass1
		pMyPlayer->UpdateShaderVariables(m_pd3dDeviceContext);
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);		
		
		m_pShadowRenderer->UpdateShaderPass1Variables(m_pd3dDeviceContext);

		m_pPlayersMgrInform->SetShader(CShaderResource::pShadowMappingPass1Shader);
		m_pScene->m_pVoxelTerrain->SetShader(CShaderResource::pInstShadowMappingPass1Shader);
		m_pLightSource->Render(m_pd3dDeviceContext);
		
		m_pPlayersMgrInform->Render(m_pd3dDeviceContext);
		m_pScene->Render(m_pd3dDeviceContext, pCamera);					// 렌더링 텍스쳐 => 후면 버퍼.
	}

//	//m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
	if (m_pLightShaftRenderer && m_bIsLightShaft)
	{
		float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		//m_pd3dDeviceContext->OMSetBlendState(NULL, NULL, 0xffffffff);
		float fLightMapBackColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pLightShaftRenderer->m_pLightMapRTV, NULL);
		m_pd3dDeviceContext->ClearRenderTargetView(m_pLightShaftRenderer->m_pLightMapRTV, fLightMapBackColor);			// 라이트맵에 배경색상을 그린다.
		m_pLightShaftRenderer->m_pLightSource->Render(m_pd3dDeviceContext);	

		m_pScene->m_pVoxelTerrain->SetShader(CShaderResource::pInstLightShaftPass0Shader);
		m_pScene->m_pVoxelTerrain->Render(m_pd3dDeviceContext, pCamera, true);
		
		for (int i = 0; i < 2; i++)
		{
			m_pScene->m_pShips[i]->SetShader(CShaderResource::pLightShaftPass0Shader);
			m_pScene->m_pShips[i]->Render(m_pd3dDeviceContext);
			m_pScene->m_pShips[i]->SetShader(CShaderResource::pTexturedLightingShader);
		}

		// 라이트 셰프트 pass1 : 라이트맵을 라이트셰프트맵으로 바꾼다. (일종의 필터링)
		//
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pLightShaftRenderer->m_pLightShaftMapRTV, NULL);
		float fLightShaftMapBackColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_pd3dDeviceContext->ClearRenderTargetView(m_pLightShaftRenderer->m_pLightShaftMapRTV, fLightShaftMapBackColor);
		//
		m_pLightShaftRenderer->UpdateLightShaftPass1Variables(m_pd3dDeviceContext);
		m_pLightShaftRenderer->SetLightShaftPassShader(CShaderResource::pLightShaftPass1Shader);
		
		m_pLightShaftRenderer->Render(m_pd3dDeviceContext);

		// 라이트 셰프트 pass2 : 라이트셰프트맵을 가산 혼합 상태를 이용하여 후면버퍼에 덧씌운다.
		// 라이트 셰프트 맵을 기존 후면 버퍼에 그리기 위하여 가산 혼합 상태를 사용한다.
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
		m_pd3dDeviceContext->OMSetBlendState(CBlendingResource::pAddBlending, blendFactors, 0xffffffff);
		m_pLightShaftRenderer->UpdateLightShaftPass2Variables(m_pd3dDeviceContext);
		m_pLightShaftRenderer->SetLightShaftPassShader(CShaderResource::pLightShaftPass2Shader);
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
		m_pLightShaftRenderer->Render(m_pd3dDeviceContext);
		m_pd3dDeviceContext->OMSetBlendState(CBlendingResource::pTransparentBlending, blendFactors, 0xffffffff);
	}
	

	AnimateObjects();

	static float fTempTime = 180.0f;
	fTempTime -= m_GameTimer.GetTimeElapsed();
	
	if (m_pUI)
	{
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, NULL);
		m_pUI->BackgroundUIRender(m_pd3dDeviceContext);
		m_pUI->PlayerHavingVoxelRender(m_pd3dDeviceContext, pMyPlayer);
		m_pUI->StaminaRender(m_pd3dDeviceContext, pMyPlayer);
		if (!pMyPlayer->m_bIsActive)
		{
			m_pUI->RespawningRender(m_pd3dDeviceContext, &m_pScene->m_RespawnManager, pMyPlayer);
		}
		m_pUI->TimerRender(m_pd3dDeviceContext, fTempTime);
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
	}
	/**/
	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer + 16, 37);
	//current_time += m_GameTimer.GetTimeElapsed();
	::SetWindowText(m_hWnd, m_pszBuffer);
}
