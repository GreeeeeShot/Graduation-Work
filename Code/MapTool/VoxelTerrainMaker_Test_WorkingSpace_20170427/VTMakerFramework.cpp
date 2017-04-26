#include "stdafx.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "ShaderResource.h"
#include "LightResource.h"
#include "BlendingResource.h"
#include "VTMakerFramework.h"

bool bIsCreateButton = false;
bool bIsDestroyButton = false;
bool bIsSpecialButton1 = false;
bool bIsSpecialButton2 = false;
int iMouseX = 0, iMouseY = 0;

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

	//m_pWorkingSpace = NULL;
	ZeroMemory(m_pWorkingSpace, sizeof(CWorkingSpace*) * WORKINGSPACE_MAX_NUM);
	m_iWorkingSpaceNum = 0;
	m_iActivedWorkingSpaceIdx = 0;
	m_pVoxelTerrain = NULL;

	m_pLights = NULL;
	m_pd3dcbLights = NULL;
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
	printf("디바이스 초기화\n");
	printf("m_pd3dRenderTargetView : %x,m_pd3dDepthStencilView : %x \n", m_pd3dRenderTargetView, m_pd3dDepthStencilView);
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
	case WM_MOUSEMOVE:
		if (m_pWorkingSpace[m_iActivedWorkingSpaceIdx] && !bIsSpecialButton1)
		{
			if (bIsCreateButton)
			{
				m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->CreateVoxelObject(m_pd3dDevice, &m_CameraOperator.m_Camera, LOWORD(lParam), HIWORD(lParam));
			}
			else if (bIsDestroyButton)
			{
				m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->DestroyVoxelObject(m_pd3dDevice, &m_CameraOperator.m_Camera, LOWORD(lParam), HIWORD(lParam));
			}
			m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->Render(m_pd3dDeviceContext, true);
		}
		break;
	case WM_LBUTTONDOWN:
		iMouseX = LOWORD(lParam);
		iMouseY = HIWORD(lParam);
		bIsCreateButton = true;
		break;
	case WM_RBUTTONDOWN:
		iMouseX = LOWORD(lParam);
		iMouseY = HIWORD(lParam);
		bIsDestroyButton = true;
		break;
	case WM_LBUTTONUP:
		if (m_pWorkingSpace[m_iActivedWorkingSpaceIdx] && bIsSpecialButton1)
		{
			m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->CreateVoxelObjectByPaint(m_pd3dDevice, &m_CameraOperator.m_Camera, iMouseX, iMouseY, LOWORD(lParam), HIWORD(lParam));
		}
		iMouseX = 0;
		iMouseY = 0;
		bIsCreateButton = false;
		break;
	case WM_RBUTTONUP:
		if (m_pWorkingSpace[m_iActivedWorkingSpaceIdx] && bIsSpecialButton1)
		{
			m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->DestroyVoxelObjectByPaint(m_pd3dDevice, &m_CameraOperator.m_Camera, iMouseX, iMouseY, LOWORD(lParam), HIWORD(lParam));
		}
		iMouseX = 0;
		iMouseY = 0;
		bIsDestroyButton = false;
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
		case VK_SPACE:
			bIsSpecialButton1 = false;
			break;
		case VK_SHIFT:
			bIsSpecialButton2 = false;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
			bIsSpecialButton1 = true;
			break;
		case VK_UP:
			if (m_pWorkingSpace[m_iActivedWorkingSpaceIdx])
			{
				if(!bIsSpecialButton1) m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->MoveForwardVoxelTerrain();
				else m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->MoveUpVoxelTerrain();
			}
			break;
		case VK_DOWN:
			if (m_pWorkingSpace[m_iActivedWorkingSpaceIdx])
			{
				if (!bIsSpecialButton1) m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->MoveRearVoxelTerrain();
				else m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->MoveDownVoxelTerrain();
			}
			break;
		case VK_RIGHT:
			if (m_pWorkingSpace[m_iActivedWorkingSpaceIdx]) m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->MoveRightVoxelTerrain();
			break;
		case VK_LEFT:
			if (m_pWorkingSpace[m_iActivedWorkingSpaceIdx]) m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->MoveLeftVoxelTerrain();
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

		m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
		m_CameraOperator.GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);

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
	CShaderResource::CreateShaderResource(m_pd3dDevice);
	CMaterialResource::CreateMaterialResource(m_pd3dDevice);
	CBlendingResource::CreateBlendingResource(m_pd3dDevice);

	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pd3dDeviceContext->OMSetBlendState(CBlendingResource::pTransparentBlending, blendFactors, 0xffffffff);

	m_CameraOperator.InitCameraOperator(NULL);
	m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_CameraOperator.GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
