#include "stdafx.h"
#include "Player.h"
#include "CameraOperator.h"
#include "PhysicalCollision.h"


CPlayer::CPlayer()
{
	// m_pCamera = NULL;
	//InitCameraOperator();
	m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
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

	/*D3DXVECTOR3 d3dxvCross;
	D3DXVec3Cross(&d3dxvCross, &GetLook(), &(m_CameraOperator.GetLook()));
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	float fDot = D3DXVec3Dot(&GetLook(), &(m_CameraOperator.GetLook()));
	float fRotationAngle = D3DXToDegree(acosf(fDot));
	fRotationAngle = d3dxvCross.y > 0.0f ? fRotationAngle : -fRotationAngle;

	RotateWorldY(fRotationAngle, fTimeElapsed);*/
}

void CPlayer::GoBack(float fTimeElapsed)
{
	Moving(-0.01f * m_CameraOperator.GetLook());
	m_CameraOperator.MoveCameraOperator(-0.01f * m_CameraOperator.GetLook());
}

void CPlayer::GoRight(float fTimeElapsed)
{
	Moving(0.01f * m_CameraOperator.GetRight());
	m_CameraOperator.MoveCameraOperator(0.01f * m_CameraOperator.GetRight());
}

void CPlayer::GoLeft(float fTimeElapsed)
{
	Moving(-0.01f * m_CameraOperator.GetRight());
	m_CameraOperator.MoveCameraOperator(-0.01f * m_CameraOperator.GetRight());
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

void CPlayer::RotateToMovingDir(D3DXVECTOR3 d3dxvMovingDir)
{
	if ((-0.01f < d3dxvMovingDir.x && d3dxvMovingDir.x < 0.01f) && (-0.01f < d3dxvMovingDir.y && d3dxvMovingDir.y < 0.01f) && (-0.01f < d3dxvMovingDir.z && d3dxvMovingDir.z < 0.01f)) return;
	/*printf("Moving Dir : (%lf, %lf, %lf) \n", d3dxvMovingDir.x, d3dxvMovingDir.y, d3dxvMovingDir.z);*/

	D3DXVec3Normalize(&d3dxvMovingDir, &d3dxvMovingDir);
	D3DXMATRIX mtxRotate;

	// 회전축을 구한다.
	D3DXVECTOR3 d3dxvCross;
	D3DXVec3Cross(&d3dxvCross, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &m_CameraOperator.GetLook());
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	// 회전량을 구한다.
	float fDot = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &m_CameraOperator.GetLook());
	float fRotationRadianForProofreading = acosf(fDot);
	fRotationRadianForProofreading = d3dxvCross.y > 0.0f ? fRotationRadianForProofreading : -fRotationRadianForProofreading;

	D3DXMatrixRotationAxis(&mtxRotate, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), fRotationRadianForProofreading);

	D3DXVec3TransformNormal(&d3dxvMovingDir, &d3dxvMovingDir, &mtxRotate);
	D3DXVec3Cross(&d3dxvCross, &GetLook(), &d3dxvMovingDir);
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	fDot = D3DXVec3Dot(&GetLook(), &d3dxvMovingDir);
	fRotationRadianForProofreading = acosf(fDot);
	fRotationRadianForProofreading = d3dxvCross.y > 0.0f ? fRotationRadianForProofreading : -fRotationRadianForProofreading;
	fRotationRadianForProofreading = D3DXToRadian(D3DXToDegree(fRotationRadianForProofreading) / 100.0f);
	
	D3DXMatrixRotationAxis(&mtxRotate, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), fRotationRadianForProofreading);
	/*printf("Prev Player Position : (%lf, %lf, %lf) \n", GetPosition().x, GetPosition().y, GetPosition().z);*/
	D3DXMatrixMultiply(&m_d3dxmtxWorld, &mtxRotate, &m_d3dxmtxWorld);
	/*printf("Post Player Position : (%lf, %lf, %lf) \n", GetPosition().x, GetPosition().y, GetPosition().z);
	printf("World Position : (%lf, %lf, %lf) \n", m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43);
	printf("_________________________________________ \n");*/
}

