// TreasureHunter.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "WorkingSpace.h"
#include "VTMakerFramework.h"
#include "VoxelTerrainMaker_Test_WorkingSpace.h"

#define MAX_LOADSTRING 100

// ���� ����:
CVTMakerFramework gVTMakerFramework;
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

												// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK	EditWorkingSpaceProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
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
	LoadStringW(hInstance, IDC_VOXELTERRAINMAKER_TEST_WORKINGSPACE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VOXELTERRAINMAKER_TEST_WORKINGSPACE));

	MSG msg;

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
			gVTMakerFramework.FrameAdvance();
		}
	}
	gVTMakerFramework.OnDestroy();


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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VOXELTERRAINMAKER_TEST_WORKINGSPACE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VOXELTERRAINMAKER_TEST_WORKINGSPACE);
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

	gVTMakerFramework.OnCreate(hInstance, hMainWnd);

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
		case IDM_EDIT_WORKINGSPACE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_EDIT_WORKINGSPACE), hWnd, EditWorkingSpaceProc);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CHAR:
		switch ((TCHAR)wParam)
		{
		case 'T' : case 't':
			if (gVTMakerFramework.m_pWorkingSpace) gVTMakerFramework.m_pWorkingSpace->MoveUpWorkingBoard();
			break;
		case 'G' : case 'g':
			if (gVTMakerFramework.m_pWorkingSpace) gVTMakerFramework.m_pWorkingSpace->MoveDownWorkingBoard();
			break;
		}
		break;
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gVTMakerFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK	EditWorkingSpaceProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	char value[10];
	D3DXVECTOR3 d3dxvWorkingSpaceSize;
	D3DXVECTOR3 d3dxvCubeSize;
	D3DXVECTOR3 d3dxvOffsetFromLocal;

	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_EDIT_LAYER, (LPCWSTR) L"1"); 
		SetDlgItemText(hDlg, IDC_EDIT_ROW, (LPCWSTR) L"1");
		SetDlgItemText(hDlg, IDC_EDIT_COL, (LPCWSTR) L"1");

		SetDlgItemText(hDlg, IDC_EDIT_WIDTH, (LPCWSTR) L"1.0");
		SetDlgItemText(hDlg, IDC_EDIT_HEIGHT, (LPCWSTR) L"1.0");
		SetDlgItemText(hDlg, IDC_EDIT_DEPTH, (LPCWSTR) L"1.0");

		SetDlgItemText(hDlg, IDC_EDIT_X, (LPCWSTR) L"0.0");
		SetDlgItemText(hDlg, IDC_EDIT_Y, (LPCWSTR) L"0.0");
		SetDlgItemText(hDlg, IDC_EDIT_Z, (LPCWSTR) L"0.0");
		return 1;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_OK:
			
			GetDlgItemText(hDlg, IDC_EDIT_LAYER, (LPWSTR) value, 10);
			d3dxvWorkingSpaceSize.y = _wtoi((const wchar_t*) value);
			// printf("WorkingSpaceSize.y : %f \n", d3dxvWorkingSpaceSize.y);
			GetDlgItemText(hDlg, IDC_EDIT_ROW, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.z = _wtoi((const wchar_t*) value);
			// printf("WorkingSpaceSize.z : %f \n", d3dxvWorkingSpaceSize.z);
			GetDlgItemText(hDlg, IDC_EDIT_COL, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.x = _wtoi((const wchar_t*) value);
			// printf("WorkingSpaceSize.x : %f \n", d3dxvWorkingSpaceSize.x);

			GetDlgItemText(hDlg, IDC_EDIT_WIDTH, (LPWSTR)value, 10);
			d3dxvCubeSize.x = _wtof((const wchar_t*) value);
			GetDlgItemText(hDlg, IDC_EDIT_HEIGHT, (LPWSTR)value, 10);
			d3dxvCubeSize.y = _wtof((const wchar_t*)value);
			GetDlgItemText(hDlg, IDC_EDIT_DEPTH, (LPWSTR)value, 10);
			d3dxvCubeSize.z = _wtof((const wchar_t*)value);

			GetDlgItemText(hDlg, IDC_EDIT_X, (LPWSTR)value, 10);
			d3dxvOffsetFromLocal.x = _wtof((const wchar_t*)value);
			GetDlgItemText(hDlg, IDC_EDIT_Y, (LPWSTR)value, 10);
			d3dxvOffsetFromLocal.y = _wtof((const wchar_t*)value);
			GetDlgItemText(hDlg, IDC_EDIT_Z, (LPWSTR)value, 10);
			d3dxvOffsetFromLocal.z = _wtof((const wchar_t*)value);

			/*printf("m_nMaxLayer : %d, m_nMaxRow : %d, m_nMaxCol : %d \n", gVTMakerFramework.m_nMaxLayer, gVTMakerFramework.m_nMaxRow, gVTMakerFramework.m_nMaxCol);
			printf("m_fCubeWidth : %f, m_fCubeHeight : %f, m_fCubeDepth : %f \n", gVTMakerFramework.m_fCubeWidth, gVTMakerFramework.m_fCubeHeight, gVTMakerFramework.m_fCubeDepth);
			printf("m_fOffsetXFromLocal : %f, m_fOffsetYFromLocal : %f, m_fOffsetZFromLocal : %f \n", gVTMakerFramework.m_fOffsetXFromLocal, gVTMakerFramework.m_fOffsetYFromLocal, gVTMakerFramework.m_fOffsetZFromLocal);*/
			
			gVTMakerFramework.CreateWorkingSpace(d3dxvWorkingSpaceSize, d3dxvCubeSize, d3dxvOffsetFromLocal);
			EndDialog(hDlg, 0);
			break;
		case ID_CANCEL:
			EndDialog(hDlg, 0);
			break;
		}
		break;
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
