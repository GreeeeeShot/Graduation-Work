// TreasureHunter.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
// #include "WorkingSpace.h"
#include "VTMakerFramework.h"
#include "VoxelTerrainMaker_Test_WorkingSpace.h"

#define MAX_LOADSTRING 100

// 전역 변수:
CVTMakerFramework gVTMakerFramework;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

												// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
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
	//CONOUT$ - console 창
	//wt - 텍스트 쓰기 모드
	//stdout - 출력될 파일 포인터(모니터로 지정)

	printf("hello DEBUG\n");
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_VOXELTERRAINMAKER_TEST_WORKINGSPACE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VOXELTERRAINMAKER_TEST_WORKINGSPACE));

	MSG msg;

	// 기본 메시지 루프입니다.
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VOXELTERRAINMAKER_TEST_WORKINGSPACE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VOXELTERRAINMAKER_TEST_WORKINGSPACE);
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
	OPENFILENAME OFN, SFN;
	wchar_t str[100];
	char multiStr[100];
	wchar_t filepath[1000] = L"", filename[100] = L"", folder[100] = L"";
	wchar_t filter[] = L"VoxelTerrain File\0*.txt\0";

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EDIT_WORKINGSPACE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_EDIT_WORKINGSPACE), hWnd, EditWorkingSpaceProc);
			break;
		case IDM_FILE_SAVE:
			memset(&SFN, 0, sizeof(OPENFILENAME));
			SFN.lStructSize = sizeof(OPENFILENAME);
			SFN.hwndOwner = hWnd;
			SFN.lpstrFilter = filter;   //중요
			SFN.lpstrFile = filepath;
			SFN.nMaxFile = 1000;
			SFN.lpstrFileTitle = filename;
			SFN.nMaxFileTitle = 100;
			// OFN.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;   // 여러 개를 선택할 수 있다.
			//OFN.lpstrInitialDir = ".";      //초기 현재 실행 폴더
			if (GetSaveFileName(&SFN) != 0)
			{
				wsprintf(str, L"%s 파일로 저장 하시겠습니까?", SFN.lpstrFileTitle);
				int answer;
				//wcstombs(str, (const wchar_t*)OFN.lpstrFileTitle, strlen((const char*)OFN.lpstrFileTitle) + 1);
				int len = WideCharToMultiByte(CP_ACP, 0, SFN.lpstrFileTitle, -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, SFN.lpstrFileTitle, -1, multiStr, len, NULL, NULL);

				answer = MessageBox(hWnd, str, L"저장 확인", MB_YESNO);
				if (answer == IDYES)
				{
					gVTMakerFramework.m_pWorkingSpace->SaveWorkingSpaceToFile(multiStr);
				}
			}
			break;
		case IDM_FILE_OPEN:
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = filter;   //중요
			OFN.lpstrFile = filepath;
			OFN.nMaxFile = 1000;
			OFN.lpstrFileTitle = filename;
			OFN.nMaxFileTitle = 100;
			// OFN.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;   // 여러 개를 선택할 수 있다.
															   //OFN.lpstrInitialDir = ".";      //초기 현재 실행 폴더
			if (GetOpenFileName(&OFN) != 0)
			{
				wsprintf(str, L"%s 파일을 오픈 하시겠습니까?", OFN.lpstrFileTitle);
				int answer;
				//wcstombs(str, (const wchar_t*)OFN.lpstrFileTitle, strlen((const char*)OFN.lpstrFileTitle) + 1);
				int len = WideCharToMultiByte(CP_ACP, 0, OFN.lpstrFileTitle, -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, OFN.lpstrFileTitle, -1, multiStr, len, NULL, NULL);
				
				answer = MessageBox(hWnd, str, L"오픈 확인", MB_YESNO);
				if (answer == IDYES)
				{
					gVTMakerFramework.CreateWorkingSpaceFromFile(multiStr);
				}
			}
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
		case '1': case '2': case '3': case '4': case '5': case '6': case '7':
			if (gVTMakerFramework.m_pWorkingSpace) gVTMakerFramework.m_pWorkingSpace->ChangeMaterial(wParam - '0' -1);
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
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
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
