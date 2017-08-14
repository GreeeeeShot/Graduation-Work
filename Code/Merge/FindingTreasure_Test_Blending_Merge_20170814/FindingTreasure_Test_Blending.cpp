// TreasureHunter.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <windows.h>   // include important windows stuff
#include <windowsx.h>
#include <thread>
//#include "GameFramework.h"
//#include "GameState.h"
#include "GameManager.h"
#include "SoundManager.h"
#include "FindingTreasure_Test_Blending.h"

#include "IOCP_Client.h"
#include "IOCP_Server.h"


#define MAX_LOADSTRING 100

// 전역 변수:


#define	WM_SOCKET				WM_USER + 1

WaitingPlayer waitingplayer[7];



//CGameFramework gGameFramework;
HWND ghWnd;
HINSTANCE ghInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

												// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	AllocConsole();
	freopen("CONOUT$", "wt", stdout);
	//CONOUT$ - console 창
	//wt - 텍스트 쓰기 모드
	//stdout - 출력될 파일 포인터(모니터로 지정)

	printf("hello DEBUG\n");
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_FINDINGTREASURE_TEST_BLENDING, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 네트워크 프레임워크 시작

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}



	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FINDINGTREASURE_TEST_BLENDING));

	MSG msg;

	std::thread* timer_thread = NULL;
	std::thread* accept_thread = NULL;
	std::vector<std::thread *> worker_threads;

	  // 기본 메시지 루프입니다.
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// test if this is a quit
			if (msg.message == WM_QUIT)
				break;

			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		}
		//gGameFramework.FrameAdvance();
		CGameManager::GetInstance()->m_pGameState->Render();
		CGameManager::GetInstance()->m_pGameState->Update();
		//CGameManager::GetInstance()->ChangeGameState(new CPlayGameState());
		if (IsWaitingRoom)
		{
			if (host)
			{
				Initialize_Server();
				for (int i = 0; i < 8; ++i) worker_threads.push_back(new std::thread{ Worker_Thread });
				accept_thread = new std::thread(Accept_Thread);
				timer_thread = new std::thread(Time_Thread);
				strcpy_s(server_ip, "127.0.0.1");
				ClientMain(ghWnd, server_ip);
			}
			else
				ClientMain(ghWnd, server_ip);

			IsWaitingRoom = false;
		}
		
	}
	// gGameFramework.OnDestroy();
	delete CGameManager::GetInstance();
	if (host)
	{
		accept_thread->join();
		timer_thread->join();
		//Create_Accept_Thread();
		for (auto pth : worker_threads) {
			pth->join();
			delete pth;
		}
		ShutDown_Server();

		if (accept_thread != NULL)
			delete accept_thread;
	}
	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FINDINGTREASURE_TEST_BLENDING));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FINDINGTREASURE_TEST_BLENDING);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	ghInst = hInstance;

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	ghWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!ghWnd) return(FALSE);

	//gGameFramework.OnCreate(hInstance, hMainWnd);
	//CGameManager::GetInstance()->ChangeGameState(new CPlayGameState());

	CFBXAnimResource::CreateFBXAnimResource();
	CSoundManager::CreateSoundResource();
	CGameManager::CreateGameManagerInst(&ghInst, &ghWnd);
	CGameManager::GetInstance()->ChangeGameState(new CIntroState());			// 처음에는 회사 로고를 보여준다.

	::ShowWindow(ghWnd, nCmdShow);
	::UpdateWindow(ghWnd);

	IsWaitingRoom = false;
	host = 0;
	strn = 0;
	ZeroMemory(server_ip, sizeof(server_ip));
	strcpy_s(server_msg, "Join");
	ready = 0;
	state = host_select;

	return(TRUE);
}


