#include "stdafx.h"
#include "GameState.h"
#include "GameManager.h"

HBITMAP CGameState::m_hMemBitmap = NULL;
HBITMAP CGameState::m_hIntroBackBitmap = NULL;
HBITMAP CGameState::m_hIntroCompanyBitmap = NULL;
HINSTANCE CGameState::m_hInstance;
HWND CGameState::m_hwnd;

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

	CGameState::m_hIntroBackBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_INTRO_BACK));
	CGameState::m_hIntroCompanyBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_INTRO_COMPANY_LOGO));
}

CPlayGameState::CPlayGameState()
{
	//m_bIsPushed = false;
	//m_bIsDigOrInstall = false;
}

CPlayGameState::~CPlayGameState()
{

}

void CPlayGameState::Init(void)
{
	//m_bIsPushed = false;
	//m_bIsDigOrInstall = false;
	//CGameManager::GetInstance()->m_pGameFramework->BuildObjects();
}

void CPlayGameState::Destroy(void)
{
	//CGameManager::GetInstance()->m_pGameFramework->ReleaseObjects();
}

void CPlayGameState::Update(void)
{
	////Ÿ�̸��� �ð��� ���ŵǵ��� �ϰ� ������ ����Ʈ�� ����Ѵ�. 
	//m_pFramework->m_GameTimer.Tick();

	//m_pFramework->ProcessInput();

	//m_pFramework->AnimateObjects();
}

void CPlayGameState::Render(void)
{
	CGameManager::GetInstance()->m_pGameFramework->FrameAdvance();
}

LRESULT CALLBACK CPlayGameState::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CGameManager::GetInstance()->m_pGameFramework->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
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
	else if(fTimeElapsed > 4.0f) CGameManager::GetInstance()->ChangeGameState(new CPlayGameState());

	InvalidateRgn(m_hwnd, NULL, FALSE);
}

void CIntroState::Render(void)
{

}

void CIntroState::Render(HDC hdc)
{
	// �Ʒ��� if���� hdc�� ���ڷ� �޴� Render �޼ҵ忡�� �ݵ�� �־��־�� �մϴ�.
	if (CGameState::m_hMemBitmap == NULL)
	{
		CGameState::m_hMemBitmap = CreateCompatibleBitmap(hdc, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	}

	/* �⺻ �޸� ����̽� ���ؽ�Ʈ�� �⺻ ��Ʈ�� ������ �����Ѵ�. */
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

	AlphaBlend(hMemDC, 325, 170, 630, 355, hCompanyDC, 0, 0, 630, 355, m_BlendFunction);
	
	SelectObject(hBackDC, oldBackBitmap);
	DeleteDC(hBackDC);
	SelectObject(hCompanyDC, oldCompanyBitmap);
	DeleteDC(hCompanyDC);

	/* ȭ�鿡 ����Ѵ�. */
	BitBlt(hdc, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, oldMemBitmap);
	DeleteDC(hMemDC);
}

//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
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