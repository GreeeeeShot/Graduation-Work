// Win32Project2.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Win32Project2.h"

#define MAX_LOADSTRING 100

enum State { host_select, server_input, play, waitingroom };
State state = host_select;
bool host = false;
int strn = 0;
bool ready = false;

char	server_ip[20] = "";
char server_msg[20] = "Join";
#define	WM_SOCKET				WM_USER + 1

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
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
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT2));

    MSG msg;

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32PROJECT2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
   DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
   AdjustWindowRect(&rc, dwStyle, FALSE);
   HWND hWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
	static HBITMAP hBit1,hipInput, hwaiting,oldBit1, oldBit2;
	static HBITMAP chara[3], charaSelect, blueSelect, redSelect;
	static RECT rectView;
	static int yPos;

	static int x, y;
	static bool Selection;
	int mx, my;

    switch (message)
    {
	case WM_CREATE:
	{
		hipInput = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP8));
		hwaiting = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP4));
		chara[0] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
		chara[1] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		chara[2] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
		charaSelect = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP6));
		blueSelect = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
		redSelect = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP7));
		break;
	}
	case WM_TIMER:
		
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
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
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (waitingroom == state)
		{
			if (mx > 0 && mx < 10 && my>0 && my < 10)
			{
				ready = (ready + 1) % 2;
			}
		}
		InvalidateRgn(hWnd, NULL, false);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (waitingroom == state)
		{
			if (mx > 0 && mx < 10 && my>0 && my < 10)
			{
				ready = (ready + 1) % 2;
			}
		}
		InvalidateRgn(hWnd, NULL, false);
		break;
    case WM_PAINT:
        {
			hdc  = GetDC(hWnd);
			hdc = BeginPaint(hWnd, &ps);
			
			if (hBit1 == NULL)
				hBit1 = CreateCompatibleBitmap(hdc, 1280, 760);

			memdc1 = CreateCompatibleDC(hdc);
			memdc2 = CreateCompatibleDC(memdc1);
			oldBit1 = (HBITMAP)SelectObject(memdc1, hBit1);
			
			
			
			// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
		if (state == server_input) {

			oldBit2 = (HBITMAP)SelectObject(memdc2, hipInput);
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
			oldBit2 = (HBITMAP)SelectObject(memdc2, hwaiting);

			BitBlt(memdc1, 0, 0, 1280, 760, memdc2, 0, 0, SRCCOPY);
			SelectObject(memdc2, oldBit2);

			BitBlt(hdc, 0, 0, 1280, 760, memdc1, 0, 0, SRCCOPY);
			SelectObject(memdc1, oldBit1);			
		}
		//else CGameManager::GetInstance()->m_pGameState->Render(hdc);

		

		DeleteDC(memdc2);
		DeleteDC(memdc1);
		EndPaint(hWnd, &ps);
        }
		return 0;
    case WM_DESTROY:
		if (hBit1) DeleteObject(hBit1);
		DeleteObject(hwaiting);
		DeleteObject(hipInput);
		for(int i =0; i<3;++i)
			DeleteObject(chara[i]);
		DeleteDC(memdc1);
        PostQuitMessage(0);
        break;
	case WM_KEYDOWN:
		if (state == server_input)
		{
			if (!wParam) break;
			if (wParam == VK_RETURN) {
				state = waitingroom;
				
				break;
			}
			else if (wParam == VK_BACK)
			{
				if(strn>0)
					strn--;
			}
			else {
				server_ip[strn++] = wParam;
				server_ip[strn] = '\0';
			}
			InvalidateRgn(hWnd, NULL, false);
			break;
		}
		else if (state == host_select)
		{
			if (!wParam) break;
			if (wParam == VK_RETURN) {
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
		else if (state == waitingroom)
		{

		}
		
    }
	InvalidateRgn(hWnd, NULL, false);
	return DefWindowProc(hWnd, message, wParam, lParam);
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
