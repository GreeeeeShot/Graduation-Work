#pragma once
#include "GameFramework.h"

class CButton
{
public:
	CButton();
	CButton(float, float, float, float);
	~CButton();

	float m_fTop;
	float m_fLeft;
	float m_fRight;
	float m_fBottom;

	bool IsInButton(float fX, float fY) const;
};

class CGameState
{
public:
	static HBITMAP m_hMemBitmap;
	static HBITMAP m_hIntroBackBitmap;
	static HBITMAP m_hIntroCompanyBitmap;
	static HBITMAP m_hTempWaitingRoomBitmap;
	static HBITMAP charaSelectbmp, readybmp, hBit1, hipInputbmp,hwaitingbmp , htitle;
	static HBITMAP SelectTeambmp[3], charabmp[2];

	//static HBITMAP piratebmp, cowgirlcharabmp, blueTeambmp, redTeambmp, nonteam;
	static HINSTANCE m_hInstance;
	static HWND m_hwnd;

public:
	CGameState();
	~CGameState();

	static void CreateAPIBitmapResource(HINSTANCE hInstance, HWND hwnd);

	virtual void Init() {};
	virtual void Destroy(void) {};
	virtual void Update(void) {};
	virtual void Render(void) {};
	virtual void Render(HDC hdc) {};
	virtual void HandOverExternalInput(void* pExternalInput) {};

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return 0;  }
};

class CPlayGameState : public CGameState
{
public:
	//bool m_bIsPushed;
	//bool m_bIsDigOrInstall;
	SWaitingRoomInformDesc* m_pWaitingRoomInformDesc;

	CPlayGameState();
	~CPlayGameState();

	void Init(void);
	void Destroy(void);
	void Update(void);
	void Render(void);
	void HandOverExternalInput(void* pExternalInput);

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	//void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
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

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

class CTempWaitingRoomState : public CGameState
{
public:
	const CButton m_sJungleButton = CButton(360.0f, 0.0f, 640.0f, 740.0f);
	const CButton m_sSnowButton = CButton(360.0f, 640.0f, 1280.0f, 740.0f);
	SWaitingRoomInformDesc* m_pWaitingRoomInformDesc;
	//(0.0f, 360.0f, 640.0f, 1280.0f);
	//CButton m_sSnowButton(0.0f, 360.0f, 640.0f, 1280.0f);

public :
	CTempWaitingRoomState();
	~CTempWaitingRoomState();

	void Init();
	void Destroy(void);
	void Update(void);
	void Render(void);
	void Render(HDC hdc); 

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};