#pragma once
#include "Object.h"		// CPlayer는 Object의 일종이다.
//#include "Camera.h"		// CPlayer는 Camera를 소지한다.
#include "CameraOperator.h"		// 20170311 카메라 조작기이다.

class CPlayer : public CGameObject
{
public:
	//CCamera *m_pCamera;
	// 플레이어는 카메라 조작기를 통해 카메라를 초기화하고 조작할 수 있게 된다.
	CCameraOperator m_CameraOperator;

public:
	CPlayer();
	virtual ~CPlayer();

	//플레이어의 현재 카메라를 설정하고 반환하는 멤버 함수를 선언한다.
	//void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }
	//CCamera *GetCamera() { return(m_pCamera); }
	void InitCameraOperator(void);

	//플레이어의 상수 버퍼를 생성하고 갱신하는 멤버 함수를 선언한다.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
};



