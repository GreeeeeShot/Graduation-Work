#include "stdafx.h"
#include "GameState.h"
#include "GameManager.h"
#include "FindingTreasure_Test_Blending.h"
#include "IOCP_Client.h"

HBITMAP CGameState::m_hMemBitmap = NULL;
HBITMAP CGameState::m_hIntroBackBitmap = NULL;
HBITMAP CGameState::m_hIntroCompanyBitmap = NULL;
HBITMAP CGameState::m_hTempWaitingRoomBitmap = NULL;
HBITMAP CGameState::m_hIPInputBitmap = NULL;
HBITMAP CGameState::m_hWaitingRoomBitmap = NULL;
HBITMAP CGameState::m_hTitleBitmap = NULL;
HBITMAP CGameState::m_hMapbowlBitmap = NULL;

HBITMAP CGameState::m_hLeftAndRightArrowBitmap = NULL;
HBITMAP CGameState::m_hReadyBitmap = NULL;
HBITMAP CGameState::m_hSelectedTeamFrameBitmap[3] = { 0, };
HBITMAP CGameState::m_hCharacterBitmap[2] = { 0, };
HBITMAP CGameState::m_hMapBitmap[2] = { 0, };

HINSTANCE CGameState::m_hInstance;
HWND CGameState::m_hwnd;

char server_ip[20];
char server_msg[20];
bool host;
int strn;
int g_map;
bool IsWaitingRoom;
bool ready;
bool g_GameStart;

CButton::CButton()
{
	m_fTop = 0.0f;
	m_fLeft = 0.0f;
	m_fRight = 0.0f;
	m_fBottom = 0.0f;
}
CButton::CButton(float fLeft ,float fTop, float fRight, float fBottom)
{
	m_fTop = fTop;
	m_fLeft = fLeft;
	m_fRight = fRight;
	m_fBottom = fBottom;
}

CButton::~CButton()
{

}

bool CButton::IsInButton(float fX, float fY) const
{
	if (m_fLeft > fX || fX > m_fRight) return false;
	if (m_fTop > fY || fY > m_fBottom) return false;

	return true;
}

CGameState::CGameState()
{
}


CGameState::~CGameState()
{
}

void CGameState::CreateAPIBitmapResource(HINSTANCE hInstance, HWND hwnd)
{
	m_hInstance = hInstance;
	m_hwnd = hwnd;
	
	m_hIntroBackBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_INTRO_BACK));
	m_hIntroCompanyBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_INTRO_COMPANY_LOGO));
	m_hTempWaitingRoomBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TEMP_WAITINGROOM));
	m_hTitleBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP12));
	m_hIPInputBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP6));
	m_hWaitingRoomBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP9));
	m_hCharacterBitmap[0] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP4));
	m_hCharacterBitmap[1] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
	m_hSelectedTeamFrameBitmap[0] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP10));
	m_hSelectedTeamFrameBitmap[1] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP11));
	m_hSelectedTeamFrameBitmap[2] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP13));
	m_hLeftAndRightArrowBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP8));
	m_hReadyBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP7));
	m_hMapBitmap[0] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP14));
	m_hMapBitmap[1] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP16));
	m_hMapbowlBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP15));
}

CPlayGameState::CPlayGameState()
{
	m_pWaitingRoomInformDesc = false;
}

CPlayGameState::~CPlayGameState()
{

}

void CPlayGameState::Init(void)
{
	CGameManager::GetInstance()->m_pGameFramework = new CGameFramework();
	CGameManager::GetInstance()->m_pGameFramework->OnCreate(*CGameManager::GetInstance()->m_pHinstance, *CGameManager::GetInstance()->m_pHwnd);
	CGameManager::GetInstance()->m_pGameFramework->m_pWaitingRoomInformDesc = m_pWaitingRoomInformDesc;
	CGameManager::GetInstance()->m_pGameFramework->BuildObjects();
}

void CPlayGameState::Destroy(void)
{
	CGameManager::GetInstance()->m_pGameFramework->OnDestroy();
	delete CGameManager::GetInstance()->m_pGameFramework;
	CGameManager::GetInstance()->m_pGameFramework = NULL;
}

