#include "stdafx.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "ShaderResource.h"
#include "BlendingResource.h"
#include "GameFramework.h"


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

	m_pScene = NULL;
	_tcscpy_s(m_pszBuffer, _T("TreasureHunter ("));
	m_ppPlayers = NULL;
}


CGameFramework::~CGameFramework()
{
}


//���� �Լ��� ���� ���α׷��� ����Ǹ� ȣ��ȴٴ� �Ϳ� �����϶�. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D ����̽�, ����̽� ���ؽ�Ʈ, ���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�. 
	if (!CreateDirect3DDisplay()) return(false);

	//�������� ��ü(���� ���� ��ü)�� �����Ѵ�. 
	BuildObjects();

	return(true);
}

bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();

	//���� Ÿ�ٰ� ���� ũ���� ���� ����(Depth Buffer)�� �����Ѵ�.
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

	//������ ���� ����(Depth Buffer)�� ���� �並 �����Ѵ�.
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dViewDesc;
	ZeroMemory(&d3dViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dViewDesc, &m_pd3dDepthStencilView))) return(false);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

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

	//����̽��� �����ϱ� ���Ͽ� �õ��� ����̹� ������ ������ ��Ÿ����.
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D10_DRIVER_TYPE);

	//����̽��� �����ϱ� ���Ͽ� �õ��� Ư�� ������ ������ ��Ÿ����.
	D3D_FEATURE_LEVEL d3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(d3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	//������ ���� ü���� �����ϴ� ����ü�̴�.
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
	//����̽��� ����̹� ������ Ư�� ������ �����ϴ� ����̽��� ���� ü���� �����Ѵ�. ������� ����̽� ������ �õ��ϰ� �����ϸ� ���� ������ ����̽��� �����Ѵ�.
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, d3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	if (!m_pDXGISwapChain || !m_pd3dDevice || !m_pd3dDeviceContext) return(false);

	//���� Ÿ�� �並 �����ϴ� �Լ��� ȣ���Ѵ�.
	if (!CreateRenderTargetDepthStencilView()) return(false);

	return(true);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		/*case VK_F1:
		case VK_F2:
		case VK_F3:
		{
			D3DXCOLOR d3dxcColor = (wParam == VK_F1) ? D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) : ((wParam == VK_F2) ? D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) : D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
			D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
			m_pd3dDeviceContext->Map(m_pd3dcbColor, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
			D3DXCOLOR *pcbColor = (D3DXCOLOR *)d3dMappedResource.pData;
			*pcbColor = d3dxcColor;
			m_pd3dDeviceContext->Unmap(m_pd3dcbColor, 0);
			break;
		}*/
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		/*�������� ũ�Ⱑ ����� ��(����� ��Alt+Enter�� ��ü ȭ�� ���� ������ ���� ��ȯ�� ��) ���� ü���� �ĸ���� ũ�⸦ �����ϰ� �ĸ���ۿ� ���� ���� Ÿ�� �並 �ٽ� �����Ѵ�. */
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

		m_ppPlayers[0]->m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);

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

//���� �Լ��� ���� ���α׷��� ����� �� ȣ��ȴٴ� �Ϳ� �����϶�. 
void CGameFramework::OnDestroy()
{
	//���� ��ü�� �Ҹ��Ѵ�. 
	ReleaseObjects();

	//Direct3D�� ���õ� ��ü�� �Ҹ��Ѵ�. 
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->ClearState();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();
	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
}

