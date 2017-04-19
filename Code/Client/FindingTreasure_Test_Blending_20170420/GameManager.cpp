#include "stdafx.h"
#include "GameManager.h"

CGameManager* CGameManager::gGameManagerInst = NULL;

CGameManager::CGameManager(HINSTANCE hInstance, HWND hMainWnd)
{
	m_pGameFramework = new CGameFramework();
	m_pGameFramework->OnCreate(hInstance, hMainWnd);
	m_pGameState = NULL;//new CPlayGameState();
}


CGameManager::~CGameManager()
{
	if (m_pGameFramework) delete m_pGameFramework;
	if (m_pGameState) delete m_pGameState;
}

CGameManager* CGameManager::GetInstance(void)
{
	return CGameManager::gGameManagerInst;
}

void CGameManager::CreateGameManagerInst(HINSTANCE hInstance, HWND hMainWnd)
{
	if (CGameManager::gGameManagerInst == NULL) CGameManager::gGameManagerInst = new CGameManager(hInstance, hMainWnd);
}

void CGameManager::ChangeGameState(CGameState *pGameState)
{
	if (m_pGameState)
	{
		m_pGameState->Destroy();
		delete m_pGameState;
	}
	m_pGameState = pGameState;
	m_pGameState->Init();
}

//LRESULT CALLBACK CGameManager::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
//{
//	if(m_pGameState) m_pGameState->OnProcessingWindowMessage(hWnd, nMessageID)
//}