//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc, memdc1, memdc2;
	PAINTSTRUCT ps;
	static HBITMAP hBit1, hipInputbmp, hwaitingbmp, oldBit1, oldBit2;
	static HBITMAP charabmp[2], SelectTeambmp[3], charaSelectbmp, readybmp;
	static RECT rectView;
	static int yPos;

	static int x, y;
	static bool Selection;
	int mx, my;

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(ghInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_SIZE:
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
		/*
		if (state == server_input)
		{
			if (!wParam) break;
			if (wParam == VK_RETURN) {
				state = waitingroom;
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
				if (host)
					state = waitingroom;
				else
					state = server_input;
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
			
		}*/
	case WM_KEYUP:
		//gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		CGameManager::GetInstance()->m_pGameState->OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
	{
		hdc = GetDC(hWnd);
		hdc = BeginPaint(hWnd, &ps);
		/*
		if (hBit1 == NULL)
			hBit1 = CreateCompatibleBitmap(hdc, 1280, 760);

		memdc1 = CreateCompatibleDC(hdc);
		memdc2 = CreateCompatibleDC(memdc1);
		oldBit1 = (HBITMAP)SelectObject(memdc1, hBit1);



		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
		if (state == server_input) {

			oldBit2 = (HBITMAP)SelectObject(memdc2, hipInputbmp);
			BitBlt(memdc1, 0, 0, 1280, 760, memdc2, 0, 0, SRCCOPY);

			SelectObject(memdc2, oldBit2);

			BitBlt(hdc, 0, 0, 1280, 760, memdc1, 0, 0, SRCCOPY);
			SelectObject(memdc1, oldBit1);

			TextOutA(hdc, 190, 350, "server ip input: ", strlen("server ip input: "));
			TextOutA(hdc, 300, 350, server_ip, strn);
		}
		else if (state == host_select)
		{
			TextOutA(hdc, 90, 100, server_msg, strlen(server_msg));
		}
		else if (state == waitingroom)
		{
			oldBit2 = (HBITMAP)SelectObject(memdc2, hwaitingbmp);

			BitBlt(memdc1, 0, 0, 1280, 760, memdc2, 0, 0, SRCCOPY);
			SelectObject(memdc2, oldBit2);

			/////////////////자신의 팀컬러 및 캐릭터, 화살표, 레디//////////////
			oldBit2 = (HBITMAP)SelectObject(memdc2, SelectTeambmp[my_team]);
			GdiTransparentBlt(memdc1, 10, 40, 374, 418, memdc2, 0, 0, 425, 475, RGB(255, 255, 255));

			oldBit2 = (HBITMAP)SelectObject(memdc2, charabmp[charac]);
			GdiTransparentBlt(memdc1, 55, 195, 258, 193, memdc2, 0, 0, 640, 480, RGB(255, 255, 255));

			oldBit2 = (HBITMAP)SelectObject(memdc2, charaSelectbmp);
			GdiTransparentBlt(memdc1, 10, 200, 360, 120, memdc2, 0, 0, 411, 142, RGB(255, 255, 255));

			if (ready) {
				oldBit2 = (HBITMAP)SelectObject(memdc2, readybmp);
				GdiTransparentBlt(memdc1, 80, 270, 227, 142, memdc2, 0, 0, 227, 142, RGB(255, 255, 255));
			}
			/////////////////팀컬러////////////////////////////////

			oldBit2 = (HBITMAP)SelectObject(memdc2, SelectTeambmp[waitingplayer[0].team]);
			GdiTransparentBlt(memdc1, 380, 70, 170, 190, memdc2, 0, 0, 425, 475, RGB(255, 255, 255));
			if (waitingplayer[0].connect)
			{
				oldBit2 = (HBITMAP)SelectObject(memdc2, charabmp[waitingplayer[0].charac]);
				GdiTransparentBlt(memdc1, 399, 140, 120, 90, memdc2, 0, 0, 640, 480, RGB(255, 255, 255));
				if (waitingplayer[0].ready)
				{
					oldBit2 = (HBITMAP)SelectObject(memdc2, readybmp);
					GdiTransparentBlt(memdc1, 405, 175, 113, 71, memdc2, 0, 0, 227, 142, RGB(255, 255, 255));
				}
			}

			oldBit2 = (HBITMAP)SelectObject(memdc2, SelectTeambmp[waitingplayer[1].team]);
			GdiTransparentBlt(memdc1, 550, 70, 170, 190, memdc2, 0, 0, 425, 475, RGB(255, 255, 255));
			if (waitingplayer[1].connect)
			{
				oldBit2 = (HBITMAP)SelectObject(memdc2, charabmp[waitingplayer[1].charac]);
				GdiTransparentBlt(memdc1, 569, 140, 120, 90, memdc2, 0, 0, 640, 480, RGB(255, 255, 255));
				if (waitingplayer[1].ready)
				{
					oldBit2 = (HBITMAP)SelectObject(memdc2, readybmp);
					GdiTransparentBlt(memdc1, 575, 175, 113, 71, memdc2, 0, 0, 227, 142, RGB(255, 255, 255));
				}
			}

			oldBit2 = (HBITMAP)SelectObject(memdc2, SelectTeambmp[waitingplayer[2].team]);
			GdiTransparentBlt(memdc1, 720, 70, 170, 190, memdc2, 0, 0, 425, 475, RGB(255, 255, 255));
			if (waitingplayer[2].connect)
			{
				oldBit2 = (HBITMAP)SelectObject(memdc2, charabmp[waitingplayer[2].charac]);
				GdiTransparentBlt(memdc1, 739, 140, 120, 90, memdc2, 0, 0, 640, 480, RGB(255, 255, 255));
				if (waitingplayer[2].ready)
				{
					oldBit2 = (HBITMAP)SelectObject(memdc2, readybmp);
					GdiTransparentBlt(memdc1, 745, 175, 113, 71, memdc2, 0, 0, 227, 142, RGB(255, 255, 255));
				}
			}

			oldBit2 = (HBITMAP)SelectObject(memdc2, SelectTeambmp[waitingplayer[3].team]);
			GdiTransparentBlt(memdc1, 340, 300, 170, 190, memdc2, 0, 0, 425, 475, RGB(255, 255, 255));
			if (waitingplayer[3].connect)
			{
				oldBit2 = (HBITMAP)SelectObject(memdc2, charabmp[waitingplayer[3].charac]);
				GdiTransparentBlt(memdc1, 359, 370, 120, 90, memdc2, 0, 0, 640, 480, RGB(255, 255, 255));
				if (waitingplayer[3].ready)
				{
					oldBit2 = (HBITMAP)SelectObject(memdc2, readybmp);
					GdiTransparentBlt(memdc1, 365, 405, 113, 71, memdc2, 0, 0, 227, 142, RGB(255, 255, 255));
				}
			}

			oldBit2 = (HBITMAP)SelectObject(memdc2, SelectTeambmp[waitingplayer[4].team]);
			GdiTransparentBlt(memdc1, 480, 300, 170, 190, memdc2, 0, 0, 425, 475, RGB(255, 255, 255));
			if (waitingplayer[4].connect)
			{
				oldBit2 = (HBITMAP)SelectObject(memdc2, charabmp[waitingplayer[4].charac]);
				GdiTransparentBlt(memdc1, 499, 370, 120, 90, memdc2, 0, 0, 640, 480, RGB(255, 255, 255));
				if (waitingplayer[4].ready)
				{
					oldBit2 = (HBITMAP)SelectObject(memdc2, readybmp);
					GdiTransparentBlt(memdc1, 505, 405, 113, 71, memdc2, 0, 0, 227, 142, RGB(255, 255, 255));
				}
			}

			oldBit2 = (HBITMAP)SelectObject(memdc2, SelectTeambmp[waitingplayer[5].team]);
			GdiTransparentBlt(memdc1, 620, 300, 170, 190, memdc2, 0, 0, 425, 475, RGB(255, 255, 255));
			if (waitingplayer[5].connect)
			{
				oldBit2 = (HBITMAP)SelectObject(memdc2, charabmp[waitingplayer[5].charac]);
				GdiTransparentBlt(memdc1, 639, 370, 120, 90, memdc2, 0, 0, 640, 480, RGB(255, 255, 255));
				if (waitingplayer[5].ready)
				{
					oldBit2 = (HBITMAP)SelectObject(memdc2, readybmp);
					GdiTransparentBlt(memdc1, 645, 405, 113, 71, memdc2, 0, 0, 227, 142, RGB(255, 255, 255));
				}
			}

			oldBit2 = (HBITMAP)SelectObject(memdc2, SelectTeambmp[waitingplayer[6].team]);
			GdiTransparentBlt(memdc1, 760, 300, 170, 190, memdc2, 0, 0, 425, 475, RGB(255, 255, 255));
			if (waitingplayer[6].connect)
			{
				oldBit2 = (HBITMAP)SelectObject(memdc2, charabmp[waitingplayer[6].charac]);
				GdiTransparentBlt(memdc1, 779, 370, 120, 90, memdc2, 0, 0, 640, 480, RGB(255, 255, 255));
				if (waitingplayer[6].ready)
				{
					oldBit2 = (HBITMAP)SelectObject(memdc2, readybmp);
					GdiTransparentBlt(memdc1, 785, 405, 113, 71, memdc2, 0, 0, 227, 142, RGB(255, 255, 255));
				}
			}

			BitBlt(hdc, 0, 0, 1280, 760, memdc1, 0, 0, SRCCOPY);
			SelectObject(memdc1, oldBit1);
		}*/
		CGameManager::GetInstance()->m_pGameState->Render(hdc);

		DeleteDC(memdc2);
		DeleteDC(memdc1);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SOCKET:
	{
		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			clienterror();
			break;
		}
		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
			ReadPacket((SOCKET)wParam);
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			clienterror();
			break;
		}
		
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	if(state != play)
		InvalidateRgn(hWnd, NULL, FALSE);
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