void CPlayGameState::Update(void)
{
	////타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산한다. 
	//m_pFramework->m_GameTimer.Tick();

	//m_pFramework->ProcessInput();

	//m_pFramework->AnimateObjects();
}

void CPlayGameState::Render(void)
{
	if(CGameManager::GetInstance()->m_pGameFramework) CGameManager::GetInstance()->m_pGameFramework->FrameAdvance();
}

void CPlayGameState::HandOverExternalInput(void* pExternalInput)
{
	m_pWaitingRoomInformDesc = (SWaitingRoomInformDesc*)pExternalInput;
}

void CPlayGameState::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case 'b': case 'B':
			CGameManager::GetInstance()->ChangeGameState(new CTempWaitingRoomState());//new CPlayGameState());
			break;
		//case 'c': case 'C':
		//	CTextureResource::DestroyTextureResource();
		//	//CShaderResource::DestroyShaderResource();
		//	//CMeshResource::DestroyMeshResource();			// 메시 리소스를 생성하기 이전에 셰이더 리소스와 FBX 리소스가 마련되어야 한다.
		//	//CMaterialResource::DestroyMaterialResource();
		//	//CBlendingResource::DestroyBlendingResource();
		//	break;
		}
		break;
	}
}

LRESULT CALLBACK CPlayGameState::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case 'b': case 'B':
		//case 'c': case 'C':
		//case 'v': case 'V':
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
		}
		break;
	}

	if (CGameManager::GetInstance()->m_pGameFramework) CGameManager::GetInstance()->m_pGameFramework->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
	return 0;
}

CIntroState::CIntroState()
{

}

CIntroState::~CIntroState()
{

}

void CIntroState::Init(void)
{
	m_endClock = m_startClock = clock();

	m_BlendFunction.BlendOp = AC_SRC_OVER;
	m_BlendFunction.BlendFlags = 0;
	m_BlendFunction.AlphaFormat = 0;

	m_BlendFunction.SourceConstantAlpha = 0;
}

void CIntroState::Destroy(void)
{
	//CGameManager::GetInstance()->m_pGameFramework->ReleaseObjects();
}

void CIntroState::Update(void)
{
	m_endClock = clock();
	float fTimeElapsed = ((float)m_endClock - (float)m_startClock) / 1000.0f;

	
	if (fTimeElapsed <= 2.0f)
	{
		m_iAlpha = 0 * (1.0f - (fTimeElapsed - 0.0f) / 2.0f) +  255 * (fTimeElapsed - 0.0f) / 2.0f;
		//printf("m_iAlpha : %d \n", m_iAlpha);
	}
	else if (4.0f <= fTimeElapsed && fTimeElapsed <= 6.0f)
	{
		m_iAlpha = 255 * (1.0f - (fTimeElapsed - 4.0f) / 2.0f) + 0 * (fTimeElapsed - 4.0f) / 2.0f;
		//printf("m_iAlpha : %d \n", m_iAlpha);
	}
	
	else if (fTimeElapsed > 4.0f)
	{
		CGameManager::GetInstance()->ChangeGameState(new CTitleState());//new CPlayGameState());

	}

	InvalidateRgn(m_hwnd, NULL, FALSE);
}

void CIntroState::Render(void)
{

}

