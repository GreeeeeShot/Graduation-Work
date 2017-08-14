#include "stdafx.h"
#include "GameState.h"
#include "GameManager.h"
#include "FindingTreasure_Test_Blending.h"
#include "IOCP_Client.h"

HBITMAP CGameState::m_hMemBitmap = NULL;
HBITMAP CGameState::m_hIntroBackBitmap = NULL;
HBITMAP CGameState::m_hIntroCompanyBitmap = NULL;
HBITMAP CGameState::m_hTempWaitingRoomBitmap = NULL;
HBITMAP CGameState::hipInputbmp = NULL;
HBITMAP CGameState::hwaitingbmp = NULL;
HBITMAP CGameState::htitle = NULL;
/*
HBITMAP CGameState::piratebmp = NULL;
HBITMAP CGameState::cowgirlcharabmp = NULL;
HBITMAP CGameState::blueTeambmp = NULL;
HBITMAP CGameState::redTeambmp = NULL;
HBITMAP CGameState::nonteam = NULL;
*/

HBITMAP CGameState::charaSelectbmp = NULL;
HBITMAP CGameState::readybmp = NULL;
HBITMAP CGameState::SelectTeambmp[3] = { 0, };
HBITMAP CGameState::charabmp[2] = { 0, };

HINSTANCE CGameState::m_hInstance;
HWND CGameState::m_hwnd;

char server_ip[20];
char server_msg[20];
bool host;
int strn;
bool IsWaitingRoom;
bool ready;

CButton::CButton()
{
	m_fTop = 0.0f;
	m_fLeft = 0.0f;
	m_fRight = 0.0f;
	m_fBottom = 0.0f;
}
CButton::CButton(float fTop, float fLeft, float fRight, float fBottom)
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
	/*
	CGameState::piratebmp = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP4));
	CGameState::cowgirlcharabmp = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
	CGameState::blueTeambmp = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP10));
	CGameState::redTeambmp = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP11));
	CGameState::nonteam = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP10));
	*/
	
	CGameState::m_hIntroBackBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_INTRO_BACK));
	CGameState::m_hIntroCompanyBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_INTRO_COMPANY_LOGO));
	CGameState::m_hTempWaitingRoomBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TEMP_WAITINGROOM));
	CGameState::htitle = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP12));
	CGameState::hipInputbmp = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP6));
	CGameState::hwaitingbmp = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP9));
	CGameState::charabmp[0] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP4));
	CGameState::charabmp[1] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
	CGameState::SelectTeambmp[0] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP10));
	CGameState::SelectTeambmp[1] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP11));
	CGameState::SelectTeambmp[2] = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP10));
	CGameState::charaSelectbmp = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP8));
	CGameState::readybmp = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP7));

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
		CGameManager::GetInstance()->ChangeGameState(new CTempWaitingRoomState());//new CPlayGameState());

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


CTempWaitingRoomState::CTempWaitingRoomState() 
{
	m_pWaitingRoomInformDesc = NULL; ;

	
}

CTempWaitingRoomState::~CTempWaitingRoomState() {}

