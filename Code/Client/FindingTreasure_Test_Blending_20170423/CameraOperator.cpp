#include "stdafx.h"
#include "CameraOperator.h"


CCameraOperator::CCameraOperator()
{
	m_pObject = NULL;
	// ī�޶� ���۱� ��ġ ��ǥ
	m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// ī�޶� ��ġ ���� ; ī�޶� ���۱� ��ǥ�� �������� ī�޶���� ���� ����
	D3DXVec3Normalize(&m_d3dxvCameraDir, &D3DXVECTOR3(0.0f, 35.0f, -45.0f)); 
	// �Ÿ� ��Į�� �� ; ī�޶� ���۱� ��ǥ�� ������������ ī�޶������ �Ÿ�
	m_fDistance = CAMERA_DISTANCE;
	// ī�޶� ���� ��ġ ��ǥ
	m_d3dxvCameraPos = m_fDistance * m_d3dxvCameraDir;
	// �� ���� ����
	m_d3dxvZoomDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// ī�޶� ���۱��� ���� ��ǥ��
	m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_d3dxvLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}


CCameraOperator::~CCameraOperator()
{
}

void CCameraOperator::InitCameraOperator(CGameObject* pObject)						// ��� �������� �ʱ�ȭ�Ѵ�.
{
	// ī�޶� ���۱� ��ġ ��ǥ
	if (pObject)
	{
		m_pObject = pObject;
		m_d3dxvPosition = pObject->GetPosition();
	}

	// ī�޶� ��ġ ���� ; ī�޶� ���۱� ��ǥ�� �������� ī�޶���� ���� ����
	D3DXVec3Normalize(&m_d3dxvCameraDir, &D3DXVECTOR3(0.0f, 30.0f, -45.0f));
	// �Ÿ� ��Į�� �� ; ī�޶� ���۱� ��ǥ�� ������������ ī�޶������ �Ÿ�
	m_fDistance = CAMERA_DISTANCE;
	// ī�޶� �ʴ� ȸ�� ����
	// ī�޶� ���� ��ġ ��ǥ
	m_d3dxvCameraPos = m_fDistance * m_d3dxvCameraDir;
	// �� ���� ����
	m_d3dxvZoomDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	// ī�޶� ���۱��� ���� ��ǥ��
	m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_d3dxvLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_fAngleX = D3DXToDegree(acosf(D3DXVec3Dot(&m_d3dxvCameraDir, &D3DXVECTOR3(0.0f, 0.0f, -1.0f))));
}

void CCameraOperator::SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ)
{
	m_Camera.SetViewport(pd3dDeviceContext, xTopLeft, yTopLeft, nWidth, nHeight, fMinZ, fMaxZ);
}

void CCameraOperator::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_Camera.GenerateProjectionMatrix(fNearPlaneDistance, fFarPlaneDistance, fAspectRatio, fFOVAngle);
}

void CCameraOperator::MoveCameraOperator(const D3DXVECTOR3& d3dxvShift)		// ī�޶� ���۱⸦ �� ��ġ���� d3dxvShift��ŭ �̵���Ų��.
{
	m_d3dxvPosition += d3dxvShift;
}

void CCameraOperator::RotateLocalY(float fRotationDegreePerSec, float fTimeElapsed)					// m_d3dxvCameraDir�� ���� Y�࿡ ���Ͽ� ȸ���Ѵ�.
{
	D3DXMATRIX mtxRotate;
	float fRotationDegree = fRotationDegreePerSec * fTimeElapsed;

	//�÷��̾��� ���� y-���� �������� ȸ���ϴ� ����� �����Ѵ�.
	D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvUp, (float)D3DXToRadian(fRotationDegree));
	// ī�޶� ���� ���͸� ȸ����Ų��. 
	D3DXVec3TransformNormal(&m_d3dxvCameraDir, &m_d3dxvCameraDir, &mtxRotate);
	D3DXVec3Normalize(&m_d3dxvCameraDir, &m_d3dxvCameraDir);
	//ī�޶��� ���� x-��, y-��, z-���� ȸ���Ѵ�.
	D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
	D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
	D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);

	/*D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
	D3DXVec3Normalize(&m_d3dxvUp, &m_d3dxvUp);
	D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);*/
}

void CCameraOperator::RotateLocalX(float fRotationDegreePerSec, float fTimeElapsed)					// m_d3dxvCameraDir�� ���� Z�࿡ ���Ͽ� ȸ���Ѵ�.
{
	D3DXMATRIX mtxRotate;
	D3DXVECTOR3 d3dxvProjCameraDirToXZ;

	float fRotationDegree = fRotationDegreePerSec * fTimeElapsed;

	if (DEGREE_X_LOWER_LIMIT < m_fAngleX && m_fAngleX < DEGREE_X_UPPER_LIMIT)
	{
		D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRight, (float)D3DXToRadian(fRotationDegree));
		D3DXVec3TransformNormal(&m_d3dxvCameraDir, &m_d3dxvCameraDir, &mtxRotate);
		D3DXVec3Normalize(&m_d3dxvCameraDir, &m_d3dxvCameraDir);
		d3dxvProjCameraDirToXZ = D3DXVECTOR3(m_d3dxvCameraDir.x, 0.0, m_d3dxvCameraDir.z);
		D3DXVec3Normalize(&d3dxvProjCameraDirToXZ, &d3dxvProjCameraDirToXZ);
		float fDot = D3DXVec3Dot(&m_d3dxvCameraDir, &d3dxvProjCameraDirToXZ);
		if (fDot >= 1.0f) fDot = 1.0f;
		else if (fDot <= -1.0f) fDot = -1.0f;
		m_fAngleX = D3DXToDegree(acosf(fDot));
	}	
	
	if(DEGREE_X_LOWER_LIMIT > m_fAngleX || m_fAngleX > DEGREE_X_UPPER_LIMIT)
	{
		//�÷��̾��� ���� y-���� �������� ȸ���ϴ� ����� �����Ѵ�.
		D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRight, (float)D3DXToRadian(-1.2*fRotationDegree));
		D3DXVec3TransformNormal(&m_d3dxvCameraDir, &m_d3dxvCameraDir, &mtxRotate);
		D3DXVec3Normalize(&m_d3dxvCameraDir, &m_d3dxvCameraDir);
		float fDot = D3DXVec3Dot(&m_d3dxvCameraDir, &d3dxvProjCameraDirToXZ);
		if (fDot >= 1.0f) fDot = 1.0f;
		else if (fDot <= -1.0f) fDot = -1.0f;
		m_fAngleX = D3DXToDegree(acosf(fDot));
	}
}