void CPlayer::MovingUnderPhysicalEnvironment(CScene *pScene, CVoxelTerrain *pVoxelTerrain, float fTimeElapsed)
{
	static D3DXVECTOR3 d3dxvSearchDir[18] =
	{
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3(1.0f, -1.0f, -1.0f),	D3DXVECTOR3(0.0f, -1.0f, -1.0f),	D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
		D3DXVECTOR3(1.0f, -1.0f, 0.0f),		D3DXVECTOR3(0.0f, -1.0f, 0.0f),		D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(1.0f, -1.0f, 1.0f),		D3DXVECTOR3(0.0f, -1.0f, 1.0f),		D3DXVECTOR3(-1.0f, -1.0f, 1.0f),

		D3DXVECTOR3(1.0f, 0.0f, -1.0f),		D3DXVECTOR3(0.0f, 0.0f, -1.0f),		D3DXVECTOR3(-1.0f, 0.0f, -1.0f),
		D3DXVECTOR3(1.0f, 0.0f, 0.0f),											D3DXVECTOR3(-1.0f, 0.0f, 0.0f),
		D3DXVECTOR3(1.0f, 0.0f, 1.0f),		D3DXVECTOR3(0.0f, 0.0f, 1.0f),		D3DXVECTOR3(-1.0f, 0.0f, 1.0f),
	};

	// 세계의 중력을 받는다.
	D3DXVECTOR3 d3dxvPostV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 d3dxvG = D3DXVECTOR3(0.0f, GRAVITATIONAL_ACCELERATION, 0.0f);
	D3DXVECTOR3 d3dxvMovingDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	d3dxvPostV = d3dxvG * fTimeElapsed + m_d3dxvVelocity;
	d3dxvMovingDir = ((d3dxvPostV + m_d3dxvVelocity) * fTimeElapsed) / 2.0f;
	m_d3dxvVelocity = d3dxvPostV;
	Moving(d3dxvMovingDir);
	m_CameraOperator.MoveCameraOperator(d3dxvMovingDir);

	// 복셀 지형맵과의 충돌 체크를 한다.
	D3DXMATRIX d3dxmtxFromPosToIdx;
	D3DXVECTOR4 d3dxvColLayerRow;

	D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &pVoxelTerrain->m_d3dxmtxFromIdxToPos);
	D3DXVec3Transform(&d3dxvColLayerRow, &GetPosition(), &d3dxmtxFromPosToIdx);

	d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
	d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
	d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);

	CVoxel *pCollidedVoxel;
	D3DXVECTOR3 d3dxvInspectionIdx;

	for (int i = 0; i < 18; i++)
	{
		d3dxvInspectionIdx.x = d3dxvColLayerRow.x + d3dxvSearchDir[i].x;
		d3dxvInspectionIdx.y = d3dxvColLayerRow.y + d3dxvSearchDir[i].y;
		d3dxvInspectionIdx.z = d3dxvColLayerRow.z + d3dxvSearchDir[i].z;

		if ((0 <= d3dxvInspectionIdx.x && d3dxvInspectionIdx.x < pVoxelTerrain->m_iMaxCol)
			&& (0 <= d3dxvInspectionIdx.y && d3dxvInspectionIdx.y < pVoxelTerrain->m_iMaxLayer)
			&& (0 <= d3dxvInspectionIdx.z && d3dxvInspectionIdx.z < pVoxelTerrain->m_iMaxRow))
		{
			if ((pCollidedVoxel = pVoxelTerrain->m_ppVoxelObjectsForCollision[
				(int)((d3dxvColLayerRow.y + d3dxvSearchDir[i].y) * pVoxelTerrain->m_iLayerStride
					+ (d3dxvColLayerRow.z + d3dxvSearchDir[i].z) * pVoxelTerrain->m_iRowStride
					+ (d3dxvColLayerRow.x + d3dxvSearchDir[i].x))]) != NULL)
			{

				if (CPhysicalCollision::IsCollided(
					&CPhysicalCollision::MoveAABB(this->m_pMesh->m_AABB, GetPosition()),
					&CPhysicalCollision::MoveAABB(pCollidedVoxel->m_pMesh->m_AABB, pCollidedVoxel->GetPosition())))
				{
					D3DXVECTOR3 d3dxvUp(0.0f, 0.0f, 0.0f);
					d3dxvUp.y = ((pCollidedVoxel->GetPosition().y + pVoxelTerrain->m_fCubeHeight / 2.0f)
						- (GetPosition().y + m_pMesh->m_AABB.m_d3dxvMin.y));
					Moving(d3dxvUp);
					m_CameraOperator.MoveCameraOperator(d3dxvUp);
					m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				}
			}
		}
	}
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