void CTempWaitingRoomState::Init()
{
	m_pWaitingRoomInformDesc = new SWaitingRoomInformDesc();
	/*
	m_pWaitingRoomInformDesc->m_iMyPlayerID = my_id;

	m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_bIsSlotActive = true;
	m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_BelongType = (BELONG_TYPE)my_team;
	m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_PlayerType = (PLAYER_TYPE)charac;

	m_pWaitingRoomInformDesc->m_eSceneType = SCENE_TYPE_JUNGLE;
	
	for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)			// 이건 준상이 알아서
	{
		if (!waitingplayer[i].connect)
			continue;
		if (waitingplayer[i].id > 7 || waitingplayer[i].id < 0)
			continue;
		else
		{
			printf("waitingplayer[i].id : %d\n", waitingplayer[i].id);
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_bIsSlotActive = true;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_PlayerType = (PLAYER_TYPE)waitingplayer[i].charac;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_BelongType = waitingplayer[i].team;
		}
		
	}*/

	for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)
	{
		m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_bIsSlotActive = false;
	}
	/*
	CGameState* pTempGameState = new CPlayGameState();
	pTempGameState->HandOverExternalInput(m_pWaitingRoomInformDesc);			// 외부 정보를 등록 시 다음과 같이 진행해야만 한다.
	CGameManager::GetInstance()->ChangeGameState(pTempGameState);								// 게임 플레이 상태로 바꾼다.
	*/
	
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

	//HDC hBackDC = CreateCompatibleDC(hdc);
	//HBITMAP oldBackBitmap = (HBITMAP)SelectObject(hBackDC, CGameState::m_hIntroBackBitmap);
	//BitBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hBackDC, 0, 0, SRCCOPY);
	HDC hTempWaitingRoomDC = CreateCompatibleDC(hdc);
	HBITMAP oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, CGameState::m_hTempWaitingRoomBitmap);

	//TransparentBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hBackDC, 0, 0, 128, 72, RGB(0, 0, 0));			
	//TransparentBlt(hMemDC, 325, 182, 630, 355, hCompanyDC, 0, 0, 630, 355, RGB(0, 0, 0));

	if (state == server_input) {

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, hipInputbmp);
		BitBlt(hMemDC, 0, 0, 1280, 760, hTempWaitingRoomDC, 0, 0, SRCCOPY);

		SelectObject(hTempWaitingRoomDC, oldTempWaitingRoomBitmap);

		BitBlt(hdc, 0, 0, 1280, 760, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, oldMemBitmap);

		TextOutA(hdc, 190, 350, "server ip input: ", strlen("server ip input: "));
		TextOutA(hdc, 300, 350, server_ip, strn);
	}
	else if (state == host_select)
	{
		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, CGameState::htitle);
	}
	else if (state == waitingroom)
	{
		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, hwaitingbmp);

		BitBlt(hMemDC, 0, 0, 1280, 760, hTempWaitingRoomDC, 0, 0, SRCCOPY);
		SelectObject(hTempWaitingRoomDC, oldTempWaitingRoomBitmap);

		/////////////////자신의 팀컬러 및 캐릭터, 화살표, 레디//////////////
		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, SelectTeambmp[my_team]);
		GdiTransparentBlt(hMemDC, 10, 40, 374, 418, hTempWaitingRoomDC, 0, 0, 425, 475, RGB(255, 255, 255));

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, charabmp[charac]);
		GdiTransparentBlt(hMemDC, 55, 195, 258, 193, hTempWaitingRoomDC, 0, 0, 640, 480, RGB(255, 255, 255));

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, charaSelectbmp);
		GdiTransparentBlt(hMemDC, 10, 200, 360, 120, hTempWaitingRoomDC, 0, 0, 411, 142, RGB(255, 255, 255));

		if (ready) {
			oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, readybmp);
			GdiTransparentBlt(hMemDC, 80, 270, 227, 142, hTempWaitingRoomDC, 0, 0, 227, 142, RGB(255, 255, 255));
		}
		/////////////////팀컬러////////////////////////////////

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, SelectTeambmp[waitingplayer[0].team]);
		GdiTransparentBlt(hMemDC, 380, 70, 170, 190, hTempWaitingRoomDC, 0, 0, 425, 475, RGB(255, 255, 255));
		if (waitingplayer[0].connect)
		{
			oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, charabmp[waitingplayer[0].charac]);
			GdiTransparentBlt(hMemDC, 399, 140, 120, 90, hTempWaitingRoomDC, 0, 0, 640, 480, RGB(255, 255, 255));
			if (waitingplayer[0].ready)
			{
				oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, readybmp);
				GdiTransparentBlt(hMemDC, 405, 175, 113, 71, hTempWaitingRoomDC, 0, 0, 227, 142, RGB(255, 255, 255));
			}
		}

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, SelectTeambmp[waitingplayer[1].team]);
		GdiTransparentBlt(hMemDC, 550, 70, 170, 190, hTempWaitingRoomDC, 0, 0, 425, 475, RGB(255, 255, 255));
		if (waitingplayer[1].connect)
		{
			oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, charabmp[waitingplayer[1].charac]);
			GdiTransparentBlt(hMemDC, 569, 140, 120, 90, hTempWaitingRoomDC, 0, 0, 640, 480, RGB(255, 255, 255));
			if (waitingplayer[1].ready)
			{
				oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, readybmp);
				GdiTransparentBlt(hMemDC, 575, 175, 113, 71, hTempWaitingRoomDC, 0, 0, 227, 142, RGB(255, 255, 255));
			}
		}

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, SelectTeambmp[waitingplayer[2].team]);
		GdiTransparentBlt(hMemDC, 720, 70, 170, 190, hTempWaitingRoomDC, 0, 0, 425, 475, RGB(255, 255, 255));
		if (waitingplayer[2].connect)
		{
			oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, charabmp[waitingplayer[2].charac]);
			GdiTransparentBlt(hMemDC, 739, 140, 120, 90, hTempWaitingRoomDC, 0, 0, 640, 480, RGB(255, 255, 255));
			if (waitingplayer[2].ready)
			{
				oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, readybmp);
				GdiTransparentBlt(hMemDC, 745, 175, 113, 71, hTempWaitingRoomDC, 0, 0, 227, 142, RGB(255, 255, 255));
			}
		}

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, SelectTeambmp[waitingplayer[3].team]);
		GdiTransparentBlt(hMemDC, 340, 300, 170, 190, hTempWaitingRoomDC, 0, 0, 425, 475, RGB(255, 255, 255));
		if (waitingplayer[3].connect)
		{
			oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, charabmp[waitingplayer[3].charac]);
			GdiTransparentBlt(hMemDC, 359, 370, 120, 90, hTempWaitingRoomDC, 0, 0, 640, 480, RGB(255, 255, 255));
			if (waitingplayer[3].ready)
			{
				oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, readybmp);
				GdiTransparentBlt(hMemDC, 365, 405, 113, 71, hTempWaitingRoomDC, 0, 0, 227, 142, RGB(255, 255, 255));
			}
		}

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, SelectTeambmp[waitingplayer[4].team]);
		GdiTransparentBlt(hMemDC, 480, 300, 170, 190, hTempWaitingRoomDC, 0, 0, 425, 475, RGB(255, 255, 255));
		if (waitingplayer[4].connect)
		{
			oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, charabmp[waitingplayer[4].charac]);
			GdiTransparentBlt(hMemDC, 499, 370, 120, 90, hTempWaitingRoomDC, 0, 0, 640, 480, RGB(255, 255, 255));
			if (waitingplayer[4].ready)
			{
				oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, readybmp);
				GdiTransparentBlt(hMemDC, 505, 405, 113, 71, hTempWaitingRoomDC, 0, 0, 227, 142, RGB(255, 255, 255));
			}
		}

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, SelectTeambmp[waitingplayer[5].team]);
		GdiTransparentBlt(hMemDC, 620, 300, 170, 190, hTempWaitingRoomDC, 0, 0, 425, 475, RGB(255, 255, 255));
		if (waitingplayer[5].connect)
		{
			oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, charabmp[waitingplayer[5].charac]);
			GdiTransparentBlt(hMemDC, 639, 370, 120, 90, hTempWaitingRoomDC, 0, 0, 640, 480, RGB(255, 255, 255));
			if (waitingplayer[5].ready)
			{
				oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, readybmp);
				GdiTransparentBlt(hMemDC, 645, 405, 113, 71, hTempWaitingRoomDC, 0, 0, 227, 142, RGB(255, 255, 255));
			}
		}

		oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, SelectTeambmp[waitingplayer[6].team]);
		GdiTransparentBlt(hMemDC, 760, 300, 170, 190, hTempWaitingRoomDC, 0, 0, 425, 475, RGB(255, 255, 255));
		if (waitingplayer[6].connect)
		{
			oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, charabmp[waitingplayer[6].charac]);
			GdiTransparentBlt(hMemDC, 779, 370, 120, 90, hTempWaitingRoomDC, 0, 0, 640, 480, RGB(255, 255, 255));
			if (waitingplayer[6].ready)
			{
				oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, readybmp);
				GdiTransparentBlt(hMemDC, 785, 405, 113, 71, hTempWaitingRoomDC, 0, 0, 227, 142, RGB(255, 255, 255));
			}
		}

		BitBlt(hdc, 0, 0, 1280, 760, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, oldMemBitmap);
	}
	else if (state == play)
	{
		m_pWaitingRoomInformDesc->m_iMyPlayerID = my_id;				// 이건 준상이 알아서
		m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_bIsSlotActive = true;
		m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_PlayerType = (PLAYER_TYPE)charac;
		m_pWaitingRoomInformDesc->m_PlayerInformDesc[my_id].m_BelongType = (BELONG_TYPE)my_team;
		m_pWaitingRoomInformDesc->m_eSceneType = SCENE_TYPE_JUNGLE;
		for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)			// 이건 준상이 알아서
		{
			if (!waitingplayer[i].connect)
				continue;
			if (!(waitingplayer[i].id >= 0 && waitingplayer[i].id < 8))
				continue;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_bIsSlotActive = true;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_PlayerType = (PLAYER_TYPE)waitingplayer[i].charac;
			m_pWaitingRoomInformDesc->m_PlayerInformDesc[waitingplayer[i].id].m_BelongType = (BELONG_TYPE)waitingplayer[i].team;
		}

		CGameState* pTempGameState = new CPlayGameState();
		pTempGameState->HandOverExternalInput(m_pWaitingRoomInformDesc);			// 외부 정보를 등록 시 다음과 같이 진행해야만 한다.
		CGameManager::GetInstance()->ChangeGameState(pTempGameState);
	}

	BitBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hTempWaitingRoomDC, 0, 0, SRCCOPY);

	//SelectObject(hBackDC, oldBackBitmap);
	//DeleteDC(hBackDC);
	SelectObject(hTempWaitingRoomDC, oldTempWaitingRoomBitmap);
	DeleteDC(hTempWaitingRoomDC);

	/* 화면에 출력한다. */
	BitBlt(hdc, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, oldMemBitmap);
	DeleteDC(hMemDC);
}

