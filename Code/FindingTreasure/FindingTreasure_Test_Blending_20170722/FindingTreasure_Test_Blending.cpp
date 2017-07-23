// TreasureHunter.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <windows.h>   // include important windows stuff
#include <windowsx.h>
#include <thread>
//#include "GameFramework.h"
//#include "GameState.h"
#include "GameManager.h"
#include "FindingTreasure_Test_Blending.h"

#include "IOCP_Client.h"
#include "IOCP_Server.h"


#define MAX_LOADSTRING 100

// ���� ����:
enum State { host_select, server_input, play };
State state = host_select;
bool host = 0;
int strn = 0;
char	server_ip[20] = "";
char server_msg[20] = "Join";
#define	WM_SOCKET				WM_USER + 1
//CGameFramework gGameFramework;
HWND hwnd = NULL;
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

												// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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
	//CONOUT$ - console â
	//wt - �ؽ�Ʈ ���� ���
	//stdout - ��µ� ���� ������(����ͷ� ����)

	printf("hello DEBUG\n");
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_FINDINGTREASURE_TEST_BLENDING, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ��Ʈ��ũ �����ӿ�ũ ����

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}



	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FINDINGTREASURE_TEST_BLENDING));

	MSG msg;

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
		} // end if
		if (state == play)
			break;
	} // end while
	std::thread* timer_thread = NULL;
	std::thread* accept_thread = NULL;
	std::vector<std::thread *> worker_threads;
	if (host)
	{
		Initialize_Server();
		for (int i = 0; i < 8; ++i) worker_threads.push_back(new std::thread{ Worker_Thread });
		accept_thread = new std::thread(Accept_Thread);
		timer_thread = new std::thread(Time_Thread);
		strcpy_s(server_ip,"127.0.0.1");
		ClientMain(hwnd, server_ip);
	}
	else
		ClientMain(hwnd, server_ip);

	// �⺻ �޽��� �����Դϴ�.
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
		{
			//gGameFramework.FrameAdvance();
			CGameManager::GetInstance()->m_pGameState->Update();
			CGameManager::GetInstance()->m_pGameState->Render();
			//CGameManager::GetInstance()->ChangeGameState(new CPlayGameState());
		}
	}
	// gGameFramework.OnDestroy();
	delete CGameManager::GetInstance();

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

	return (int)msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hMainWnd) return(FALSE);

	//gGameFramework.OnCreate(hInstance, hMainWnd);
	//CGameManager::GetInstance()->ChangeGameState(new CPlayGameState());
	CGameManager::CreateGameManagerInst(hInst, hMainWnd);
	CGameManager::GetInstance()->ChangeGameState(new CIntroState());

	::ShowWindow(hMainWnd, nCmdShow);
	::UpdateWindow(hMainWnd);

	return(TRUE);
}


//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �޴� ������ ���� �м��մϴ�.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
		if (state == server_input)
		{
			if (!wParam) break;
			if (wParam == VK_RETURN) {
				state = play;
				break;
			}
			else if (wParam == VK_BACK)
			{
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
			InvalidateRgn(hWnd, NULL, TRUE);
			break;
		}
		else if (state == host_select)
		{
			if (!wParam) break;
			if (wParam == VK_RETURN) {
				if (host)
					state = play;
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
			InvalidateRgn(hWnd, NULL, TRUE);
			break;
		}
	case WM_KEYUP:
		//gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		CGameManager::GetInstance()->m_pGameState->OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
		if (state == server_input) {
			TextOutA(hdc, 90, 100, "server ip input: ", strlen("server ip input: "));
			TextOutA(hdc, 200, 100, server_ip, strn);
		}
		else if (state == host_select)
		{
			TextOutA(hdc, 90, 100, server_msg, strlen(server_msg));
		}
		else CGameManager::GetInstance()->m_pGameState->Render(hdc);
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
	return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