void CIntroState::Render(HDC hdc)
{
	// 아래의 if문은 hdc를 인자로 받는 Render 메소드에서 반드시 넣어주어야 합니다.
	if (CGameState::m_hMemBitmap == NULL)
	{
		CGameState::m_hMemBitmap = CreateCompatibleBitmap(hdc, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	}

	/* 기본 메모리 디바이스 컨텍스트에 기본 비트맵 정보를 설정한다. */
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP oldMemBitmap = (HBITMAP)SelectObject(hMemDC, CGameState::m_hMemBitmap);

	HDC hBackDC = CreateCompatibleDC(hdc);
	HBITMAP oldBackBitmap = (HBITMAP)SelectObject(hBackDC, CGameState::m_hIntroBackBitmap);
	//BitBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hBackDC, 0, 0, SRCCOPY);
	HDC hCompanyDC = CreateCompatibleDC(hdc);
	HBITMAP oldCompanyBitmap = (HBITMAP)SelectObject(hCompanyDC, CGameState::m_hIntroCompanyBitmap);

	TransparentBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hBackDC, 0, 0, 128, 72, RGB(0, 0, 0));
	//TransparentBlt(hMemDC, 325, 182, 630, 355, hCompanyDC, 0, 0, 630, 355, RGB(0, 0, 0));

	printf("m_iAlpha : %d \n", m_iAlpha);
	m_BlendFunction.SourceConstantAlpha = m_iAlpha;

	AlphaBlend(hMemDC, (FRAME_BUFFER_WIDTH - 630) / 2, (FRAME_BUFFER_HEIGHT - 355) / 2, 630, 355, hCompanyDC, 0, 0, 630, 355, m_BlendFunction);
	
	SelectObject(hBackDC, oldBackBitmap);
	DeleteDC(hBackDC);
	SelectObject(hCompanyDC, oldCompanyBitmap);
	DeleteDC(hCompanyDC);

	/* 화면에 출력한다. */
	BitBlt(hdc, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, oldMemBitmap);
	DeleteDC(hMemDC);
}

//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
void CIntroState::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CIntroState::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

}

LRESULT CALLBACK CIntroState::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

CTitleState::CTitleState() {}
CTitleState::~CTitleState() {}
void CTitleState::Init(void) {}
void CTitleState::Destroy(void) {}
void CTitleState::Update(void) {}
void CTitleState::Render(void) {}
void CTitleState::Render(HDC hdc)
{
	// 아래의 if문은 hdc를 인자로 받는 Render 메소드에서 반드시 넣어주어야 합니다.
	if (CGameState::m_hMemBitmap == NULL)
	{
		CGameState::m_hMemBitmap = CreateCompatibleBitmap(hdc, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	}

	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP oldMemBitmap = (HBITMAP)SelectObject(hMemDC, CGameState::m_hMemBitmap);

	HDC hTitleDC = CreateCompatibleDC(hdc);
	HBITMAP oldTitleBitmap = (HBITMAP)SelectObject(hTitleDC, CGameState::m_hTitleBitmap);

	BitBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hTitleDC, 0, 0, SRCCOPY);

	SelectObject(hTitleDC, oldTitleBitmap);
	DeleteDC(hTitleDC);


	/* 화면에 출력한다. */
	BitBlt(hdc, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, oldMemBitmap);
	DeleteDC(hMemDC);
}

//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
void CTitleState::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	static bool bIsClickedButtonMakingWaitingRoom = false;			// DOWN 버튼 시 버튼을 눌렀는지 확인함. UP은 아님!
	static bool bIsClickedButtonJoinInWaitingRoom = false;			// ..
	static bool bIsClickedButtonOption = false;						// ..
	static bool bIsClickedButtonExit = false;						// ..

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		if (m_sButtonMakingWaitingRoom.IsInButton(LOWORD(lParam), HIWORD(lParam))) bIsClickedButtonMakingWaitingRoom = true;		// 일단 해당 버튼이 눌림을 확인함.
		else if (m_sButtonJoinInWaitingRoom.IsInButton(LOWORD(lParam), HIWORD(lParam))) bIsClickedButtonJoinInWaitingRoom = true;
		else if (m_sButtonOption.IsInButton(LOWORD(lParam), HIWORD(lParam)))  bIsClickedButtonOption = true;
		else if (m_sButtonExit.IsInButton(LOWORD(lParam), HIWORD(lParam))) bIsClickedButtonExit = true;
		break;
	case WM_LBUTTONUP:
		if (m_sButtonMakingWaitingRoom.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			ServerMain();
			if (bIsClickedButtonMakingWaitingRoom) CGameManager::GetInstance()->ChangeGameState(new CWaitingRoomState());                        
		}
		else if (m_sButtonJoinInWaitingRoom.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			if (bIsClickedButtonJoinInWaitingRoom) CGameManager::GetInstance()->ChangeGameState(new CIPInputState());
		}
		else if (m_sButtonOption.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			//if (bIsClickedButtonOption) CGameManager::GetInstance()->ChangeGameState(NULL);
		}
		else if (m_sButtonExit.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			if (bIsClickedButtonExit) exit(-1);
		}
		bIsClickedButtonMakingWaitingRoom = false;
		bIsClickedButtonJoinInWaitingRoom = false;
		bIsClickedButtonOption = false;
		bIsClickedButtonExit = false;
		break;
	}
}
//void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CTitleState::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return 0;
}

