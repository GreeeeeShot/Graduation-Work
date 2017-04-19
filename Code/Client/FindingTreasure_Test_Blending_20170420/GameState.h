#pragma once
#include "GameFramework.h"

class CGameState
{
public:
	CGameState();
	~CGameState();

	virtual void Init(void) {};
	virtual void Destroy(void) {};
	virtual void Update(void) {};
	virtual void Render(void) {};

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return 0;  }
};

class CPlayGameState : public CGameState
{
public:
	bool m_bIsPushed;
	bool m_bIsDigOrInstall;

	CPlayGameState();
	~CPlayGameState();

	void Init(void);
	void Destroy(void);
	void Update(void);
	void Render(void);

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	//void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};