//	m_CameraOperator.GenerateViewMatrix();
	m_CameraOperator.CreateShaderVariables(m_pd3dDevice);

	CreateShaderVariables(m_pd3dDevice);
}

void CVTMakerFramework::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	//게임 월드 전체를 비추는 주변조명을 설정한다.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.05f, 0.1f, 0.05f, 1.0f);

	//3개의 조명(점 광원, 스팟 광원, 방향성 광원)을 설정한다.
	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(-20.0f, 20.0f, -20.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = 1.0f;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 700.0f;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.001f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(0.6f, 0.05f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3(0.0f, 30.0f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 18.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(D3DXToRadian(50.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(20.0f));
	m_pLights->m_pLights[2].m_bEnable = 1.0f;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.2f, 0.2f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(-5.0f, -5.0f, 5.0f);
	m_pLights->m_pLights[3].m_bEnable = 1.0f;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcDiffuse = D3DXCOLOR(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	m_pLights->m_pLights[3].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 20.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(D3DXToRadian(15.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(LIGHTS);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights);
}

void CVTMakerFramework::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT, 1, &m_pd3dcbLights);
}

void CVTMakerFramework::ReleaseShaderVariables()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
}


void CVTMakerFramework::SetWorkingSpace(CWorkingSpace *pWorkingSpace)
{
	if (m_pWorkingSpace[m_iActivedWorkingSpaceIdx])
	{
		delete m_pWorkingSpace[m_iActivedWorkingSpaceIdx];
		m_iWorkingSpaceNum--;
	}
	m_pWorkingSpace[m_iActivedWorkingSpaceIdx] = pWorkingSpace;
	m_iWorkingSpaceNum++;
}

void CVTMakerFramework::SetVoxelTerrain(CVoxelTerrain *pVoxelTerrain)
{
	if (m_pVoxelTerrain) delete m_pVoxelTerrain;
	m_pVoxelTerrain = pVoxelTerrain;
}

void CVTMakerFramework::CreateWorkingSpace(D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal)
{
	CWorkingSpace *pWorkingSpace = new CWorkingSpace();

	pWorkingSpace->CreateWorkingSpace(m_pd3dDevice, d3dxvWorkingSpaceSize, d3dxvCubeSize, d3dxvOffsetFromLocal);
	SetWorkingSpace(pWorkingSpace);
}

void CVTMakerFramework::ReviseWorkingSpace(D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal)
{
	if (!m_pWorkingSpace[m_iActivedWorkingSpaceIdx]) CreateWorkingSpace(d3dxvWorkingSpaceSize, d3dxvCubeSize, d3dxvOffsetFromLocal);
	else m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->ReviseWorkingSpace(m_pd3dDevice, d3dxvWorkingSpaceSize, d3dxvCubeSize, d3dxvOffsetFromLocal);
}
void CVTMakerFramework::OptimizeWorkingSpace(void)
{
	if (!m_pWorkingSpace[m_iActivedWorkingSpaceIdx]) return;
	m_pWorkingSpace[m_iActivedWorkingSpaceIdx]->OptimizeWorkingSpace(m_pd3dDevice);
}

void CVTMakerFramework::CreateWorkingSpaceFromFile(const char* pFileName)
{
	CWorkingSpace *pWorkingSpace = new CWorkingSpace();

	FILE* bluePrintFp;

	fopen_s(&bluePrintFp, pFileName, "r");

	if (bluePrintFp == NULL)
	{
		exit(1);
	}

	D3DXVECTOR3 d3dxvWorkingSpaceSize;
	D3DXVECTOR3 d3dxvCubeSize;
	D3DXVECTOR3 d3dxvOffsetFromLocal;

	fscanf_s(bluePrintFp, " %f %f %f %f %f %f %f %f %f",
		&d3dxvWorkingSpaceSize.y, &d3dxvWorkingSpaceSize.z, &d3dxvWorkingSpaceSize.x,
		&d3dxvCubeSize.x, &d3dxvCubeSize.y, &d3dxvCubeSize.z,
		&d3dxvOffsetFromLocal.x, &d3dxvOffsetFromLocal.y, &d3dxvOffsetFromLocal.z);//,

	printf("d3dxvWorkingSpaceSize :  (%f, %f, %f) \n", d3dxvWorkingSpaceSize.y, d3dxvWorkingSpaceSize.z, d3dxvWorkingSpaceSize.x);
	printf("d3dxvCubeSize :  (%f, %f, %f) \n", d3dxvCubeSize.x, d3dxvCubeSize.y, d3dxvCubeSize.z);
	printf("d3dxvOffsetFromLocal :  (%f, %f, %f) \n" , d3dxvOffsetFromLocal.x, d3dxvOffsetFromLocal.y, d3dxvOffsetFromLocal.z);
				
	pWorkingSpace->CreateWorkingSpace(m_pd3dDevice, d3dxvWorkingSpaceSize, d3dxvCubeSize, d3dxvOffsetFromLocal);
	VOXEL_TYPE iSelectedType;
	CVoxel* pSelectedVoxel;
	int iLinearIdx;

	for (int layer = 0; layer < (int)d3dxvWorkingSpaceSize.y; layer++)
	{
		for (int row = 0; row < (int)d3dxvWorkingSpaceSize.z; row++)
		{
			for (int col = 0; col < (int)d3dxvWorkingSpaceSize.x; col++)
			{
				fscanf_s(bluePrintFp, " %d", &(pWorkingSpace->m_iBlueprint[layer][row][col]));	//텍스쳐 값을 입력받는다.
				printf("%d ", pWorkingSpace->m_iBlueprint[layer][row][col]);
				if (iSelectedType = (VOXEL_TYPE)pWorkingSpace->m_iBlueprint[layer][row][col])
				{
					iLinearIdx = layer * pWorkingSpace->m_iLayerStride + row * pWorkingSpace->m_iRowStride + col;
					pWorkingSpace->m_ppVoxelObjects[iLinearIdx] = pWorkingSpace->GetVoxelForType(iSelectedType);
					pWorkingSpace->m_ppVoxelObjects[iLinearIdx]->SetShader(CShaderResource::pTexturedLightingShader);
					pWorkingSpace->m_ppVoxelObjects[iLinearIdx]->SetMaterial(CMaterialResource::pStandardMaterial);
					D3DXVECTOR4 d3dxvPosition;
					D3DXVec3Transform(&d3dxvPosition, &D3DXVECTOR3(col, layer, row), &pWorkingSpace->m_d3dxmtxFromIdxToPos);
					pWorkingSpace->m_ppVoxelObjects[iLinearIdx]->SetPosition(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);
					////printf("d3dxvPosition : (%f, %f, %f) \n", d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);
					/*pSelectedVoxel = pWorkingSpace->GetVoxelForType(iSelectedType);
					pSelectedVoxel->SetMaterial(CMaterialResource::pStandardMaterial);
					D3DXVec3Transform(&d3dxvPosition, &D3DXVECTOR3(col, layer, row), &pWorkingSpace->m_d3dxmtxFromIdxToPos);
					pSelectedVoxel->SetPosition(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);

					m_vecVoxelObjectsForRendering[iSelectedType].push_back(pSelectedVoxel);*/
					//m_nVoxelObjectsNum++;
				}
			}
			printf("\n");
		}
		printf("\n");
	}

	fclose(bluePrintFp);
	SetWorkingSpace(pWorkingSpace);
}

void CVTMakerFramework::ReleaseObjects()
{
	for (int i = 0; i < WORKINGSPACE_MAX_NUM; i++)
	{
		if (m_pWorkingSpace[i]) delete m_pWorkingSpace[i];
	}
	

	if (m_pd3dcbColor) m_pd3dcbColor->Release();
}

void CVTMakerFramework::ChangeWorkingSpace(int iIdx)
{
	if (0 <= iIdx && iIdx < WORKINGSPACE_MAX_NUM)
	{
		m_iActivedWorkingSpaceIdx = iIdx;
	}
}

void CVTMakerFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];

	float fTimeElapsed;
	if (GetKeyboardState(pKeyBuffer))
	{
		fTimeElapsed = m_GameTimer.GetTimeElapsed();
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
	m_CameraOperator.GenerateViewMatrix(fTimeElapsed, false);
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

	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(m_pd3dDeviceContext, m_pLights);
	//CCamera *pCamera = &(m_ppPlayers[0]->m_CameraOperator.m_Camera);//(m_pPlayer) ? m_pPlayer->GetCamera() : NULL;
	//if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, pCamera);

	bool bIsActived = false;

	for (int i = 0; i < WORKINGSPACE_MAX_NUM; i++)
	{
		bIsActived = false;
		if (m_iActivedWorkingSpaceIdx == i) bIsActived = true;
		if (m_pWorkingSpace[i]) m_pWorkingSpace[i]->Render(m_pd3dDeviceContext, bIsActived);
	}

	//if (m_pVoxelTerrain) m_pVoxelTerrain->Render(m_pd3dDeviceContext);
	// if (m_ppPlayers[0]) m_ppPlayers[0]->Render(m_pd3dDeviceContext);
	// if (m_ppPlayers[1]) m_ppPlayers[1]->Render(m_pd3dDeviceContext);

	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer + 16, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);
}
