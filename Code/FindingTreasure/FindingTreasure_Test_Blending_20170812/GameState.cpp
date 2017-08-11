#include "stdafx.h"
#include "GameState.h"
#include "GameManager.h"

HBITMAP CGameState::m_hMemBitmap = NULL;
HBITMAP CGameState::m_hIntroBackBitmap = NULL;
HBITMAP CGameState::m_hIntroCompanyBitmap = NULL;
HBITMAP CGameState::m_hTempWaitingRoomBitmap = NULL;
HINSTANCE CGameState::m_hInstance;
HWND CGameState::m_hwnd;

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

	CGameState::m_hIntroBackBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_INTRO_BACK));
	CGameState::m_hIntroCompanyBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_INTRO_COMPANY_LOGO));
	CGameState::m_hTempWaitingRoomBitmap = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TEMP_WAITINGROOM));
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
	////Ÿ�̸��� �ð��� ���ŵǵ��� �ϰ� ������ ����Ʈ�� ����Ѵ�. 
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
		//	//CMeshResource::DestroyMeshResource();			// �޽� ���ҽ��� �����ϱ� ������ ���̴� ���ҽ��� FBX ���ҽ��� ���õǾ�� �Ѵ�.
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

	AlphaBlend(hMemDC, (FRAME_BUFFER_WIDTH - 630) / 2, (FRAME_BUFFER_HEIGHT - 355) / 2, 630, 355, hCompanyDC, 0, 0, 630, 355, m_BlendFunction);
	
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


CTempWaitingRoomState::CTempWaitingRoomState() 
{
	m_pWaitingRoomInformDesc = NULL;
}

CTempWaitingRoomState::~CTempWaitingRoomState() {}

void CTempWaitingRoomState::Init(void) 
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
	// �Ʒ��� if���� hdc�� ���ڷ� �޴� Render �޼ҵ忡�� �ݵ�� �־��־�� �մϴ�.
	if (CGameState::m_hMemBitmap == NULL)
	{
		CGameState::m_hMemBitmap = CreateCompatibleBitmap(hdc, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	}

	/* �⺻ �޸� ����̽� ���ؽ�Ʈ�� �⺻ ��Ʈ�� ������ �����Ѵ�. */
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP oldMemBitmap = (HBITMAP)SelectObject(hMemDC, CGameState::m_hMemBitmap);

	//HDC hBackDC = CreateCompatibleDC(hdc);
	//HBITMAP oldBackBitmap = (HBITMAP)SelectObject(hBackDC, CGameState::m_hIntroBackBitmap);
	//BitBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hBackDC, 0, 0, SRCCOPY);
	HDC hTempWaitingRoomDC = CreateCompatibleDC(hdc);
	HBITMAP oldTempWaitingRoomBitmap = (HBITMAP)SelectObject(hTempWaitingRoomDC, CGameState::m_hTempWaitingRoomBitmap);

	//TransparentBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hBackDC, 0, 0, 128, 72, RGB(0, 0, 0));			
	//TransparentBlt(hMemDC, 325, 182, 630, 355, hCompanyDC, 0, 0, 630, 355, RGB(0, 0, 0));

	BitBlt(hMemDC, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hTempWaitingRoomDC, 0, 0, SRCCOPY);

	//SelectObject(hBackDC, oldBackBitmap);
	//DeleteDC(hBackDC);
	SelectObject(hTempWaitingRoomDC, oldTempWaitingRoomBitmap);
	DeleteDC(hTempWaitingRoomDC);

	/* ȭ�鿡 ����Ѵ�. */
	BitBlt(hdc, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, oldMemBitmap);
	DeleteDC(hMemDC);
}

void CTempWaitingRoomState::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	static LONG x;
	static LONG y;

	static bool bIsClickedJungleButton = false;				// DOWN ��ư �� ��ư�� �������� Ȯ����. UP�� �ƴ�!
	static bool bIsClickedSnowButton = false;				// ..

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		if (m_sJungleButton.IsInButton(LOWORD(lParam), HIWORD(lParam))) bIsClickedJungleButton = true;		// �ϴ� �ش� ��ư�� ������ Ȯ����.
		else if (m_sSnowButton.IsInButton(LOWORD(lParam), HIWORD(lParam))) bIsClickedSnowButton = true;
		break;
	case WM_LBUTTONUP:			// ���콺 Ŭ�� �� ���콺 Ŀ���� �Ʊ� ������ ��ư ���� �ִٸ� ���� �÷��� ���·� ��ȯ��.
		if (m_sJungleButton.IsInButton(LOWORD(lParam), HIWORD(lParam)))
		{
			if (bIsClickedJungleButton)										// ���� ���� ������ ���
			{
				m_pWaitingRoomInformDesc->m_iMyPlayerID = 0;				// �̰� �ػ��� �˾Ƽ�
				m_pWaitingRoomInformDesc->m_eSceneType = SCENE_TYPE_JUNGLE;
				for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)			// �̰� �ػ��� �˾Ƽ�
				{
					m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_bIsSlotActive = true;
					m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_PlayerType = PLAYER_TYPE_PIRATE;
					m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_BelongType = i < 4 ? BELONG_TYPE_BLUE : BELONG_TYPE_RED;
				}

				CGameState* pTempGameState = new CPlayGameState();
				pTempGameState->HandOverExternalInput(m_pWaitingRoomInformDesc);			// �ܺ� ������ ��� �� ������ ���� �����ؾ߸� �Ѵ�.
				CGameManager::GetInstance()->ChangeGameState(pTempGameState);								// ���� �÷��� ���·� �ٲ۴�.
			}
		}
		else if (m_sSnowButton.IsInButton(LOWORD(lParam), HIWORD(lParam)))	// �� ���� �������� ���
		{
			if (bIsClickedSnowButton)
			{
				m_pWaitingRoomInformDesc->m_iMyPlayerID = 0;				// �̰� �ػ��� �˾Ƽ�
				m_pWaitingRoomInformDesc->m_eSceneType = SCENE_TYPE_SNOW;
				for (int i = 0; i < MAX_CONNECTED_PLAYERS_NUM; i++)			// �̰� �ػ��� �˾Ƽ�
				{
					m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_bIsSlotActive = true;
					m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_PlayerType = PLAYER_TYPE_PIRATE;
					m_pWaitingRoomInformDesc->m_PlayerInformDesc[i].m_BelongType = i < 4 ? BELONG_TYPE_BLUE : BELONG_TYPE_RED;
				}

				CGameState* pTempGameState = new CPlayGameState();
				pTempGameState->HandOverExternalInput(m_pWaitingRoomInformDesc);			// �ܺ� ������ ��� �� ������ ���� �����ؾ߸� �Ѵ�.
				CGameManager::GetInstance()->ChangeGameState(pTempGameState);
			}
		}
		bIsClickedJungleButton = false;
		bIsClickedSnowButton = false;
		break;
	}
}

void CTempWaitingRoomState::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	
}
LRESULT CALLBACK CTempWaitingRoomState::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
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