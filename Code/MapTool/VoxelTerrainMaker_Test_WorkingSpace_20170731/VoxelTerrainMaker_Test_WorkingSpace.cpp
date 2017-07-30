// TreasureHunter.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
// #include "WorkingSpace.h"
#include "VTMakerFramework.h"
#include "VoxelTerrainMaker_Test_WorkingSpace.h"

#define MAX_LOADSTRING 100
#define IDC_WORKINGSPACE_LIST_CTRL	5001
//#define IDC_ACTIVATE_WORKINGSPACE_BUTTON	5003
#define IDC_TO_STAMP_BUTTON	6001
#define IDC_ROTATE_RIGHT_BUTTON 6002
#define IDC_ROTATE_LEFT_BUTTON 6003

#define IDC_TO_STAMP_EDIT 7001
#define IDC_FROM_STAMP_EDIT 7002

// 전역 변수:
CVTMakerFramework gVTMakerFramework;
HWND ghMainWnd;					// 프레임 윈도우
HWND ghMainClientWnd;			// 클라이언트 윈도우
HWND ghSupportWnd[2];
HWND hWorkingSpaceList;
HWND hToStampEdit;
HWND hFromStampEdit;
HWND hToStampButton;
HWND hRotateRightButton;
HWND hRotateLeftButton;
HWND hActivateWorkingSpaceButton;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

												// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    FrameWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    SupportWndProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK	EditWorkingSpaceGenerateProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	EditWorkingSpaceRegenerateProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	EditWorkingSpaceTransformProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
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
	wcex.lpfnWndProc = FrameWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VOXELTERRAINMAKER_TEST_WORKINGSPACE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VOXELTERRAINMAKER_TEST_WORKINGSPACE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);

	wcex.lpfnWndProc = SupportWndProc;
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"Support Window Class Name";

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

	CLIENTCREATESTRUCT clientcreate;
	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH + SUPPORT_WINDOW_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	ghMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!ghMainWnd) return(FALSE);
	//GetClientRect(hWnd, &rectView);
	/*clientcreate.hWindowMenu = GetSubMenu(GetMenu(ghMainWnd), 0);
	clientcreate.idFirstChild = 100;
	ghMainClientWnd = CreateWindow(L"MDICLIENT", NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0, 0, 0,
		ghMainWnd, NULL, hInst, (LPSTR)&clientcreate);*/
	/**/ghSupportWnd[0] = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		L"Support Window Class Name",
		NULL,
		WS_CHILD | WS_VISIBLE,
		0,
		0,
		FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT,
		ghMainWnd,
		NULL,
		hInst,
		NULL
	);

	//printf("ghSupportWnd[0] %x \n", ghSupportWnd[0]);
	/*ghSupportWnd[1] = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		L"Support Window Class Name",
		NULL,
		WS_CHILD | WS_VISIBLE,
		rc.right / 2 + 1,
		0,
		rc.right / 2, rc.bottom,
		ghMainWnd,
		NULL,
		hInst,
		NULL
	);*/
	//printf("ghMainWnd, ghSupportWnd[0] [1]: %x, %x, %x\n", ghMainWnd, ghSupportWnd[0], ghSupportWnd[1]);
	::DragAcceptFiles(ghMainWnd, TRUE);

	gVTMakerFramework.OnCreate(hInstance, ghSupportWnd[0]);

	::ShowWindow(ghMainWnd, nCmdShow);
	::UpdateWindow(ghMainWnd);
	/*::ShowWindow(ghMainClientWnd, SW_SHOW);*/

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
LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME OFN, SFN;
	wchar_t str[1000];
	char multiStr[1000];
	wchar_t filepath[1000] = L"", filename[100] = L"", folder[100] = L"";
	char multiFilepath[1000];
	wchar_t filter[] = L"VoxelTerrain File\0*.txt\0";
	static HMENU hMenu, hSubMenu;
	static int iSelectedIdxInList;
	RECT rectView;
	POINT mouse;
	//printf("frame [%d %d]", WM_COMMAND, message);
	switch (message)
	{
	case WM_CREATE:
		printf("create!\n");
		hMenu = GetMenu(hWnd);
		hSubMenu = GetSubMenu(hMenu, 1);
		EnableMenuItem(hSubMenu, IDM_EDIT_WORKINGSPACE_TRANSFORM, MF_GRAYED);
		printf("hWnd ghMainWnd: %x %x\n", hWnd, ghMainWnd);
		hWorkingSpaceList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), NULL,
			WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_BORDER | WS_VSCROLL | WS_HSCROLL, FRAME_BUFFER_WIDTH + 50, 50, 300, 400, hWnd, (HMENU)IDC_WORKINGSPACE_LIST_CTRL, hInst, NULL);

		hToStampButton = CreateWindow(L"button", L"Register 'To'", 
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON , FRAME_BUFFER_WIDTH + 50 + 150, 510, 100, 25, 
			hWnd, (HMENU)IDC_TO_STAMP_BUTTON, hInst, NULL);

		hRotateRightButton = CreateWindow(L"button", L"->",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, FRAME_BUFFER_WIDTH + 50 + 150, 560, 100, 25,
			hWnd, (HMENU)IDC_ROTATE_RIGHT_BUTTON, hInst, NULL);

		hRotateLeftButton = CreateWindow(L"button", L"<-",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, FRAME_BUFFER_WIDTH + 50 + 50, 560, 100, 25,
			hWnd, (HMENU)IDC_ROTATE_LEFT_BUTTON, hInst, NULL);

		hToStampEdit = CreateWindow(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER, FRAME_BUFFER_WIDTH + 50, 510, 150, 25, hWnd, (HMENU) IDC_TO_STAMP_EDIT, hInst, NULL);

		hFromStampEdit = CreateWindow(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER, FRAME_BUFFER_WIDTH + 50, 480, 150, 25, hWnd, (HMENU)IDC_FROM_STAMP_EDIT, hInst, NULL);
		break;
	case WM_DROPFILES:
	{
		std::vector<std::wstring> dragFiles;
		TCHAR* refFiles;
		UINT refFilesLen;
		POINT refPoint;
		UINT dropCount = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, refFiles, refFilesLen);
		if (dropCount > 0)
		{
			for (int cnt = 0; cnt<dropCount; cnt++)
			{
				int fLen = DragQueryFile((HDROP)wParam, cnt, NULL, 0) + 1;
				TCHAR* newFName = new TCHAR[fLen];
				memset(newFName, 0, sizeof(TCHAR)*fLen);
				DragQueryFile((HDROP)wParam, cnt, newFName, fLen);

				int len = WideCharToMultiByte(CP_ACP, 0, newFName, -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, newFName, -1, multiStr, len, NULL, NULL);

				gVTMakerFramework.CreateWorkingSpaceFromFile(hWorkingSpaceList, multiStr);
				/*char str[30] = "작업공간";
				char num[5];
				itoa(gVTMakerFramework.m_iActivedWorkingSpaceIdx, num, 10);
				strcat(str, num);
				CHAR *strMB = str;
				WCHAR strWC[1024];
				MultiByteToWideChar(CP_ACP, 0, strMB, -1, strWC, 1024);
				SendMessage(hWorkingSpaceList, LB_ADDSTRING, gVTMakerFramework.m_iActivedWorkingSpaceIdx, (LPARAM)strWC);*/
			}
			hMenu = GetMenu(hWnd);
			hSubMenu = GetSubMenu(hMenu, 1);
			EnableMenuItem(hSubMenu, IDM_EDIT_WORKINGSPACE_TRANSFORM, MF_ENABLED);
		}
		break;
	}
	/*case WM_SETCURSOR:
		printf("sdfksjflsjelfjse");
		return DefWindowProc(hWnd, message, wParam, lParam);*/
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDC_WORKINGSPACE_LIST_CTRL:
			if (HIWORD(wParam) == LBN_SELCHANGE)
			{
				printf(" SelectedIdx : %d \n", (int)SendMessage(GetDlgItem(hWnd, IDC_WORKINGSPACE_LIST_CTRL), LB_GETCURSEL, 0, 0));
				gVTMakerFramework.ChangeWorkingSpace(iSelectedIdxInList = (int)SendMessage(GetDlgItem(hWnd, IDC_WORKINGSPACE_LIST_CTRL), LB_GETCURSEL, 0, 0));
			}
			else if (HIWORD(wParam) == LBN_DBLCLK)
			{
				int answer = MessageBox(hWnd, L"활성화 됩니다.", L"활성화", MB_OK);
				SendMessage(hWorkingSpaceList, LB_GETTEXT, iSelectedIdxInList, (LPARAM)str);
				SetDlgItemText(hWnd, IDC_FROM_STAMP_EDIT, str);
			}
			//EndDialog(ghMainWnd, 0);
			return 0;// DefWindowProc(hWnd, message, wParam, lParam);
		case IDC_TO_STAMP_BUTTON:
			if(gVTMakerFramework.m_iWorkingSpaceNum > 0)
			{ 
				SendMessage(hWorkingSpaceList, LB_GETTEXT, iSelectedIdxInList, (LPARAM)str);
				SetDlgItemText(hWnd, IDC_TO_STAMP_EDIT, str);
				gVTMakerFramework.SetToStampWoringSpace(iSelectedIdxInList);
			}
			else
			{
				SetDlgItemText(hWnd, IDC_TO_STAMP_EDIT, L"");
			}
			break;
		case IDC_ROTATE_RIGHT_BUTTON:
			gVTMakerFramework.RotateWorkingSpaceRight();
			break;
		case IDC_ROTATE_LEFT_BUTTON:
			gVTMakerFramework.RotateWorkingSpaceLeft();
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EDIT_WORKINGSPACE_GENERATE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_EDIT_WORKINGSPACE_GENERATE), hWnd, EditWorkingSpaceGenerateProc);
			break;
		case  IDM_EDIT_WORKINGSPACE_REGENERATE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_EDIT_WORKINGSPACE_GENERATE), hWnd, EditWorkingSpaceRegenerateProc);
			break;
		case IDM_EDIT_WORKINGSPACE_DESTROY:
			SendMessage(hWorkingSpaceList, LB_DELETESTRING, gVTMakerFramework.m_iActivedWorkingSpaceIdx, 0);
			gVTMakerFramework.DestroyWorkingSpace();
			break;
		case IDM_EDIT_WORKINGSPACE_TRANSFORM:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_EDIT_WORKINGSPACE_REVISE), hWnd, EditWorkingSpaceTransformProc);
			break;
		case IDM_EDIT_WORKINGSPACE_OPTiMIZE:
			gVTMakerFramework.OptimizeWorkingSpace();
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
					//gVTMakerFramework.GetActivedWorkingSpace()->SaveWorkingSpaceToFile(multiStr);
					gVTMakerFramework.SaveActivedWorkingSpaceToFile(multiStr);
				}
			}
			break;
		case IDM_FILE_SAVE_ALL:
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
					//gVTMakerFramework.GetActivedWorkingSpace()->SaveWorkingSpaceToFile(multiStr);
					gVTMakerFramework.SaveAllWorkingSpaceToFile(multiStr);
				}
			}
			break;
		case IDM_FILE_SAVE_INDIVIDUAL:
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
					//gVTMakerFramework.GetActivedWorkingSpace()->SaveWorkingSpaceToFile(multiStr);
					gVTMakerFramework.SaveIndividualWorkingSpaceToFile(multiStr);
				}
			}
			break;
		case IDM_FILE_OPEN:
			printf("open");
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
					gVTMakerFramework.CreateWorkingSpaceFromFile(hWorkingSpaceList, multiStr);
					
					/*char str[30] = "작업공간";
					char num[5];
					itoa(gVTMakerFramework.m_iActivedWorkingSpaceIdx, num, 10);
					strcat(str, num);
					CHAR *strMB = str;
					WCHAR strWC[1024];
					MultiByteToWideChar(CP_ACP, 0, strMB, -1, strWC, 1024);
					SendMessage(hWorkingSpaceList, LB_ADDSTRING, gVTMakerFramework.m_iActivedWorkingSpaceIdx, (LPARAM)strWC);*/
					hMenu = GetMenu(hWnd);
					hSubMenu = GetSubMenu(hMenu, 1);
					EnableMenuItem(hSubMenu, IDM_EDIT_WORKINGSPACE_TRANSFORM, MF_ENABLED);
				}
			}
			break;
		case IDM_EDIT_MTRL_GRASS_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_GRASS_0);
			break;
		case IDM_EDIT_MTRL_WOOD_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_WOOD_0);
			break;
		case IDM_EDIT_MTRL_ZUNGLE_UPHILL_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_ZUNGLE_UPHILL_0);
			break;
		case IDM_EDIT_MTRL_ZUNGLE_UPHILL_1:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_ZUNGLE_UPHILL_1);
			break;
		case IDM_EDIT_MTRL_ZUNGLE_UPHILL_2:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_ZUNGLE_UPHILL_2);
			break;
		case IDM_EDIT_MTRL_ZUNGLE_UPHILL_3:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_ZUNGLE_UPHILL_3);
			break;
		case IDM_EDIT_MTRL_BUSH_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_BUSH_0);
			break;
		case IDM_EDIT_MTRL_SWAMP_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_SWAMP_0);
			break;
		case IDM_EDIT_MTRL_JUMP_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_JUMP_0);
			break;
		case IDM_EDIT_MTRL_SNOW_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_SNOW_0);
			break;
		case IDM_EDIT_MTRL_EMPTY_SNOW_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_EMPTY_SNOW_0);
			break;
		case IDM_EDIT_MTRL_ICE_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_ICE_0);
			break;
		case IDM_VIEW_ALL:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeViewMode(VIEW_MODE_ALL);
			break;
		case IDM_VIEW_ACCUMULATE:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeViewMode(VIEW_MODE_ACCUMULATE);
			break;
		case IDM_VIEW_RELEVANT:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeViewMode(VIEW_MODE_RELEVANT);
			break;
		case IDM_EDIT_MTRL_SNOW_UPHILL_0:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_SNOW_UPHILL_0);
			break;
		case IDM_EDIT_MTRL_SNOW_UPHILL_1:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_SNOW_UPHILL_1);
			break;
		case IDM_EDIT_MTRL_SNOW_UPHILL_2:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_SNOW_UPHILL_2);
			break;
		case IDM_EDIT_MTRL_SNOW_UPHILL_3:
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->ChangeMaterial(VOXEL_TYPE_SNOW_UPHILL_3);
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
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->MoveUpWorkingBoard();
			break;
		case 'G' : case 'g':
			if (gVTMakerFramework.GetActivedWorkingSpace()) gVTMakerFramework.GetActivedWorkingSpace()->MoveDownWorkingBoard();
			break;
		case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
			gVTMakerFramework.ChangeWorkingSpace((TCHAR)wParam - '0' - 1);
			break;
		}
		break;
	/*case WM_SIZE:
		GetClientRect(hWnd, &rectView);

		MoveWindow(ghSupportWnd[0],
			0,
			0,
			FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, TRUE);*/

		/*printf("rectView.right, rectView.bottom : %d, %d", rectView.right, rectView.bottom);
		MoveWindow(ghSupportWnd[1], 
			rectView.right / 2 + 10,
			0,
			rectView.right / 2 - 11, 
			rectView.bottom, TRUE);*/
	case WM_LBUTTONDOWN:
		//printf("selectedIdx : %d \n", (int)SendMessage(hWorkingSpaceList, LB_GETCURSEL, 0, 0));
		//gVTMakerFramework.ChangeWorkingSpace((int)SendMessage(hWorkingSpaceList, LB_GETCURSEL, 0, 0));
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		/*printf("mouse : %d %d\n", LOWORD(lParam), HIWORD(lParam));
		if ((0 < LOWORD(lParam) && LOWORD(lParam) < rectView.right) && (0 < HIWORD(lParam) && HIWORD(lParam) < rectView.bottom)) SetCapture(ghSupportWnd[0]);
		else ReleaseCapture();*/
	case WM_KEYDOWN:
	case WM_KEYUP:
		//printf("FrameKey");
		if (ghSupportWnd[0]) gVTMakerFramework.OnProcessingWindowMessage(ghSupportWnd[0], message, wParam, lParam);
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

