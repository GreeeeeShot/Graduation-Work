#pragma once
#include "Object.h"		// CPlayer�� Object�� �����̴�.
//#include "Camera.h"		// CPlayer�� Camera�� �����Ѵ�.
#include "CameraOperator.h"		// 20170311 ī�޶� ���۱��̴�.

class CPlayer : public CGameObject
{
public:
	//CCamera *m_pCamera;
	// �÷��̾�� ī�޶� ���۱⸦ ���� ī�޶� �ʱ�ȭ�ϰ� ������ �� �ְ� �ȴ�.
	CCameraOperator m_CameraOperator;

public:
	CPlayer();
	virtual ~CPlayer();

	//�÷��̾��� ���� ī�޶� �����ϰ� ��ȯ�ϴ� ��� �Լ��� �����Ѵ�.
	//void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }
	//CCamera *GetCamera() { return(m_pCamera); }
	void InitCameraOperator(void);

	//�÷��̾��� ��� ���۸� �����ϰ� �����ϴ� ��� �Լ��� �����Ѵ�.
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
};



