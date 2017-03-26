#include "stdafx.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "VTMakerFramework.h"


CVTMakerFramework::CVTMakerFramework()
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDeviceContext = NULL;
	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDepthStencilView = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	//m_pScene = NULL;
	_tcscpy_s(m_pszBuffer, _T("TreasureHunter ("));
	//m_ppPlayers = NULL;

	m_pWorkingSpace = NULL;
}


CVTMakerFramework::~CVTMakerFramework()
{
}


//다음 함수는 응용 프로그램이 실행되면 호출된다는 것에 유의하라. 
bool CVTMakerFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D 디바이스, 디바이스 컨텍스트, 스왑 체인 등을 생성하는 함수를 호출한다. 
	if (!CreateDirect3DDisplay()) return(false);

	//렌더링할 객체(게임 월드 객체)를 생성한다. 
	BuildObjects();

	return(true);
}

bool CVTMakerFramework::CreateRenderTargetDepthStencilView()
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

	return(true);
}

bool CVTMakerFramework::CreateDirect3DDisplay()
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
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

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

void CVTMakerFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		//m_pPlayer->m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
		break;
	case WM_RBUTTONDOWN:
		//m_pPlayer->m_CameraOperator.ZoomOutAtOnce(ZOOM_AT_ONCE_DISTANCE);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//m_pPlayer->m_CameraOperator.OriginalZoomState();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CVTMakerFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CVTMakerFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		/*윈도우의 크기가 변경될 때(현재는 “Alt+Enter“ 전체 화면 모드와 윈도우 모드로 전환될 때) 스왑 체인의 후면버퍼 크기를 조정하고 후면버퍼에 대한 렌더 타겟 뷰를 다시 생성한다. */
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);

		if (m_pd3dRenderTargetView) m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

		if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
		if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
		if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

		m_pDXGISwapChain->ResizeBuffers(2, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		CreateRenderTargetDepthStencilView();

		//m_ppPlayers[0]->m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);

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
void CVTMakerFramework::OnDestroy()
{
	//게임 객체를 소멸한다. 
	ReleaseObjects();

	//Direct3D와 관련된 객체를 소멸한다. 
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->ClearState();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();
	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
}

void CVTMakerFramework::BuildObjects()
{
	CTextureResource::CreateTextureResource(m_pd3dDevice);
	CMeshResource::CreateMeshResource(m_pd3dDevice);

	m_CameraOperator.InitCameraOperator(NULL);
	m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_CameraOperator.GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
	m_CameraOperator.GenerateViewMatrix();
	m_CameraOperator.CreateShaderVariables(m_pd3dDevice);
}

void CVTMakerFramework::SetWorkingSpace(CWorkingSpace *pWorkingSpace)
{
	if (m_pWorkingSpace) delete m_pWorkingSpace;
	m_pWorkingSpace = pWorkingSpace;
}

void CVTMakerFramework::CreateWorkingSpace(D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal)
{
	CWorkingSpace *pWorkingSpace = new CWorkingSpace();

	pWorkingSpace->CreateWorkingSpace(m_pd3dDevice, d3dxvWorkingSpaceSize, d3dxvCubeSize, d3dxvOffsetFromLocal);
	SetWorkingSpace(pWorkingSpace);
}

void CVTMakerFramework::ReleaseObjects()
{
	if (m_pWorkingSpace) delete m_pWorkingSpace;

	if (m_pd3dcbColor) m_pd3dcbColor->Release();
}

void CVTMakerFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];

	if (GetKeyboardState(pKeyBuffer))
	{
		float fTimeElapsed = m_GameTimer.GetTimeElapsed();
		if (pKeyBuffer['F'] & 0xF0)		m_CameraOperator.RotateLocalX(CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
		if (pKeyBuffer['R'] & 0xF0)		m_CameraOperator.RotateLocalX(-CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
		if (pKeyBuffer['Q'] & 0xF0)		m_CameraOperator.RotateLocalY(-CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
		if (pKeyBuffer['E'] & 0xF0)		m_CameraOperator.RotateLocalY(CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
		if (pKeyBuffer['W'] & 0xF0)		m_CameraOperator.MoveForward(CAMERA_VELOCITY, fTimeElapsed);
		if (pKeyBuffer['S'] & 0xF0)		m_CameraOperator.MoveBack(CAMERA_VELOCITY, fTimeElapsed);
		if (pKeyBuffer['A'] & 0xF0)		m_CameraOperator.MoveLeft(CAMERA_VELOCITY, fTimeElapsed);
		if (pKeyBuffer['D'] & 0xF0)		m_CameraOperator.MoveRight(CAMERA_VELOCITY, fTimeElapsed);
		if (pKeyBuffer['V'] & 0xF0)		m_CameraOperator.MoveUp(CAMERA_VELOCITY, fTimeElapsed);
		if (pKeyBuffer['C'] & 0xF0)		m_CameraOperator.MoveDown(CAMERA_VELOCITY, fTimeElapsed);
		if (pKeyBuffer['X'] & 0xF0)		m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
		if (pKeyBuffer['Z'] & 0xF0)		m_CameraOperator.ZoomOutAtOnce(ZOOM_AT_ONCE_DISTANCE);
	}
	m_CameraOperator.GenerateViewMatrix(true);
	m_CameraOperator.OriginalZoomState();
}

void CVTMakerFramework::AnimateObjects()
{
	//if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

void CVTMakerFramework::FrameAdvance()
{
	//타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산한다. 
	m_GameTimer.Tick();

	ProcessInput();

	AnimateObjects();

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//if (m_ppPlayers[0]) m_ppPlayers[0]->UpdateShaderVariables(m_pd3dDeviceContext);

	m_CameraOperator.UpdateShaderVariables(m_pd3dDeviceContext);

	//CCamera *pCamera = &(m_ppPlayers[0]->m_CameraOperator.m_Camera);//(m_pPlayer) ? m_pPlayer->GetCamera() : NULL;
	//if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, pCamera);
	if (m_pWorkingSpace) m_pWorkingSpace->Render(m_pd3dDeviceContext);
	// if (m_ppPlayers[0]) m_ppPlayers[0]->Render(m_pd3dDeviceContext);
	// if (m_ppPlayers[1]) m_ppPlayers[1]->Render(m_pd3dDeviceContext);

	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer + 16, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);
}
