#include "stdafx.h"
#include "GameState.h"
#include "GameManager.h"

CGameState::CGameState()
{
}


CGameState::~CGameState()
{
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
	////타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산한다. 
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