void CGameFramework::BuildObjects()
{
	CTextureResource::CreateTextureResource(m_pd3dDevice);
	CMeshResource::CreateMeshResource(m_pd3dDevice);
	CShaderResource::CreateShaderResource(m_pd3dDevice);
	CMaterialResource::CreateMaterialResource(m_pd3dDevice);
	CBlendingResource::CreateBlendingResource(m_pd3dDevice);

	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pd3dDeviceContext->OMSetBlendState(CBlendingResource::pTransparentBlending, blendFactors, 0xffffffff);
	
	CShader *pTexturedLightingShader = new CTexturedLightingShader();
	pTexturedLightingShader->CreateShaderVariables(m_pd3dDevice);
	
	//CScene Ŭ���� ��ü�� �����ϰ� CScene Ŭ���� ��ü�� BuildObjects() ��� �Լ��� ȣ���Ѵ�.
	m_pScene = new CScene();
	//�÷��̾� ��ü�� �����Ѵ�.
	m_ppPlayers = new CPlayer*[2];

	m_ppPlayers[0] = new CPlayer();
	m_ppPlayers[1] = new CPlayer();

	CMaterial *pStandardMaterial = new CMaterial();
	pStandardMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 8.0f);
	pStandardMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	m_ppPlayers[0]->SetMaterial(pStandardMaterial);
	m_ppPlayers[0]->SetMesh(CMeshResource::pPirateMesh);
	m_ppPlayers[0]->SetShader(pTexturedLightingShader);
	CTexture *pTexture = new CTexture(1);
	pTexture->SetTexture(0, CTextureResource::ppd3dTexture[TEXTURE_GRASS_0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	m_ppPlayers[0]->SetTexture(pTexture);
	m_ppPlayers[0]->SetPosition(-7.0f, 40.0f, 7.0f);

	m_ppPlayers[0]->InitCameraOperator();
	m_ppPlayers[0]->m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_ppPlayers[0]->m_CameraOperator.GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
	//m_ppPlayers[0]->m_CameraOperator.GenerateViewMatrix();
	m_ppPlayers[0]->m_CameraOperator.CreateShaderVariables(m_pd3dDevice);

	m_ppPlayers[1]->SetMaterial(pStandardMaterial);
	m_ppPlayers[1]->SetMesh(CMeshResource::pPirateMesh);
	m_ppPlayers[1]->SetShader(pTexturedLightingShader);
	CTexture *pTexture2 = new CTexture(1);
	pTexture2->SetTexture(0, CTextureResource::ppd3dTexture[TEXTURE_WOOD_0], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
	m_ppPlayers[1]->SetTexture(pTexture2);
	m_ppPlayers[1]->SetPosition(-7.0f, 40.0f, 3.0f);

	m_ppPlayers[1]->InitCameraOperator();
	m_ppPlayers[1]->m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_ppPlayers[1]->m_CameraOperator.GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
	//m_ppPlayers[1]->m_CameraOperator.GenerateViewMatrix();
	m_ppPlayers[1]->m_CameraOperator.CreateShaderVariables(m_pd3dDevice);

	if (m_pScene)
	{
		m_pScene->BuildObjects(m_pd3dDevice);
		m_pScene->SetPlayers(m_ppPlayers);
	}
}

void CGameFramework::SetPlayersToScene(CScene *pScene, CPlayer **ppPlayers)
{
	pScene->SetPlayers(ppPlayers);
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
	if (m_ppPlayers) delete []m_ppPlayers;

	if (m_pd3dcbColor) m_pd3dcbColor->Release();
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	if (GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_SPACE] & 0xF0)	m_ppPlayers[0]->m_d3dxvMoveDir.y = 5.0f;
		if (pKeyBuffer['F'] & 0xF0)			m_ppPlayers[0]->m_CameraOperator.RotateLocalX(CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
		if (pKeyBuffer['R'] & 0xF0)			m_ppPlayers[0]->m_CameraOperator.RotateLocalX(-CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
		if (pKeyBuffer['Q'] & 0xF0)			m_ppPlayers[0]->m_CameraOperator.RotateLocalY(-CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
		if (pKeyBuffer['E'] & 0xF0)			m_ppPlayers[0]->m_CameraOperator.RotateLocalY(CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
		if (pKeyBuffer[VK_UP] & 0xF0)		m_ppPlayers[0]->m_d3dxvMoveDir.z += 2.0f;
		if (pKeyBuffer[VK_DOWN] & 0xF0)		m_ppPlayers[0]->m_d3dxvMoveDir.z -= 2.0f;
		if (pKeyBuffer[VK_LEFT] & 0xF0)		m_ppPlayers[0]->m_d3dxvMoveDir.x -= 2.0f;
		if (pKeyBuffer[VK_RIGHT] & 0xF0)	m_ppPlayers[0]->m_d3dxvMoveDir.x += 2.0f;
		
		if (pKeyBuffer['S'] & 0xF0)	m_ppPlayers[0]->m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
		if (pKeyBuffer['W'] & 0xF0)	m_ppPlayers[0]->m_CameraOperator.ZoomOutAtOnce(ZOOM_AT_ONCE_DISTANCE);
		m_ppPlayers[0]->ProofreadLocalAxis();
	}
	m_ppPlayers[0]->m_CameraOperator.GenerateViewMatrix(fTimeElapsed, true);
	m_ppPlayers[0]->m_CameraOperator.OriginalZoomState();
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_pd3dDeviceContext, m_GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	//Ÿ�̸��� �ð��� ���ŵǵ��� �ϰ� ������ ����Ʈ�� ����Ѵ�. 
	m_GameTimer.Tick();

	ProcessInput();

	AnimateObjects();

	float fClearColor[4] = { 0.9f, 0.4f, 0.2f, 1.0f };
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	if (m_ppPlayers[0]) m_ppPlayers[0]->UpdateShaderVariables(m_pd3dDeviceContext);
	if (m_ppPlayers[0]) m_ppPlayers[0]->Render(m_pd3dDeviceContext);
	if (m_ppPlayers[1]) m_ppPlayers[1]->Render(m_pd3dDeviceContext);
	CCamera *pCamera = &(m_ppPlayers[0]->m_CameraOperator.m_Camera);//(m_pPlayer) ? m_pPlayer->GetCamera() : NULL;
	if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, pCamera);
	

	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer + 16, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);
}
