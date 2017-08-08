#pragma once
#include "GameFramework.h"
#include "GameState.h"

class CGameManager
{
private:
	static CGameManager *gGameManagerInst;

public:
	CGameFramework *m_pGameFramework;
	CGameState *m_pGameState;
	HINSTANCE* m_pHinstance;
	HWND* m_pHwnd;

	CGameManager(HINSTANCE* pHinstance, HWND* pHmainWnd);
	~CGameManager();

	static CGameManager* GetInstance(void);
	static void CreateGameManagerInst(HINSTANCE* pHinstance, HWND* pHmainWnd);
	void ChangeGameState(CGameState *pGameState);
};