LRESULT CALLBACK    SupportWndProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rectView;
	
	switch (iMsg)
	{
	case WM_CREATE:
		break;
	case WM_SIZE:
		printf("size\n");
		if (ghSupportWnd[0])
		{
			GetClientRect(hDlg, &rectView);
			printf("ghSupportWnd[0] %x %x \n", ghSupportWnd[0], hDlg);
			printf("w : %d, h : %d\n", LOWORD(lParam), HIWORD(lParam));
			MoveWindow(ghSupportWnd[0],
				0,
				0,
				LOWORD(lParam), HIWORD(lParam), TRUE);
			gVTMakerFramework.OnProcessingWindowMessage(hDlg, iMsg, wParam, lParam);
		}
		
		break;
	//case WM_COMMAND:
	//{
	//	int wmId = LOWORD(wParam);
	//	// 메뉴 선택을 구문 분석합니다.
	//	switch (wmId)
	//	{
	//	case IDC_WORKINGSPACE_LIST_CTRL:
	//		if (HIWORD(wParam) == LBN_SELCHANGE)
	//		{
	//			printf(" SelectedIdx : %d \n", (int)SendMessage(hWorkingSpaceList, LB_GETCURSEL, 0, 0));
	//			gVTMakerFramework.ChangeWorkingSpace((int)SendMessage(hWorkingSpaceList, LB_GETCURSEL, 0, 0));
	//		}
	//		break;
	//	}
	//}
	case WM_LBUTTONDOWN:
		//printf("selectedIdx : %d \n", (int)SendMessage(hWorkingSpaceList, LB_GETCURSEL, 0, 0));
		//gVTMakerFramework.ChangeWorkingSpace((int)SendMessage(hWorkingSpaceList, LB_GETCURSEL, 0, 0));
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		//case WM_MOUSEMOVE:
		/*printf("mouse : %d %d\n", LOWORD(lParam), HIWORD(lParam));
		if ((0 < LOWORD(lParam) && LOWORD(lParam) < rectView.right) && (0 < HIWORD(lParam) && HIWORD(lParam) < rectView.bottom)) SetCapture(ghSupportWnd[0]);
		else ReleaseCapture();*/
	case WM_KEYDOWN:
	case WM_KEYUP:
		//printf("FrameKey");
		if (ghSupportWnd[0])
		{
			//printf("Support");
			gVTMakerFramework.OnProcessingWindowMessage(ghSupportWnd[0], iMsg, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		break;
	default:
		return DefMDIChildProc(hDlg, iMsg, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK	EditWorkingSpaceGenerateProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	char value[10];
	D3DXVECTOR3 d3dxvWorkingSpaceSize;
	D3DXVECTOR3 d3dxvCubeSize;
	D3DXVECTOR3 d3dxvOffsetFromLocal;
	static HMENU hMenu, hSubMenu;

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
		{
			GetDlgItemText(hDlg, IDC_EDIT_LAYER, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.y = _wtoi((const wchar_t*)value);
			// printf("WorkingSpaceSize.y : %f \n", d3dxvWorkingSpaceSize.y);
			GetDlgItemText(hDlg, IDC_EDIT_ROW, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.z = _wtoi((const wchar_t*)value);
			// printf("WorkingSpaceSize.z : %f \n", d3dxvWorkingSpaceSize.z);
			GetDlgItemText(hDlg, IDC_EDIT_COL, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.x = _wtoi((const wchar_t*)value);
			// printf("WorkingSpaceSize.x : %f \n", d3dxvWorkingSpaceSize.x);

			GetDlgItemText(hDlg, IDC_EDIT_WIDTH, (LPWSTR)value, 10);
			d3dxvCubeSize.x = _wtof((const wchar_t*)value);
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
			char str[30] = "작업공간";
			char num[5];
			itoa(gVTMakerFramework.m_iActivedWorkingSpaceIdx, num, 10);
			strcat(str, num);
			CHAR *strMB = str;
			WCHAR strWC[1024];
			MultiByteToWideChar(CP_ACP, 0, strMB, -1, strWC, 1024);
			SendMessage(hWorkingSpaceList, LB_ADDSTRING, gVTMakerFramework.m_iActivedWorkingSpaceIdx, (LPARAM)strWC);
			hMenu = GetMenu(ghMainWnd);
			hSubMenu = GetSubMenu(hMenu, 1);
			EnableMenuItem(hSubMenu, IDM_EDIT_WORKINGSPACE_TRANSFORM, MF_ENABLED);

			EndDialog(hDlg, 0);
		}
			break;
		case ID_CANCEL:
			EndDialog(hDlg, 0);
			break;
		}
		break;
	}
	return 0;
}

INT_PTR CALLBACK	EditWorkingSpaceRegenerateProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	char value[10];
	D3DXVECTOR3 d3dxvWorkingSpaceSize;
	D3DXVECTOR3 d3dxvCubeSize;
	D3DXVECTOR3 d3dxvOffsetFromLocal;
	static HMENU hMenu, hSubMenu;

	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_EDIT_LAYER, (LPCWSTR)L"1");
		SetDlgItemText(hDlg, IDC_EDIT_ROW, (LPCWSTR)L"1");
		SetDlgItemText(hDlg, IDC_EDIT_COL, (LPCWSTR)L"1");

		SetDlgItemText(hDlg, IDC_EDIT_WIDTH, (LPCWSTR)L"1.0");
		SetDlgItemText(hDlg, IDC_EDIT_HEIGHT, (LPCWSTR)L"1.0");
		SetDlgItemText(hDlg, IDC_EDIT_DEPTH, (LPCWSTR)L"1.0");

		SetDlgItemText(hDlg, IDC_EDIT_X, (LPCWSTR)L"0.0");
		SetDlgItemText(hDlg, IDC_EDIT_Y, (LPCWSTR)L"0.0");
		SetDlgItemText(hDlg, IDC_EDIT_Z, (LPCWSTR)L"0.0");
		return 1;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_OK:

			GetDlgItemText(hDlg, IDC_EDIT_LAYER, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.y = _wtoi((const wchar_t*)value);
			// printf("WorkingSpaceSize.y : %f \n", d3dxvWorkingSpaceSize.y);
			GetDlgItemText(hDlg, IDC_EDIT_ROW, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.z = _wtoi((const wchar_t*)value);
			// printf("WorkingSpaceSize.z : %f \n", d3dxvWorkingSpaceSize.z);
			GetDlgItemText(hDlg, IDC_EDIT_COL, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.x = _wtoi((const wchar_t*)value);
			// printf("WorkingSpaceSize.x : %f \n", d3dxvWorkingSpaceSize.x);

			GetDlgItemText(hDlg, IDC_EDIT_WIDTH, (LPWSTR)value, 10);
			d3dxvCubeSize.x = _wtof((const wchar_t*)value);
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

			gVTMakerFramework.RegenerateWorkingSpace(d3dxvWorkingSpaceSize, d3dxvCubeSize, d3dxvOffsetFromLocal);

			hMenu = GetMenu(ghMainWnd);
			hSubMenu = GetSubMenu(hMenu, 1);
			EnableMenuItem(hSubMenu, IDM_EDIT_WORKINGSPACE_TRANSFORM, MF_ENABLED);

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

INT_PTR CALLBACK	EditWorkingSpaceTransformProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	char value[10];
	D3DXVECTOR3 d3dxvWorkingSpaceSize;
	D3DXVECTOR3 d3dxvCubeSize;
	D3DXVECTOR3 d3dxvOffsetFromLocal;
	wchar_t str[100];


	switch (iMsg)
	{
	case WM_INITDIALOG:
		_stprintf(str, L"%d", gVTMakerFramework.GetActivedWorkingSpace()->m_nMaxLayer);
		SetDlgItemText(hDlg, IDC_EDIT_LAYER, (LPCWSTR)str);
		_stprintf(str, L"%d", gVTMakerFramework.GetActivedWorkingSpace()->m_nMaxRow);
		SetDlgItemText(hDlg, IDC_EDIT_ROW, (LPCWSTR)str);
		_stprintf(str, L"%d", gVTMakerFramework.GetActivedWorkingSpace()->m_nMaxCol);
		SetDlgItemText(hDlg, IDC_EDIT_COL, (LPCWSTR)str);

		_stprintf(str, L"%.2f", gVTMakerFramework.GetActivedWorkingSpace()->m_fCubeWidth);
		SetDlgItemText(hDlg, IDC_EDIT_WIDTH, (LPCWSTR)str);
		_stprintf(str, L"%.2f", gVTMakerFramework.GetActivedWorkingSpace()->m_fCubeHeight);
		SetDlgItemText(hDlg, IDC_EDIT_HEIGHT, (LPCWSTR)str);
		_stprintf(str, L"%.2f", gVTMakerFramework.GetActivedWorkingSpace()->m_fCubeDepth);
		SetDlgItemText(hDlg, IDC_EDIT_DEPTH, (LPCWSTR)str);

		_stprintf(str, L"%.2f", gVTMakerFramework.GetActivedWorkingSpace()->m_fOffsetXFromLocal);
		SetDlgItemText(hDlg, IDC_EDIT_X, (LPCWSTR)str);
		_stprintf(str, L"%.2f", gVTMakerFramework.GetActivedWorkingSpace()->m_fOffsetYFromLocal);
		SetDlgItemText(hDlg, IDC_EDIT_Y, (LPCWSTR)str);
		_stprintf(str, L"%.2f", gVTMakerFramework.GetActivedWorkingSpace()->m_fOffsetZFromLocal);
		SetDlgItemText(hDlg, IDC_EDIT_Z, (LPCWSTR)str);
		return 1;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_OK:

			GetDlgItemText(hDlg, IDC_EDIT_LAYER, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.y = _wtoi((const wchar_t*)value);
			// printf("WorkingSpaceSize.y : %f \n", d3dxvWorkingSpaceSize.y);
			GetDlgItemText(hDlg, IDC_EDIT_ROW, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.z = _wtoi((const wchar_t*)value);
			// printf("WorkingSpaceSize.z : %f \n", d3dxvWorkingSpaceSize.z);
			GetDlgItemText(hDlg, IDC_EDIT_COL, (LPWSTR)value, 10);
			d3dxvWorkingSpaceSize.x = _wtoi((const wchar_t*)value);
			// printf("WorkingSpaceSize.x : %f \n", d3dxvWorkingSpaceSize.x);

			GetDlgItemText(hDlg, IDC_EDIT_WIDTH, (LPWSTR)value, 10);
			d3dxvCubeSize.x = _wtof((const wchar_t*)value);
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

			gVTMakerFramework.TransformWorkingSpace(d3dxvWorkingSpaceSize, d3dxvCubeSize, d3dxvOffsetFromLocal);
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