CTempWaitingRoomState::CTempWaitingRoomState() 
{
	m_pWaitingRoomInformDesc = NULL; ;
}

CTempWaitingRoomState::~CTempWaitingRoomState() {}

void CTempWaitingRoomState::Init()
{
	m_pWaitingRoomInformDesc = new SWaitingRoomInformDesc();

	for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)
	{
		m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_bIsSlotActive = false;
	}	
}
void CTempWaitingRoomState::Destroy(void) {}
void CTempWaitingRoomState::Update(void) 
{
	InvalidateRgn(m_hwnd, NULL, FALSE);
}

void CTempWaitingRoomState::Render(void) {}
void CTempWaitingRoomState::Render(HDC hdc)
{
	// 아래의 if문은 hdc를 인자로 받는 Render 메소드에서 반드시 넣어주어야 합니다.
	if (CGameState::m_hMemBitmap == NULL)
	{
		CGameState::m_hMemBitmap = CreateCompatibleBitmap(hdc, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	}

	/* 기본 메모리 디바이스 컨텍스트에 기본 비트맵 정보를 설정한다. */
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP oldMemBitmap = (HBITMAP)SelectObject(hMemDC, CGameState::m_hMemBitmap);

	HDC hTempWaitingRoomDC = CreateCompatibleDC(hdc);
	HBITMAP oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, CGameState::m_hTempWaitingRoomBitmap);

	//TransparentBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hBackDC, 0, 0, 128, 72, RGB(0, 0, 0));			
	//TransparentBlt(hMemDC, 325, 182, 630, 355, hCompanyDC, 0, 0, 630, 355, RGB(0, 0, 0));

	//if (state == server_input) {

	//	oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, hipInputbmp);
	//	BitBlt(hMemDC, 0, 0, 1280, 760, hTempWaitingRoomDC, 0, 0, SRCCOPY);

	//	SelectObject(hTempWaitingRoomDC, oldTempWaitingRoomBitmap);

	//	BitBlt(hdc, 0, 0, 1280, 760, hMemDC, 0, 0, SRCCOPY);
	//	SelectObject(hMemDC, oldMemBitmap);

	//	TextOutA(hdc, 190, 350, "server ip input: ", strlen("server ip input: "));
	//	TextOutA(hdc, 300, 350, server_ip, strn);
	//}

	//BitBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hTempWaitingRoomDC, 0, 0, SRCCOPY);

	////SelectObject(hBackDC, oldBackBitmap);
	////DeleteDC(hBackDC);
	//SelectObject(hTempWaitingRoomDC, oldTempWaitingRoomBitmap);
	//DeleteDC(hTempWaitingRoomDC);

	///* 화면에 출력한다. */
	//BitBlt(hdc, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hMemDC, 0, 0, SRCCOPY);

	//SelectObject(hMemDC, oldMemBitmap);
	//DeleteDC(hMemDC);
}