void CTempWaitingRoomState::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	int mx, my;

	static LONG x;
	static LONG y;

	static bool bIsClickedJungleButton = false;				// DOWN 버튼 시 버튼을 눌렀는지 확인함. UP은 아님!
	static bool bIsClickedSnowButton = false;				// ..

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		if (m_sJungleButton.IsInButton(LOWORD(lParam), HIWORD(lParam))) bIsClickedJungleButton = true;		// 일단 해당 버튼이 눌림을 확인함.
		else if (m_sSnowButton.IsInButton(LOWORD(lParam), HIWORD(lParam))) bIsClickedSnowButton = true;
		break;
	case WM_LBUTTONUP:			// 마우스 클릭 후 마우스 커서가 아까 눌렀던 버튼 위에 있다면 게임 플레이 상태로 전환함.
		if (state == host_select)
		{
			mx = LOWORD(lParam);
			my = HIWORD(lParam);

			if (mx > 468 && mx < 750 && my>367 && my < 438)
			{
				host = true;
				IsWaitingRoom = true;
				state = waitingroom;
			}
			else if (mx > 468 && mx < 750 && my>454 && my < 515)
			{
				state = server_input;
			}
		}
		if (waitingroom == state)
		{
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if (mx > 10 && mx < 90 && my>240 && my < 280)
			{
				WaitingPacket(CHARACHANGE);
				charac = (charac + 1) % 2;
			}
			else if (mx > 290 && mx < 370 && my>240 && my < 280)
			{
				WaitingPacket(CHARACHANGE);
				charac = (charac + 1) % 2;
			}
			else if (mx > 50 && mx < 330 && my>40 && my < 458)
			{
				WaitingPacket(TEAMCHANGE);
				my_team = (my_team + 1) % 2;
			}
			else if (mx > 357 && mx < 904 && my>514 && my < 661)
			{
				WaitingPacket(READY);
				ready = (ready + 1) % 2;
			}
		}
		else {
			if (m_sJungleButton.IsInButton(LOWORD(lParam), HIWORD(lParam)))
			{
				if (bIsClickedJungleButton)										// 정글 맵을 눌렀을 경우
				{
					m_pWaitingRoomInformDesc->m_iMyPlayerID = 0;				// 이건 준상이 알아서
					m_pWaitingRoomInformDesc->m_eSceneType = SCENE_TYPE_JUNGLE;
					for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)			// 이건 준상이 알아서
					{
						m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_bIsSlotActive = true;
						m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_PlayerType = PLAYER_TYPE_PIRATE;
						m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_BelongType = i < 4 ? BELONG_TYPE_BLUE : BELONG_TYPE_RED;
					}

					CGameState* pTempGameState = new CPlayGameState();
					pTempGameState->HandOverExternalInput(m_pWaitingRoomInformDesc);			// 외부 정보를 등록 시 다음과 같이 진행해야만 한다.
					CGameManager::GetInstance()->ChangeGameState(pTempGameState);								// 게임 플레이 상태로 바꾼다.
				}
			}
			else if (m_sSnowButton.IsInButton(LOWORD(lParam), HIWORD(lParam)))	// 눈 맵을 선택했을 경우
			{
				if (bIsClickedSnowButton)
				{
					m_pWaitingRoomInformDesc->m_iMyPlayerID = 0;				// 이건 준상이 알아서
					m_pWaitingRoomInformDesc->m_eSceneType = SCENE_TYPE_SNOW;
					for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)			// 이건 준상이 알아서
					{
						m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_bIsSlotActive = true;
						m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_PlayerType = PLAYER_TYPE_PIRATE;
						m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_BelongType = i < 4 ? BELONG_TYPE_BLUE : BELONG_TYPE_RED;
					}

					CGameState* pTempGameState = new CPlayGameState();
					pTempGameState->HandOverExternalInput(m_pWaitingRoomInformDesc);			// 외부 정보를 등록 시 다음과 같이 진행해야만 한다.
					CGameManager::GetInstance()->ChangeGameState(pTempGameState);
				}
			}
		}
		bIsClickedJungleButton = false;
		bIsClickedSnowButton = false;
		break;
	}
}

void CTempWaitingRoomState::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		if (state == server_input)
		{
			if (!wParam) break;
			if (wParam == VK_RETURN) {
				state = waitingroom;
				IsWaitingRoom = true;
				InvalidateRgn(hWnd, NULL, FALSE);
				break;
			}
			else if (wParam == VK_BACK)
			{
				if (strn>0)
					strn--;
			}
			else if (wParam == VK_OEM_PERIOD)
			{
				server_ip[strn++] = '.';
				server_ip[strn] = '\0';
			}
			else {
				server_ip[strn++] = wParam;
				server_ip[strn] = '\0';
			}
			InvalidateRgn(hWnd, NULL, FALSE);
			break;
		}
		else if (state == host_select)
		{
			if (!wParam) break;
			if (wParam == VK_RETURN) {
			}
			else if (wParam == VK_UP) {
				host = true;
				strcpy_s(server_msg, "Host");
			}
			else if (wParam == VK_DOWN) {
				host = false;
				strcpy_s(server_msg, "Join");
			}
			InvalidateRgn(hWnd, NULL, FALSE);
			break;
		}
	}
}
LRESULT CALLBACK CTempWaitingRoomState::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return 0;
}