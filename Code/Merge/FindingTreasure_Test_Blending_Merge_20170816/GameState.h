#pragma once
#include "GameFramework.h"

class CButton
{
public:
	CButton();
	CButton(float fLeft, float fTop, float fRight, float fBottom);
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

	static HBITMAP m_hTitleBitmap;

	static HBITMAP m_hTempWaitingRoomBitmap;
	static HBITMAP m_hLeftAndRightArrowBitmap, m_hReadyBitmap, hBit1, m_hIPInputBitmap, m_hWaitingRoomBitmap;
	static HBITMAP m_hSelectedTeamFrameBitmap[3], m_hCharacterBitmap[2];

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

class CTitleState : public CGameState
{
public:
	const CButton m_sButtonMakingWaitingRoom = CButton(468.0f, 367.0f, 750.0f, 438.0f);
	const CButton m_sButtonJoinInWaitingRoom = CButton(468.0f, 454.0f, 750.0f, 515.0f);
	const CButton m_sButtonOption = CButton(468.0f, 529.0f, 750.0f, 584.0f);
	const CButton m_sButtonExit = CButton(468.0f, 613.0f, 750.0f, 677.0f);

	CTitleState();
	~CTitleState();

	void Init(void);
	void Destroy(void);
	void Update(void);
	void Render(void);
	void Render(HDC hdc);

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

class CWaitingRoomState : public CGameState
{
	SWaitingRoomInformDesc* m_pWaitingRoomInformDesc;

	const CButton m_sButtonLeftArrow = CButton(10.0f, 240.0f, 90.0f, 280.0f);
	const CButton m_sButtonRightArrow = CButton(290.0f, 240.0f, 370.0f, 280.0f);
	const CButton m_sButtonTeamFrame = CButton(50.0f, 40.0f, 330.0f, 458.0f);
	const CButton m_sButtonReadyAndStart = CButton(357.0f, 514.0f, 904.0f, 661.0f);
	const CButton m_sButtonFakie = CButton(10.0f, 11.0f, 60.0f, 65.0f);

	const POINT m_sStartTeamPos[7] =
	{
		380, 70, 
		550, 70, 
		720, 70,
		340, 300, 
		480, 300, 
		620, 300, 
		760, 300
	};

	const POINT m_sStartCharaterPos[7] =
	{
		399, 140,
		569, 140,
		739, 140,
		359, 370,
		499, 370,
		639, 370,
		779, 370
	};

	const POINT m_sReadyPos[7] =
	{
		405, 175,
		575, 175,
		745, 175,
		365, 405,
		505, 405,
		645, 405,
		785, 405
	};
public:
	CWaitingRoomState();
	~CWaitingRoomState();

	void Init(void);
	void Destroy(void);
	void Update(void);
	void Render(void);
	void Render(HDC hdc);

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

class CIPInputState : public CGameState
{
public:
	const CButton m_sButtonFakie = CButton(10.0f, 11.0f, 60.0f, 65.0f);

public:
	CIPInputState();
	~CIPInputState();

	void Init();
	void Destroy(void);
	void Update(void);
	void Render(void);
	void Render(HDC hdc);

	////윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

class CTempWaitingRoomState : public CGameState
{
public:
	const CButton m_sJungleButton = CButton(0.0f,360.0f , 640.0f, 740.0f);
	const CButton m_sSnowButton = CButton(640.0f,360.0f , 1280.0f, 740.0f);
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

	////윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	//void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};