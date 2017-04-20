#pragma once
#include "GameFramework.h"

class CGameState
{
public:
	static HBITMAP m_hMemBitmap;
	static HBITMAP m_hIntroBackBitmap;
	static HBITMAP m_hIntroCompanyBitmap;

	static HINSTANCE m_hInstance;
	static HWND m_hwnd;

public:
	CGameState();
	~CGameState();

	static void CreateAPIBitmapResource(HINSTANCE hInstance, HWND hwnd);

	virtual void Init(void) {};
	virtual void Destroy(void) {};
	virtual void Update(void) {};
	virtual void Render(void) {};
	virtual void Render(HDC hdc) {};

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return 0;  }
};

class CPlayGameState : public CGameState
{
public:
	//bool m_bIsPushed;
	//bool m_bIsDigOrInstall;

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

class CIntroState : public CGameState
{
public:
	//bool m_bIsPushed;
	//bool m_bIsDigOrInstall;
	clock_t m_startClock, m_endClock;
	BLENDFUNCTION m_BlendFunction;
	int m_iAlpha;

	CIntroState();
	~CIntroState();

	void Init(void);
	void Destroy(void);
	void Update(void);
	void Render(void);
	void Render(HDC hdc);

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};