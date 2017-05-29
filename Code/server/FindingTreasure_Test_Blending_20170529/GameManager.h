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

	CGameManager(HINSTANCE hInstance, HWND hMainWnd);
	~CGameManager();

	static CGameManager* GetInstance(void);
	static void CreateGameManagerInst(HINSTANCE hInstance, HWND hMainWnd);
	void ChangeGameState(CGameState *pGameState);
};