CWaitingRoomState::CWaitingRoomState() 
{
	m_pWaitingRoomInformDesc = NULL;
}
CWaitingRoomState::~CWaitingRoomState() {}
void CWaitingRoomState::Init(void) 
{
	m_pWaitingRoomInformDesc = new SWaitingRoomInformDesc();

	for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)
	{
		m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_bIsSlotActive = false;
	}
}
void CWaitingRoomState::Destroy(void) {}
void CWaitingRoomState::Update(void) 
{
	
	if (g_GameStart)
	{
		m_pWaitingRoomInformDesc->m_iMyPlayerID = my_id;            // 이건 준상이 알아서
		m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_bIsSlotActive = true;
		m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_PlayerType = (PLAYER_TYPE)charac;
		m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_BelongType = (BELONG_TYPE)my_team;
		m_pWaitingRoomInformDesc->m_eSceneType = (SCENE_TYPE)g_map;
		for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)         // 이건 준상이 알아서
		{
			if (!waitingplayer[i].connect)
				continue;
			if (!(waitingplayer[i].id >= 0 && waitingplayer[i].id < 8))
				continue;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_bIsSlotActive = true;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_PlayerType = (PLAYER_TYPE)waitingplayer[i].charac;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_BelongType = (BELONG_TYPE)waitingplayer[i].team;
		}
		g_GameStart = false;
		CGameState* pTempGameState = new CPlayGameState();
		pTempGameState->HandOverExternalInput(m_pWaitingRoomInformDesc);         // 외부 정보를 등록 시 다음과 같이 진행해야만 한다.
		CGameManager::GetInstance()->ChangeGameState(pTempGameState);
	}
}
void CWaitingRoomState::Render(void) {}
void CWaitingRoomState::Render(HDC hdc) 
{
	// 아래의 if문은 hdc를 인자로 받는 Render 메소드에서 반드시 넣어주어야 합니다.
	if (CGameState::m_hMemBitmap == NULL)
	{
		CGameState::m_hMemBitmap = CreateCompatibleBitmap(hdc, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	}

	/* 기본 메모리 디바이스 컨텍스트에 기본 비트맵 정보를 설정한다. */
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP oldMemBitmap = (HBITMAP)SelectObject(hMemDC, CGameState::m_hMemBitmap);

	
	// 대기방 배경 출력
	HDC hWaitingRoomDC = CreateCompatibleDC(hdc);
	HBITMAP oldWaitingRoomBitmap = (HBITMAP)SelectObject(hWaitingRoomDC, m_hWaitingRoomBitmap);

	BitBlt(hMemDC, 0, 0, 1280, 760, hWaitingRoomDC, 0, 0, SRCCOPY);

	SelectObject(hWaitingRoomDC, oldWaitingRoomBitmap);
	DeleteDC(hWaitingRoomDC);

	/////////////////자신의 팀컬러 및 캐릭터, 화살표, 레디//////////////
	HDC hSelectedTeamFrameDC = CreateCompatibleDC(hdc);
	HBITMAP oldSelectedTeamFrameBitmap = (HBITMAP)SelectObject(hSelectedTeamFrameDC, m_hSelectedTeamFrameBitmap[my_team]);
	
	GdiTransparentBlt(hMemDC, 10, 40, 374, 418, hSelectedTeamFrameDC, 0, 0, 425, 475, RGB(255, 255, 255));

	SelectObject(hSelectedTeamFrameDC, oldSelectedTeamFrameBitmap);
	DeleteDC(hSelectedTeamFrameDC);


	HDC hCharacterDC = CreateCompatibleDC(hdc);
	HBITMAP oldCharacterBitmap = (HBITMAP)SelectObject(hCharacterDC, m_hCharacterBitmap[charac]);

	GdiTransparentBlt(hMemDC, 55, 195, 258, 193, hCharacterDC, 0, 0, 640, 480, RGB(255, 255, 255));

	SelectObject(hCharacterDC, oldCharacterBitmap);
	DeleteDC(hCharacterDC);


	HDC hLeftAndRightArrowDC = CreateCompatibleDC(hdc);
	HBITMAP oldLeftAndRightArrowBitmap = (HBITMAP)SelectObject(hLeftAndRightArrowDC, m_hLeftAndRightArrowBitmap);

	GdiTransparentBlt(hMemDC, 10, 200, 360, 120, hLeftAndRightArrowDC, 0, 0, 411, 142, RGB(255, 255, 255));

	SelectObject(hLeftAndRightArrowDC, oldLeftAndRightArrowBitmap);
	DeleteDC(hLeftAndRightArrowDC);

	if (ready) {
		HDC hReadyDC = CreateCompatibleDC(hdc);
		HBITMAP oldReadyBitmap = (HBITMAP)SelectObject(hReadyDC, m_hReadyBitmap);

		GdiTransparentBlt(hMemDC, 80, 270, 227, 142, hReadyDC, 0, 0, 227, 142, RGB(255, 255, 255));

		SelectObject(hReadyDC, oldReadyBitmap);
		DeleteDC(hReadyDC);
	}
	/////////////////팀컬러////////////////////////////////

	for (int i = 0; i < 7; i++)
	{
		hSelectedTeamFrameDC = CreateCompatibleDC(hdc);
		oldSelectedTeamFrameBitmap = (HBITMAP)SelectObject(hSelectedTeamFrameDC, m_hSelectedTeamFrameBitmap[waitingplayer[i].team]);

		GdiTransparentBlt(hMemDC, m_sStartTeamPos[i].x, m_sStartTeamPos[i].y, 170, 190, hSelectedTeamFrameDC, 0, 0, 425, 475, RGB(255, 255, 255));

		SelectObject(hSelectedTeamFrameDC, oldSelectedTeamFrameBitmap);
		DeleteDC(hSelectedTeamFrameDC);

		if (waitingplayer[i].connect)
		{
			hCharacterDC = CreateCompatibleDC(hdc);
			oldCharacterBitmap = (HBITMAP)SelectObject(hCharacterDC, m_hCharacterBitmap[waitingplayer[i].charac]);

			GdiTransparentBlt(hMemDC, m_sStartCharaterPos[i].x, m_sStartCharaterPos[i].y, 120, 90, hCharacterDC, 0, 0, 640, 480, RGB(255, 255, 255));

			SelectObject(hCharacterDC, oldCharacterBitmap);
			DeleteDC(hCharacterDC);

			if (waitingplayer[i].ready)
			{
				HDC hReadyDC = CreateCompatibleDC(hdc);
				HBITMAP oldReadyBitmap = (HBITMAP)SelectObject(hReadyDC, m_hReadyBitmap);

				GdiTransparentBlt(hMemDC, m_sReadyPos[i].x, m_sReadyPos[i].y, 113, 71, hReadyDC, 0, 0, 227, 142, RGB(255, 255, 255));

				SelectObject(hReadyDC, oldReadyBitmap);
				DeleteDC(hReadyDC);
			}
		}
		else
		{
			hCharacterDC = CreateCompatibleDC(hdc);
			oldCharacterBitmap = (HBITMAP)SelectObject(hCharacterDC, m_hCharacterBitmap[2]);

			GdiTransparentBlt(hMemDC, m_sStartCharaterPos[i].x, m_sStartCharaterPos[i].y, 120, 90, hCharacterDC, 0, 0, 640, 480, RGB(255, 255, 255));

			SelectObject(hCharacterDC, oldCharacterBitmap);
			DeleteDC(hCharacterDC);
		}
	}

	/////////////////맵////////////////////////

	HDC hMapDC = CreateCompatibleDC(hdc);
	HBITMAP oldMapBitmap = (HBITMAP)SelectObject(hMapDC, m_hMapbowlBitmap);

	GdiTransparentBlt(hMemDC, m_sMapPos[0].x , m_sMapPos[0].y, 350, 350, hMapDC, 0, 0, 425, 425, RGB(255, 255, 255));

	SelectObject(hMapDC, oldCharacterBitmap);
	DeleteDC(hMapDC);

	hMapDC = CreateCompatibleDC(hdc);

	oldMapBitmap = (HBITMAP)SelectObject(hMapDC, m_hMapBitmap[g_map]);

	GdiTransparentBlt(hMemDC, m_sMapPos[1].x, m_sMapPos[1].y, 250, 290, hMapDC, 0, 0, 295, 371, RGB(255, 255, 255));

	SelectObject(hMapDC, oldCharacterBitmap);
	DeleteDC(hMapDC);


	BitBlt(hdc, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, oldMemBitmap);
	DeleteObject(hMemDC);
}

//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
void CWaitingRoomState::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	//case WM_LBUTTONDOWN:
	//	if (!wParam) break;
	//	switch (wParam)
	//	{
	//	case VK_RETURN:
	//		//state = waitingroom;
	//		IsWaitingRoom = true;
	//		//InvalidateRgn(hWnd, NULL, FALSE);
	//		break;
	//	case VK_BACK:
	//		if (strn>0)
	//			strn--;
	//		break;
	//	case VK_OEM_PERIOD:
	//		server_ip[strn++] = '.';
	//		server_ip[strn] = '\0';
	//		break;
	//	default:
	//		server_ip[strn++] = wParam;
	//		server_ip[strn] = '\0';
	//		break;
	//	}
	//	InvalidateRgn(hWnd, NULL, FALSE);
	//	break;
	case WM_LBUTTONUP:
		if (m_sButtonLeftArrow.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			WaitingPacket(CHARACHANGE);
			charac = (charac + 1) % 2;
		}
		else if (m_sButtonRightArrow.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			WaitingPacket(CHARACHANGE);
			charac = (charac + 1) % 2;
		}
		else if (m_sButtonTeamFrame.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			WaitingPacket(TEAMCHANGE);
			my_team = (my_team + 1) % 2;
		}
		else if (m_sButtonReadyAndStart.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			WaitingPacket(READY);
			ready = (ready + 1) % 2;

			//
		}
		else if (m_sButtonMap.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			if (my_id == 0)
			{
				WaitingPacket(MAPCHANGE);
				g_map = (g_map + 1) % 2;
			}
		}
		else if (m_sButtonFakie.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			WaitingPacket(EXIT);
			CGameManager::GetInstance()->ChangeGameState(new CTitleState());
		}
		break;
	}
}

