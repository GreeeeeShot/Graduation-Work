#include "stdafx.h"
#include "Player.h"
#include "CameraOperator.h"


CPlayer::CPlayer()
{
	// m_pCamera = NULL;
	//InitCameraOperator();
	m_fAccelerationMagnitude = PLAYER_ACCELERATION_MAGNITUDE;
	m_fVelocityMagnitude = 0.0f;
	m_fDamping = PLAYER_DAMPING_VALUE;
}

CPlayer::~CPlayer()
{
	// if (m_pCamera) delete m_pCamera;

}

void CPlayer::InitCameraOperator(void)
{
	m_CameraOperator.InitCameraOperator(this);
}

void CPlayer::GoForward(float fTimeElapsed)
{
	Moving(0.01f * m_CameraOperator.GetLook());
	m_CameraOperator.MoveCameraOperator(0.01f * m_CameraOperator.GetLook());

	D3DXVECTOR3 d3dxvCross;
	D3DXVec3Cross(&d3dxvCross, &GetLook(), &(m_CameraOperator.GetLook()));
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	float fDot = D3DXVec3Dot(&GetLook(), &(m_CameraOperator.GetLook()));
	float fRotationAngle = D3DXToDegree(acosf(fDot));
	fRotationAngle = d3dxvCross.y > 0.0f ? fRotationAngle : -fRotationAngle;

	RotateWorldY(fRotationAngle, fTimeElapsed);
}

void CPlayer::GoBack(float fTimeElapsed)
{
	Moving(-0.01f * m_CameraOperator.GetLook());
	m_CameraOperator.MoveCameraOperator(-0.01f * m_CameraOperator.GetLook());

	D3DXVECTOR3 d3dxvCross;
	D3DXVec3Cross(&d3dxvCross, &GetLook(), &(-m_CameraOperator.GetLook()));
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	float fDot = D3DXVec3Dot(&GetLook(), &(-m_CameraOperator.GetLook()));
	float fRotationAngle = D3DXToDegree(acosf(fDot));
	fRotationAngle = d3dxvCross.y > 0.0f ? fRotationAngle : -fRotationAngle;

	RotateWorldY(fRotationAngle, fTimeElapsed);
}

void CPlayer::GoRight(float fTimeElapsed)
{
	Moving(0.01f * m_CameraOperator.GetRight());
	m_CameraOperator.MoveCameraOperator(0.01f * m_CameraOperator.GetRight());

	D3DXVECTOR3 d3dxvCross;
	D3DXVec3Cross(&d3dxvCross, &GetLook(), &(m_CameraOperator.GetRight()));
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	float fDot = D3DXVec3Dot(&GetLook(), &(m_CameraOperator.GetRight()));
	float fRotationAngle = D3DXToDegree(acosf(fDot));
	fRotationAngle = d3dxvCross.y > 0.0f ? fRotationAngle : -fRotationAngle;

	RotateWorldY(fRotationAngle, fTimeElapsed);
}

void CPlayer::GoLeft(float fTimeElapsed)
{
	Moving(-0.01f * m_CameraOperator.GetRight());
	m_CameraOperator.MoveCameraOperator(-0.01f * m_CameraOperator.GetRight());

	D3DXVECTOR3 d3dxvCross;
	D3DXVec3Cross(&d3dxvCross, &GetLook(), &(-m_CameraOperator.GetRight()));
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	float fDot = D3DXVec3Dot(&GetLook(), &(-m_CameraOperator.GetRight()));
	float fRotationAngle = D3DXToDegree(acosf(fDot));
	fRotationAngle = d3dxvCross.y > 0.0f ? fRotationAngle : -fRotationAngle;

	RotateWorldY(fRotationAngle, fTimeElapsed);
}

void CPlayer::MovingByCamera(D3DXVECTOR3 d3dxvDirSum, float fTimeElapsed)
{
	if (-0.01 < d3dxvDirSum.x && d3dxvDirSum.x  <0.01f && -0.01 < d3dxvDirSum.y && d3dxvDirSum.y  <0.01f && -0.01 < d3dxvDirSum.z && d3dxvDirSum.z  <0.01f) return;

	D3DXVECTOR3 d3dxvCross;
	D3DXVec3Cross(&d3dxvCross, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &(m_CameraOperator.GetLook()));
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	float fDot = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &(m_CameraOperator.GetLook()));
	float fRotationAngle = D3DXToDegree(acosf(fDot));	
	fRotationAngle = d3dxvCross.y > 0.0f ? fRotationAngle : -fRotationAngle;  // Degree
	
	D3DXMATRIX d3dxmtxRotation;
	D3DXMatrixRotationAxis(&d3dxmtxRotation, &d3dxvCross, D3DXToRadian(fRotationAngle));

	D3DXVec3TransformNormal(&d3dxvDirSum, &d3dxvDirSum, &d3dxmtxRotation);

	D3DXVec3Cross(&d3dxvCross, &GetLook(), &d3dxvDirSum);
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	fDot = D3DXVec3Dot(&GetLook(), &d3dxvDirSum);
	fRotationAngle = D3DXToDegree(acosf(fDot));
	fRotationAngle = d3dxvCross.y > 0.0f ? fRotationAngle : -fRotationAngle;  // Degree

	// RotateWorldY(fRotationAngle, fTimeElapsed);
	Moving(0.01f * d3dxvDirSum);
	m_CameraOperator.MoveCameraOperator(0.01f * d3dxvDirSum);
}

void CPlayer::RotateWorldY(float fAngle, float fTimeElapsed)
{
	D3DXMATRIX mtxRotate;
	float fRotationDegree = fAngle*fTimeElapsed;

	//플레이어의 로컬 y-축을 기준으로 회전하는 행렬을 생성한다.
	D3DXMatrixRotationAxis(&mtxRotate, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToRadian(fRotationDegree));
	D3DXMatrixMultiply(&m_d3dxmtxWorld, &mtxRotate, &m_d3dxmtxWorld);
}

void CPlayer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_CameraOperator.CreateShaderVariables(pd3dDevice);
}

void CPlayer::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	//플레이어의 현재 카메라의 UpdateShaderVariables() 멤버 함수를 호출한다.
	// if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dDeviceContext);
	m_CameraOperator.UpdateShaderVariables(pd3dDeviceContext);
}
