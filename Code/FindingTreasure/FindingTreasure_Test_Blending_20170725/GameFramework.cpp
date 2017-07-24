#include "stdafx.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "ShaderResource.h"
#include "BlendingResource.h"
#include "EffectManager.h"
#include "FBXAnim.h"
#include "GameFramework.h"
#include "IOCP_Client.h"

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

	m_pShadowRenderer = NULL;
	m_pLightShaftRenderer = NULL;
	m_pScene = NULL;
	_tcscpy_s(m_pszBuffer, _T("TreasureHunter ("));
	m_pPlayersMgrInform = NULL;
	m_pUI = NULL;

	//current_time = 0.0f;
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
	printf("����̽� �ʱ�ȭ\n");
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
		default:
			break;
		}
		m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed = false;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'a': case 'A':
		case '1': case '2': case '3': case '4' : case '5':
			m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
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
		/*�������� ũ�Ⱑ ����� ��(����� ��Alt+Enter�� ��ü ȭ�� ���� ������ ���� ��ȯ�� ��) ���� ü���� �ĸ���� ũ�⸦ �����ϰ� �ĸ���ۿ� ���� ���� Ÿ�� �並 �ٽ� �����Ѵ�. */
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

		CCamera *pUI_Camera = new CCamera();
		pUI_Camera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
		pUI_Camera->GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
		pUI_Camera->CreateShaderVariables(m_pd3dDevice);		// �̷������δ� ��� ��.

		m_pUI->SetCamera(pUI_Camera);

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
	CFBXAnimResource::CreateFBXAnimResource();
	CShaderResource::CreateShaderResource(m_pd3dDevice);
	CMeshResource::CreateMeshResource(m_pd3dDevice);			// �޽� ���ҽ��� �����ϱ� ������ ���̴� ���ҽ��� FBX ���ҽ��� ���õǾ�� �Ѵ�.
	CMaterialResource::CreateMaterialResource(m_pd3dDevice);
	CBlendingResource::CreateBlendingResource(m_pd3dDevice);

	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pd3dDeviceContext->OMSetBlendState(CBlendingResource::pTransparentBlending, blendFactors, 0xffffffff);
	
	/*CShader *pTexturedLightingShader = new CTexturedLightingShader();
	pTexturedLightingShader->CreateShaderVariables(m_pd3dDevice);*/
	
	// ������ �������� �����Ѵ�.
	m_pShadowRenderer = new CShadowRenderer();
	m_pShadowRenderer->CreateShadowMapAndView(m_pd3dDevice);
	m_pShadowRenderer->CreateShaderVariables(m_pd3dDevice);

	// ������ �������� ���� ������ �����Ѵ�.
	m_pShadowRenderer->m_cLightSource.SetMesh(new CSphereMesh(m_pd3dDevice, 5.0f, 20, 20, D3DXCOLOR(2.5f, 2.5f, 2.5f, 1.0f)));
	m_pShadowRenderer->m_cLightSource.SetShader(CShaderResource::pDiffusedShader);
	m_pShadowRenderer->m_cLightSource.CreateViewMatrix(
		D3DXVECTOR3(-0.0f, 10.0f/*40.0f*/, -60.0f), D3DXVECTOR3(-0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_pShadowRenderer->m_cLightSource.CreateProjectionMatrix(70.0f, 300.0f, 200.0f, 10.0f, 100.0f); // ���� ����

	// ����Ʈ ����Ʈ �������� �����Ѵ�.
	m_pLightShaftRenderer = new CLightShaftRenderer();
	m_pLightShaftRenderer->CreateLightShaftSRVAndRTV(m_pd3dDevice);
	m_pLightShaftRenderer->CreateShaderVariables(m_pd3dDevice);
	m_pLightShaftRenderer->SetLightSource(&m_pShadowRenderer->m_cLightSource);
	

	//CScene Ŭ���� ��ü�� �����ϰ� CScene Ŭ���� ��ü�� BuildObjects() ��� �Լ��� ȣ���Ѵ�.
	m_pScene = new CScene("new mapfile test/jungle0.txt");
	//�÷��̾� ��ü�� �����Ѵ�.

	m_pUI = new CGameUI();

	CCamera *pUI_Camera = new CCamera();
	pUI_Camera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	pUI_Camera->GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
	pUI_Camera->CreateShaderVariables(m_pd3dDevice);		// �̷������δ� ��� ��.

	m_pUI->SetCamera(pUI_Camera);
	m_pUI->BuildUIObjects(m_pd3dDevice);
	
	m_pPlayersMgrInform = new CPlayersMgrInform();
	m_pPlayersMgrInform->m_iMyPlayerID = 4;
	m_pPlayersMgrInform->m_iPlayersNum = 8;
	m_pPlayersMgrInform->m_ppPlayers = new CPlayer*[m_pPlayersMgrInform->m_iPlayersNum];

	for (int i = 0; i <m_pPlayersMgrInform->m_iPlayersNum; i++)
	{
		m_pPlayersMgrInform->m_ppPlayers[i] = new CPlayer();

		m_pPlayersMgrInform->m_ppPlayers[i]->SetMaterial(CMaterialResource::pStandardMaterial);
		m_pPlayersMgrInform->m_ppPlayers[i]->SetMesh(new CTexturedLightingPirateMesh(m_pd3dDevice));
		m_pPlayersMgrInform->m_ppPlayers[i]->SetShader(CShaderResource::pTexturedLightingShader);
		m_pPlayersMgrInform->m_ppPlayers[i]->SetTexture(CTextureResource::pPirateTexture);
		m_pPlayersMgrInform->m_ppPlayers[i]->SetBelongType(i < 4? BELONG_TYPE_BLUE : BELONG_TYPE_RED);
	}

	m_pPlayersMgrInform->GetMyPlayer()->InitCameraOperator();
	m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.GenerateProjectionMatrix(2.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 60.0f);
	//m_ppPlayers[0]->m_CameraOperator.GenerateViewMatrix();
	m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.CreateShaderVariables(m_pd3dDevice);

	m_pLightShaftRenderer->SetCamera(&m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.m_Camera);
	m_pLightShaftRenderer->SetLightShaftProjMesh(CMeshResource::pTexturedRectMesh);

	//m_ppPlayers[1]->SetMaterial(CMaterialResource::pStandardMaterial);
	//m_ppPlayers[1]->SetMesh(CMeshResource::pPirateMesh);
	//m_ppPlayers[1]->SetShader(CShaderResource::pTexturedLightingShader);
	//m_ppPlayers[1]->SetTexture(CTextureResource::pWoodTexture);
	//m_ppPlayers[1]->SetPosition(-7.0f, 40.0f, 3.0f);

	//m_ppPlayers[1]->InitCameraOperator();
	//m_ppPlayers[1]->m_CameraOperator.SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	//m_ppPlayers[1]->m_CameraOperator.GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 90.0f);
	//m_ppPlayers[1]->m_CameraOperator.CreateShaderVariables(m_pd3dDevice);

	if (m_pScene)
	{
		m_pScene->SetPlayersMgrInform(m_pPlayersMgrInform);
		m_pScene->BuildObjects(m_pd3dDevice);
	}
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
	if (m_pPlayersMgrInform->GetMyPlayer()->m_bIsActive)
	{
		if (GetKeyboardState(pKeyBuffer))
		{
			if (pKeyBuffer['S'] & 0xF0 && !pMyPlayer->m_VoxelPocket[pMyPlayer->m_iVoxelPocketSlotIdx].m_bIsActive) {
				pMyPlayer->m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
				pMyPlayer->DigInVoxelTerrain(m_pScene->m_pVoxelTerrain, true, fTimeElapsed);
				pMyPlayer->m_bIsDigOrInstall = true;
				pMyPlayer->SetFBXAnimForType(PIRATE_ANIM_TYPE_DIG);
			}
			else if (pKeyBuffer['D'] & 0xF0 && pMyPlayer->m_VoxelPocket[pMyPlayer->m_iVoxelPocketSlotIdx].m_bIsActive)
			{
				pMyPlayer->m_CameraOperator.ZoomInAtOnce(ZOOM_AT_ONCE_DISTANCE);
				pMyPlayer->InstallVoxel(m_pScene->m_pVoxelTerrain, true, fTimeElapsed);
				pMyPlayer->m_bIsDigOrInstall = true;
				pMyPlayer->SetFBXAnimForType(PIRATE_ANIM_TYPE_DIG);
			}
			// 'S'�� 'D'��ư�� ���� ������ �Ʒ� else���� �۵��Ѵٴ� ���� ������ ��.
			else
			{
				pMyPlayer->SetFBXAnimForType(PIRATE_ANIM_TYPE_IDLE);
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
				pMyPlayer->DigInVoxelTerrain(m_pScene->m_pVoxelTerrain, false, fTimeElapsed);
				pMyPlayer->InstallVoxel(m_pScene->m_pVoxelTerrain, false, fTimeElapsed);
				pMyPlayer->m_bIsDigOrInstall = false;
			}
			if (pKeyBuffer['F'] & 0xF0)			pMyPlayer->m_CameraOperator.RotateLocalX(CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
			if (pKeyBuffer['R'] & 0xF0)			pMyPlayer->m_CameraOperator.RotateLocalX(-CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
			if (pKeyBuffer['Q'] & 0xF0)			pMyPlayer->m_CameraOperator.RotateLocalY(-CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
			if (pKeyBuffer['E'] & 0xF0)			pMyPlayer->m_CameraOperator.RotateLocalY(CAMERA_ROTATION_DEGREE_PER_SEC, fTimeElapsed);
			if (pKeyBuffer['W'] & 0xF0)			pMyPlayer->m_CameraOperator.ZoomOutAtOnce(ZOOM_AT_ONCE_DISTANCE);
			pMyPlayer->ProofreadLocalAxis();
		}
		pMyPlayer->m_CameraOperator.GenerateViewMatrix(fTimeElapsed, true);
		pMyPlayer->m_CameraOperator.OriginalZoomState();

	}
	//m_pPlayersMgrInform->m_ppPlayers[2]->m_d3dxvMoveDir.x += 2.0f;
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_pd3dDeviceContext, m_GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	 CPlayer *pMyPlayer = NULL;
	if (m_pPlayersMgrInform) pMyPlayer = m_pPlayersMgrInform->GetMyPlayer();
	//Ÿ�̸��� �ð��� ���ŵǵ��� �ϰ� ������ ����Ʈ�� ����Ѵ�. 
	m_GameTimer.Tick(/*30.0f*/);

	ProcessInput();

	

	float fClearColor[4] = { 0.1f, 0.2f, 0.25f, 1.0f };
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	CCamera *pCamera = &(pMyPlayer->m_CameraOperator.m_Camera);//(m_pPlayer) ? m_pPlayer->GetCamera() : NULL;

	if (m_pShadowRenderer)    // ������ �������� �ִٸ� �׸��� ������ �����Ѵ�.
	{
		// pass0
		m_pShadowRenderer->BindDSVAndSetNullRenderTarget(m_pd3dDeviceContext);
		m_pShadowRenderer->UpdateShaderPass0Variables(m_pd3dDeviceContext);

		for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
		{
			m_pPlayersMgrInform->m_ppPlayers[i]->SetShader(CShaderResource::pShadowMappingPass0Shader);
		}
		m_pScene->m_pVoxelTerrain->SetShader(CShaderResource::pShadowMappingPass0Shader);

		for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
		{
			if (m_pPlayersMgrInform->m_ppPlayers[i] && m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsActive)
			{
				m_pPlayersMgrInform->m_ppPlayers[i]->Render(m_pd3dDeviceContext);
			}
		}
		m_pScene->Render(m_pd3dDeviceContext, NULL);

		// pass1
		pMyPlayer->UpdateShaderVariables(m_pd3dDeviceContext);
		if (m_pLightShaftRenderer)
		{
			// pass 0 : m_pLightMapRTV�� ���� Ÿ���� �׸���.
			//m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pLightShaftRenderer->m_pLightMapRTV, m_pd3dDepthStencilView);
			//float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			//m_pd3dDeviceContext->OMSetBlendState(0, NULL, NULL);
			m_pLightShaftRenderer->UpdateLightShaftPass1Variables(m_pd3dDeviceContext);
			float fLightMapBackColor[4] = { 0.4f, 0.1f, 0.1f, 1.0f };
			m_pd3dDeviceContext->ClearRenderTargetView(m_pLightShaftRenderer->m_pLightMapRTV, fLightMapBackColor);			// ����Ʈ�ʿ� �������� �׸���.
			m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pLightShaftRenderer->m_pLightMapRTV, NULL);							// ����Ʈ �ʿ� ���� ������Ʈ�� �׸���.
			m_pLightShaftRenderer->m_pLightSource->Render(m_pd3dDeviceContext);												// ����Ʈ�ʿ� ������ �׸���.

			//m_pd3dDeviceContext->OMSetRenderTargets(1, NULL, NULL);
			//float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			//m_pd3dDeviceContext->OMSetBlendState(0, NULL, NULL);
			ID3D11RenderTargetView *rendertargets[2] = { m_pd3dRenderTargetView ,m_pLightShaftRenderer->m_pLightMapRTV  };
			//if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
			m_pd3dDeviceContext->OMSetRenderTargets(2, rendertargets, m_pd3dDepthStencilView);
		}
		else
		{
			m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
		}

		m_pShadowRenderer->UpdateShaderPass1Variables(m_pd3dDeviceContext);
		for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
		{
			m_pPlayersMgrInform->m_ppPlayers[i]->SetShader(CShaderResource::pShadowMappingPass1Shader);
		}
		m_pScene->m_pVoxelTerrain->SetShader(CShaderResource::pShadowMappingPass1Shader);

		for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
		{
			if (m_pPlayersMgrInform->m_ppPlayers[i] && m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsActive)
			{
				m_pPlayersMgrInform->m_ppPlayers[i]->Render(m_pd3dDeviceContext);
			}
		}
		m_pScene->Render(m_pd3dDeviceContext, pCamera);					// ������ �ؽ��� => �ĸ� ����.
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
		if (m_pLightShaftRenderer)
		{
			// ����Ʈ ����Ʈ pass1
			
			float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			//m_pd3dDeviceContext->OMSetBlendState(CBlendingResource::pAddBlending, blendFactors, 0xffffffff);
			m_pLightShaftRenderer->UpdateLightShaftPass1Variables(m_pd3dDeviceContext);
			m_pLightShaftRenderer->SetLightShaftPassShader(CShaderResource::pLightShaftPass1Shader);
			m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, 0);
			m_pLightShaftRenderer->Render(m_pd3dDeviceContext);
			
			/*
			// ����Ʈ ����Ʈ pass2 
			// ����Ʈ ����Ʈ ���� ���� �ĸ� ���ۿ� �׸��� ���Ͽ� ���� ȥ�� ���¸� ����Ѵ�.
			
			
			m_pd3dDeviceContext->OMSetBlendState(CBlendingResource::pAddBlending, blendFactors, 0xffffffff);
			m_pLightShaftRenderer->UpdateLightShaftPass2Variables(m_pd3dDeviceContext);
			
			m_pLightShaftRenderer->SetLightShaftPassShader(CShaderResource::pLightShaftPass2Shader);
			m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
			m_pLightShaftRenderer->Render(m_pd3dDeviceContext);
			m_pd3dDeviceContext->OMSetBlendState(CBlendingResource::pTransparentBlending, blendFactors, 0xffffffff);*/
		}

	}
	else                      // ������ �������� ���ٸ� �Ϲ����� �������� �����Ѵ�.
	{
		
		pMyPlayer->UpdateShaderVariables(m_pd3dDeviceContext);

		for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
		{
			if (m_pPlayersMgrInform->m_ppPlayers[i] && m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsActive)
			{
				m_pPlayersMgrInform->m_ppPlayers[i]->Render(m_pd3dDeviceContext);
			}
		}

		//if (m_ppPlayers[1]) m_ppPlayers[1]->Render(m_pd3dDeviceContext);
		
		if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, pCamera);
	}

	AnimateObjects();
	/*
	if (m_pUI)
	{
		if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		m_pUI->BackgroundUIRender(m_pd3dDeviceContext);
		m_pUI->PlayerHavingVoxelRender(m_pd3dDeviceContext, pMyPlayer);

		if (!pMyPlayer->m_bIsActive)
		{
			m_pUI->RespawningRender(m_pd3dDeviceContext, &m_pScene->m_RespawnManager, pMyPlayer);
		}
	}
	*/
	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer + 16, 37);
	//current_time += m_GameTimer.GetTimeElapsed();
	::SetWindowText(m_hWnd, m_pszBuffer);
}
