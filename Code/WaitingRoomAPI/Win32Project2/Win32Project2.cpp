// Win32Project2.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
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

// ���� ����:
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
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT2));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
			
			
			
			// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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