void CCameraOperator::ZoomInAtOnce(float distance)						// �ٶ󺸰� �ִ� ������ ���������.
{
	D3DXVec3Normalize(&m_d3dxvZoomDir, &m_d3dxvZoomDir);
	D3DXVec3Normalize(&m_d3dxvZoomDir, &D3DXVECTOR3(m_pObject ? m_pObject->GetPosition() - (m_d3dxvCameraDir * m_fDistance + m_d3dxvPosition) : m_d3dxvPosition - (m_d3dxvCameraDir * m_fDistance + m_d3dxvPosition)));
	m_d3dxvZoomDir *= distance;
}

void CCameraOperator::ZoomOutAtOnce(float distance)						// �ٶ󺸰� �ִ� �����κ��� �־�����.
{
	D3DXVec3Normalize(&m_d3dxvZoomDir, &m_d3dxvZoomDir);
	D3DXVec3Normalize(&m_d3dxvZoomDir, &D3DXVECTOR3(m_pObject ? m_pObject->GetPosition() - (m_d3dxvCameraDir * m_fDistance + m_d3dxvPosition) : m_d3dxvPosition - (m_d3dxvCameraDir * m_fDistance + m_d3dxvPosition)));
	m_d3dxvZoomDir *= -distance;
}

void CCameraOperator::OriginalZoomState(void)
{
	m_d3dxvZoomDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void CCameraOperator::ProofreadLocalAxis(void)
{
	D3DXVECTOR3 d3dxvNormalizedAxis;

	D3DXVec3Normalize(&d3dxvNormalizedAxis, &GetLook());
	m_d3dxvLook = d3dxvNormalizedAxis;
	m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVec3Cross(&d3dxvNormalizedAxis, &GetUp(), &GetLook());
	D3DXVec3Normalize(&d3dxvNormalizedAxis, &d3dxvNormalizedAxis);
	m_d3dxvRight = d3dxvNormalizedAxis;
}

void CCameraOperator::CreateShaderVariables(ID3D11Device *pd3dDevice)				// ī�޶�� �����Ͽ� ���̴��� ������ �ڿ� ������ �����Ѵ�.
{
	m_Camera.CreateShaderVariables(pd3dDevice);
}

void CCameraOperator::GenerateViewMatrix(float fTimeElapsed, bool bSoftMoving)
{
	D3DXVECTOR3 d3dxvEyePosition;
	D3DXVECTOR3 d3dxvLookAt;

	/*printf("dot : xy, xz, zx : (%f, %f, %f)\n",
		D3DXVec3Dot(&m_d3dxvUp, &m_d3dxvRight),
		D3DXVec3Dot(&m_d3dxvUp, &m_d3dxvLook),
		D3DXVec3Dot(&m_d3dxvLook, &m_d3dxvRight));
	printf("Player Position : (%f, %f, %f)",
		m_pObject->GetPosition().x,
		m_pObject->GetPosition().y,
		m_pObject->GetPosition().z);
*/
	if (bSoftMoving)
	{
		D3DXVECTOR3 d3dxvToUpdatedDir = (m_d3dxvCameraDir * m_fDistance + m_d3dxvPosition + m_d3dxvZoomDir) - m_d3dxvCameraPos;
		
		m_d3dxvCameraPos += d3dxvToUpdatedDir / (float(CAMERA_SOFT_MOVING_SPLIT_FRACTION) * (1/fTimeElapsed));
	}
	else m_d3dxvCameraPos = m_d3dxvPosition + m_d3dxvCameraDir * m_fDistance + m_d3dxvZoomDir;
	/*printf("Camera Position : (%f, %f, %f) \n", m_d3dxvCameraPos.x, m_d3dxvCameraPos.y, m_d3dxvCameraPos.z);
	printf("CameraOperator Position : (%f, %f, %f) \n", m_d3dxvPosition.x, m_d3dxvPosition.y, m_d3dxvPosition.z);*/
	
	d3dxvLookAt = m_pObject ? m_pObject->GetPosition() : m_d3dxvPosition;

	m_Camera.GenerateViewMatrix(m_d3dxvCameraPos, d3dxvLookAt, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	//ī�޶��� ��ġ�� ������ �ٲ��(ī�޶� ��ȯ ����� �ٲ��) ����ü ����� �ٽ� ����Ѵ�.
	m_Camera.CalculateFrustumPlanes();
}

void CCameraOperator::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)	// ī�޶� ��� ���۸� �����Ѵ�.
{
	m_Camera.UpdateShaderVariables(pd3dDeviceContext);
}