LRESULT CALLBACK CWaitingRoomState::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return 0;
}

CIPInputState::CIPInputState() {}
CIPInputState::~CIPInputState() {}

void CIPInputState::Init() {}
void CIPInputState::Destroy(void) {}
void CIPInputState::Update(void) 
{
	InvalidateRgn(m_hwnd, NULL, FALSE);
}
void CIPInputState::Render(void) {}
void CIPInputState::Render(HDC hdc) 
{
	// 아래의 if문은 hdc를 인자로 받는 Render 메소드에서 반드시 넣어주어야 합니다.
	if (CGameState::m_hMemBitmap == NULL)
	{
		CGameState::m_hMemBitmap = CreateCompatibleBitmap(hdc, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	}

	/* 기본 메모리 디바이스 컨텍스트에 기본 비트맵 정보를 설정한다. */
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP oldMemBitmap = (HBITMAP)SelectObject(hMemDC, CGameState::m_hMemBitmap);

	HDC hIPInputDC = CreateCompatibleDC(hdc);
	HBITMAP oldIPInputBitmap = (HBITMAP)SelectObject(hIPInputDC, m_hIPInputBitmap);

	BitBlt(hMemDC, 0, 0, 1280, 760, hIPInputDC, 0, 0, SRCCOPY);

	SelectObject(hIPInputDC, oldIPInputBitmap);
	DeleteDC(hIPInputDC);

	TextOutA(hMemDC, 190, 350, "server ip input: ", strlen("server ip input: "));
	TextOutA(hMemDC, 300, 350, server_ip, strn);
	
	BitBlt(hdc, 0, 0, 1280, 760, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, oldMemBitmap);
	DeleteDC(hMemDC);
}

////윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
void CIPInputState::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONUP:
		if (m_sButtonFakie.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			CGameManager::GetInstance()->ChangeGameState(new CTitleState());
		}
		break;
	}
}

void CIPInputState::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_RETURN:
			//state = waitingroom;
			//IsWaitingRoom = true;
			//InvalidateRgn(hWnd, NULL, FALSE);
			ClientMain(hWnd, server_ip);
			CGameManager::GetInstance()->ChangeGameState(new CWaitingRoomState());
			break;
		case VK_BACK:
			if (strn>0)
				strn--;
			break;
		case VK_OEM_PERIOD:
			server_ip[strn++] = '.';
			server_ip[strn] = '\0';
			break;
		default:
			server_ip[strn++] = wParam;
			server_ip[strn] = '\0';
			break;
		}
		break;
	}
	InvalidateRgn(hWnd, NULL, FALSE);
}
LRESULT CALLBACK CIPInputState::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return 0;
}
