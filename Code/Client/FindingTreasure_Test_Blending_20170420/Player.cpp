#include "stdafx.h"
#include "Player.h"
#include "CameraOperator.h"
#include "PhysicalCollision.h"


CPlayer::CPlayer()
{
	// m_pCamera = NULL;
	//InitCameraOperator();
	m_d3dxvMoveDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fFriction = PLAYER_FRICTION;

	// m_pHavingVoxel = NULL;
	// m_bIsHavingVoxel = false;
	m_iVoxelPocketSlotIdx = 0;
}

CPlayer::~CPlayer()
{
	// if (m_pCamera) delete m_pCamera;

}

void CPlayer::InitCameraOperator(void)
{
	m_CameraOperator.InitCameraOperator(this);
}

//void CPlayer::GoForward(float fTimeElapsed)
//{
//	Moving(0.01f * m_CameraOperator.GetLook());
//	m_CameraOperator.MoveCameraOperator(0.01f * m_CameraOperator.GetLook());
//}
//
//void CPlayer::GoBack(float fTimeElapsed)
//{
//	Moving(-0.01f * m_CameraOperator.GetLook());
//	m_CameraOperator.MoveCameraOperator(-0.01f * m_CameraOperator.GetLook());
//}
//
//void CPlayer::GoRight(float fTimeElapsed)
//{
//	Moving(0.01f * m_CameraOperator.GetRight());
//	m_CameraOperator.MoveCameraOperator(0.01f * m_CameraOperator.GetRight());
//}
//
//void CPlayer::GoLeft(float fTimeElapsed)
//{
//	Moving(-0.01f * m_CameraOperator.GetRight());
//	m_CameraOperator.MoveCameraOperator(-0.01f * m_CameraOperator.GetRight());
//}

//void CPlayer::MovingByCamera(D3DXVECTOR3 d3dxvDirSum, float fTimeElapsed)
//{
//	if (-0.01 < d3dxvDirSum.x && d3dxvDirSum.x  <0.01f && -0.01 < d3dxvDirSum.y && d3dxvDirSum.y  <0.01f && -0.01 < d3dxvDirSum.z && d3dxvDirSum.z  <0.01f) return;
//
//	D3DXVECTOR3 d3dxvCross;
//	D3DXVec3Cross(&d3dxvCross, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &(m_CameraOperator.GetLook()));
//	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);
//
//	float fDot = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &(m_CameraOperator.GetLook()));
//	float fRotationAngle = D3DXToDegree(acosf(fDot));	
//	fRotationAngle = d3dxvCross.y > 0.0f ? fRotationAngle : -fRotationAngle;  // Degree
//	
//	D3DXMATRIX d3dxmtxRotation;
//	D3DXMatrixRotationAxis(&d3dxmtxRotation, &d3dxvCross, D3DXToRadian(fRotationAngle));
//
//	D3DXVec3TransformNormal(&d3dxvDirSum, &d3dxvDirSum, &d3dxmtxRotation);
//
//	D3DXVec3Cross(&d3dxvCross, &GetLook(), &d3dxvDirSum);
//	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);
//
//	fDot = D3DXVec3Dot(&GetLook(), &d3dxvDirSum);
//	fRotationAngle = D3DXToDegree(acosf(fDot));
//	fRotationAngle = d3dxvCross.y > 0.0f ? fRotationAngle : -fRotationAngle;  // Degree
//
//	// RotateWorldY(fRotationAngle, fTimeElapsed);
//	Moving(0.01f * d3dxvDirSum);
//	m_CameraOperator.MoveCameraOperator(0.01f * d3dxvDirSum);
//}

void CPlayer::DigInVoxelTerrain(CVoxelTerrain *pVoxelTerrain, bool bIsDigging, float fTimeElapsed)
{
#define DIG_COMPLETE_TIME	1.0f

	static float fDigTime = 0.0f;
	static int iLinearIdx = -1;
	static int i = 0;

	static D3DXVECTOR3 d3dxvSearchDir[2] =
	{
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// 자신의 위치
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),					// 위
	};

	if (!bIsDigging)
	{
		fDigTime = 0.0f;
		iLinearIdx = -1;
		i = 0;
		if (m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel && !m_VoxelPocket[m_iVoxelPocketSlotIdx].m_bIsActive) {
			m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetMaterial(CMaterialResource::pStandardMaterial);
			m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel = NULL;
		}
		return;
	}

	if (m_VoxelPocket[m_iVoxelPocketSlotIdx].m_bIsActive) return;

	if (!m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel)			// 가지고 있는 복셀이 없다면, 가질 복셀을 찾는다.
	{
		D3DXVECTOR3 d3dxvSearchPoint;

		D3DXVec3Normalize(&d3dxvSearchPoint, &GetLook());
		d3dxvSearchPoint *= 0.65f;
		d3dxvSearchPoint += GetPosition();
		d3dxvSearchPoint.y -= 0.655f;
		// 복셀 지형맵과의 충돌 체크를 한다.
		D3DXMATRIX d3dxmtxFromPosToIdx;
		D3DXVECTOR4 d3dxvColLayerRow;

		D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
		D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &pVoxelTerrain->m_d3dxmtxFromIdxToPos);
		D3DXVec3Transform(&d3dxvColLayerRow, &GetPosition(), &d3dxmtxFromPosToIdx);

		CVoxel *pCollidedVoxel;
		D3DXVECTOR3 d3dxvInspectionIdx;			// 콜리젼 박스에 설치될 인덱스 좌표
		//D3DXVECTOR3 d3dxvInstallPos;			// 복셀이 설치될 위치 좌표

		// 탐색 지점을 인덱스화 한다.
		D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvSearchPoint, &d3dxmtxFromPosToIdx);

		d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
		d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
		d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);

		//if ((int)d3dxvColLayerRow.y == iPlayerIdxY - 1)
		//{
		printf("candidate!\n");
		for (i = 0; i < 2; i++)
		{
			d3dxvInspectionIdx.x = d3dxvColLayerRow.x + d3dxvSearchDir[i].x;
			d3dxvInspectionIdx.y = d3dxvColLayerRow.y + d3dxvSearchDir[i].y;
			d3dxvInspectionIdx.z = d3dxvColLayerRow.z + d3dxvSearchDir[i].z;

			if ((0 <= d3dxvInspectionIdx.x && d3dxvInspectionIdx.x < pVoxelTerrain->m_iMaxCol)
				&& (0 <= d3dxvInspectionIdx.y && d3dxvInspectionIdx.y < pVoxelTerrain->m_iMaxLayer)
				&& (0 <= d3dxvInspectionIdx.z && d3dxvInspectionIdx.z < pVoxelTerrain->m_iMaxRow))
			{
				// printf("d3dxvColLayerRow : %f %f %f \n", d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z);
				if ((pCollidedVoxel = pVoxelTerrain->m_ppVoxelObjectsForCollision[iLinearIdx =
					(int)((d3dxvInspectionIdx.y) * pVoxelTerrain->m_iLayerStride
						+ (d3dxvInspectionIdx.z) * pVoxelTerrain->m_iRowStride
						+ (d3dxvInspectionIdx.x))]) != NULL)
				{
					if (0 <= d3dxvInspectionIdx.y + 1 && d3dxvInspectionIdx.y + 1 < pVoxelTerrain->m_iMaxLayer)
					{
						if (!pVoxelTerrain->m_ppVoxelObjectsForCollision[
							(int)((d3dxvInspectionIdx.y + 1) * pVoxelTerrain->m_iLayerStride
								+ (d3dxvInspectionIdx.z) * pVoxelTerrain->m_iRowStride
								+ (d3dxvInspectionIdx.x))])
						{
							// 해당 복셀을 소유한다.
							m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel = pCollidedVoxel;
							printf("iLinearIdx : %d ", iLinearIdx);
							// pCollidedVoxel->SetActive(false);
							printf("delete!\n");
							break;
						}
					}
					else
					{
						// 해당 복셀을 소유한다.
						m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel = pCollidedVoxel;
						printf("iLinearIdx : %d ", iLinearIdx);
						// pCollidedVoxel->SetActive(false);
						printf("delete!\n");
						break;
					}
				}
			}
		}
		
		//}
		//else return;
	}
	else		// 복셀을 소유하고 있다면,
	{
		D3DXVECTOR3 d3dxvSearchPoint;
		AABB HavingVoxelAABB = CPhysicalCollision::MoveAABB(m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->m_pMesh->m_AABB, m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->GetPosition());

		D3DXVec3Normalize(&d3dxvSearchPoint, &GetLook());
		d3dxvSearchPoint *= 0.65f;
		d3dxvSearchPoint += (GetPosition() + d3dxvSearchDir[i]);
		d3dxvSearchPoint.y -= 0.655f;

		if (HavingVoxelAABB.m_d3dxvMin.x < d3dxvSearchPoint.x && d3dxvSearchPoint.x < HavingVoxelAABB.m_d3dxvMax.x
			&& HavingVoxelAABB.m_d3dxvMin.y < d3dxvSearchPoint.y && d3dxvSearchPoint.y < HavingVoxelAABB.m_d3dxvMax.y
			&& HavingVoxelAABB.m_d3dxvMin.z < d3dxvSearchPoint.z && d3dxvSearchPoint.z < HavingVoxelAABB.m_d3dxvMax.z)
		{
			fDigTime += fTimeElapsed;
			if (fDigTime >= DIG_COMPLETE_TIME)
			{
				printf("delete iLinearIdx : %d ", iLinearIdx);
				pVoxelTerrain->m_ppVoxelObjectsForCollision[iLinearIdx] = NULL;
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetMaterial(CMaterialResource::pStandardMaterial);
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetVisible(false);
				fDigTime = 0.0f;
				iLinearIdx = -1;
				i = 0;
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_bIsActive = true;
			}
			else if (fDigTime >= DIG_COMPLETE_TIME * 0.66f)
			{
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetMaterial(CMaterialResource::pHalfMoreTransMaterial);
			}
			else if (fDigTime >= DIG_COMPLETE_TIME * 0.33f)
			{
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetMaterial(CMaterialResource::pHalfLessTransMaterial);
			}
		}
	}
}

void CPlayer::InstallVoxel(CVoxelTerrain *pVoxelTerrain, bool bIsInstalling, float fTimeElapsed)
{
#define INSTALL_COMPLETE_TIME	1.0f

	static float fInstallTime = 0.0f;
	static int iLinearIdx = -1;
	static int i = 0;

	static D3DXVECTOR3 d3dxvSearchDir[2] =
	{
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// 자신의 위치
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),					// 위
	};

	if (!bIsInstalling)
	{
		fInstallTime = 0.0f;
		iLinearIdx = -1;
		i = 0;
		if (m_VoxelPocket[m_iVoxelPocketSlotIdx].m_bIsActive) {
			m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetVisible(false);
		}
		return;
	}

	if (!m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->m_bIsVisible)			// 복셀이 설치되지 않았다면. 설치 위치를 찾는다.
	{
		D3DXVECTOR3 d3dxvSearchPoint;

		D3DXVec3Normalize(&d3dxvSearchPoint, &GetLook());
		d3dxvSearchPoint *= 0.65f;
		d3dxvSearchPoint += GetPosition();
		d3dxvSearchPoint.y -= 0.655f;
		// 복셀 지형맵과의 충돌 체크를 한다.
		D3DXMATRIX d3dxmtxFromPosToIdx;
		D3DXVECTOR4 d3dxvColLayerRow;

		D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
		D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &pVoxelTerrain->m_d3dxmtxFromIdxToPos);
		D3DXVec3Transform(&d3dxvColLayerRow, &GetPosition(), &d3dxmtxFromPosToIdx);


		// 자신의 위치를 인덱스화 한다.
		d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
		d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
		d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);

		int iPlayerIdxY = d3dxvColLayerRow.y;

		CVoxel *pCollidedVoxel;
		D3DXVECTOR3 d3dxvInspectionIdx;			// 콜리젼 박스에 설치될 인덱스 좌표
		D3DXVECTOR3 d3dxvInstallPos;			// 복셀이 설치될 위치 좌표

		// 지면에 닿아있는 경우, 
		// 탐색 지점을 인덱스화 한다.
		D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvSearchPoint, &d3dxmtxFromPosToIdx);

		d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
		d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
		d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);

		//if ((int)d3dxvColLayerRow.y == iPlayerIdxY - 1)
		//{
		printf("candidate!\n");
		for (i = 0; i < 2; i++)
		{
			d3dxvInspectionIdx.x = d3dxvColLayerRow.x + d3dxvSearchDir[i].x;
			d3dxvInspectionIdx.y = d3dxvColLayerRow.y + d3dxvSearchDir[i].y;
			d3dxvInspectionIdx.z = d3dxvColLayerRow.z + d3dxvSearchDir[i].z;

			if ((0 <= d3dxvInspectionIdx.x && d3dxvInspectionIdx.x < pVoxelTerrain->m_iMaxCol)
				&& (0 <= d3dxvInspectionIdx.y && d3dxvInspectionIdx.y < pVoxelTerrain->m_iMaxLayer)
				&& (0 <= d3dxvInspectionIdx.z && d3dxvInspectionIdx.z < pVoxelTerrain->m_iMaxRow))
			{
				// printf("d3dxvColLayerRow : %f %f %f \n", d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z);
				if (!pVoxelTerrain->m_ppVoxelObjectsForCollision[iLinearIdx =
					(int)((d3dxvInspectionIdx.y) * pVoxelTerrain->m_iLayerStride
						+ (d3dxvInspectionIdx.z) * pVoxelTerrain->m_iRowStride
						+ (d3dxvInspectionIdx.x))])
				{
					if (0 <= d3dxvInspectionIdx.y + 1&& d3dxvInspectionIdx.y + 1 < pVoxelTerrain->m_iMaxLayer)
					{
						if (!pVoxelTerrain->m_ppVoxelObjectsForCollision[
							(int)((d3dxvInspectionIdx.y + 1) * pVoxelTerrain->m_iLayerStride
								+ (d3dxvInspectionIdx.z) * pVoxelTerrain->m_iRowStride
								+ (d3dxvInspectionIdx.x))])
						{
							// 해당 위치에 가지고 있는 복셀을 설치한다.
							D3DXMATRIX d3dxmtxFromIdxToPos;

							D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvInspectionIdx, &pVoxelTerrain->m_d3dxmtxFromIdxToPos);
							m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetPosition(D3DXVECTOR3(d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z));
							m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetVisible(true);
							m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetMaterial(CMaterialResource::pHalfMoreTransMaterial);
							printf("Installing \n");
							printf(" Install IDX : %d", iLinearIdx);
							break;
						}
					}
					else
					{
						// 해당 위치에 가지고 있는 복셀을 설치한다.
						D3DXMATRIX d3dxmtxFromIdxToPos;

						D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvInspectionIdx, &pVoxelTerrain->m_d3dxmtxFromIdxToPos);
						m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetPosition(D3DXVECTOR3(d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z));
						m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetVisible(true);
						m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetMaterial(CMaterialResource::pHalfMoreTransMaterial);
						printf("Installing \n");
						printf(" Install IDX : %d", iLinearIdx);
						break;
					}
				}
			}
		}
	}
	else		// 복셀 설치가 활성화되었다면.
	{
		D3DXVECTOR3 d3dxvSearchPoint;
		AABB HavingVoxelAABB = CPhysicalCollision::MoveAABB(m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->m_pMesh->m_AABB, m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->GetPosition());

		D3DXVec3Normalize(&d3dxvSearchPoint, &GetLook());
		d3dxvSearchPoint *= 0.65f;
		d3dxvSearchPoint += ( GetPosition() + d3dxvSearchDir[i]);
		d3dxvSearchPoint.y -= 0.655f;

		if (HavingVoxelAABB.m_d3dxvMin.x < d3dxvSearchPoint.x && d3dxvSearchPoint.x < HavingVoxelAABB.m_d3dxvMax.x
			&& HavingVoxelAABB.m_d3dxvMin.y < d3dxvSearchPoint.y && d3dxvSearchPoint.y < HavingVoxelAABB.m_d3dxvMax.y
			&& HavingVoxelAABB.m_d3dxvMin.z < d3dxvSearchPoint.z && d3dxvSearchPoint.z < HavingVoxelAABB.m_d3dxvMax.z)
		{
			fInstallTime += fTimeElapsed;
			if (fInstallTime >= INSTALL_COMPLETE_TIME)
			{
				printf("Insert iLinearIdx : %d ", iLinearIdx);
				
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetMaterial(CMaterialResource::pStandardMaterial);
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetVisible(true);
				pVoxelTerrain->m_ppVoxelObjectsForCollision[iLinearIdx] = m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel;
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel = NULL;
				fInstallTime = 0.0f;
				iLinearIdx = -1;
				i = 0;
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_bIsActive = false;
				
			}
			else if (fInstallTime >= INSTALL_COMPLETE_TIME * 0.5f)
			{
				m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->SetMaterial(CMaterialResource::pHalfLessTransMaterial);
			}
		}
	}
}

void CPlayer::PushPlayers(CPlayer **m_ppPlayers, int nConnectedPlayersNum)
{
	D3DXVECTOR3 d3dxvPushVelocity;
	float fDot;
	float fAttackRange;

	for (int i = 0; i < nConnectedPlayersNum; i++)
	{
		if (m_ppPlayers[i] == this) continue;
		else if (m_ppPlayers[i])
		{
			d3dxvPushVelocity = m_ppPlayers[i]->GetPosition() - this->GetPosition();

			if (D3DXVec3Length(&d3dxvPushVelocity) > 1.0f) continue;

			d3dxvPushVelocity.y = 0.0f;

			D3DXVec3Normalize(&d3dxvPushVelocity, &d3dxvPushVelocity);

			fDot = D3DXVec3Dot(&d3dxvPushVelocity, &this->GetLook());
			if (fDot >= 1.0f) fDot = 1.0f;
			else if (fDot <= -1.0f) fDot = -1.0f;
			float fAttackRange = acosf(fDot);

			if (D3DXToDegree(fAttackRange) > 45.0f) continue;

			m_ppPlayers[i]->m_d3dxvVelocity.x += d3dxvPushVelocity.x * 50.0f;
			m_ppPlayers[i]->m_d3dxvVelocity.z += d3dxvPushVelocity.z * 50.0f;;
		}
		
	}
}

void CPlayer::RotateMoveDir(float fTimeElapsed)
{
	if (m_d3dxvMoveDir.x == 0.0f && m_d3dxvMoveDir.z == 0.0f) return;
	/*printf("Moving Dir : (%lf, %lf, %lf) \n", d3dxvMovingDir.x, d3dxvMovingDir.y, d3dxvMovingDir.z);*/

	//D3DXVec3Normalize(&m_d3dxvMoveDir, &m_d3dxvMoveDir);
	D3DXMATRIX mtxRotate;

	// 회전축을 구한다.
	D3DXVECTOR3 d3dxvCross;
	D3DXVec3Cross(&d3dxvCross, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &m_CameraOperator.GetLook());
	//D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	// 회전량을 구한다.
	float fDot = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &m_CameraOperator.GetLook());
	if (fDot >= 1.0f) fDot = 1.0f;
	else if (fDot <= -1.0f) fDot = -1.0f;
	float fRotationRadianForProofreading = acosf(fDot);
	fRotationRadianForProofreading = d3dxvCross.y > 0.0f ? fRotationRadianForProofreading : -fRotationRadianForProofreading;

	D3DXMatrixRotationAxis(&mtxRotate, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), fRotationRadianForProofreading);

	D3DXVECTOR3 d3dxvMoveDir = D3DXVECTOR3(m_d3dxvMoveDir.x, 0.0f, m_d3dxvMoveDir.z);
	D3DXVec3TransformNormal(&d3dxvMoveDir, &d3dxvMoveDir, &mtxRotate);		// 보정된 위치 벡터
	D3DXVec3Normalize(&d3dxvMoveDir, &d3dxvMoveDir);
	D3DXVec3Cross(&d3dxvCross, &GetLook(), &d3dxvMoveDir);
	//D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	fDot = D3DXVec3Dot(&GetLook(), &d3dxvMoveDir);
	if (fDot >= 1.0f) fDot = 1.0f;
	else if (fDot <= -1.0f) fDot = -1.0f;

	m_d3dxvMoveDir.x = d3dxvMoveDir.x;
	m_d3dxvMoveDir.z = d3dxvMoveDir.z;
	/*printf("Player GetLook : (%f, %f, %f), Length : %f \n", GetLook().x, GetLook().y, GetLook().z, D3DXVec3Length(&GetLook()));
	printf("d3dxvMoveIdr : (%f, %f, %f), Length : %f \n", m_d3dxvMoveDir.x, m_d3dxvMoveDir.y, m_d3dxvMoveDir.z, D3DXVec3Length(&m_d3dxvMoveDir));*/
	fRotationRadianForProofreading = acosf(fDot);
	/*printf("fDot : %f \n", fDot);*/
	fRotationRadianForProofreading = d3dxvCross.y > 0.0f ? fRotationRadianForProofreading : -fRotationRadianForProofreading;
	fRotationRadianForProofreading = D3DXToRadian(D3DXToDegree(fRotationRadianForProofreading) / ( PLAYER_SOFT_ROTATION_SPLIT_FRACTION * 1/ fTimeElapsed));
	
	D3DXMatrixRotationAxis(&mtxRotate, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), fRotationRadianForProofreading);
	/*printf("Prev Player Position : (%lf, %lf, %lf) \n", GetPosition().x, GetPosition().y, GetPosition().z);
	printf("fRotationRadianForProofreading(Degree) : %f \n", D3DXToDegree(fRotationRadianForProofreading));
	printf("fDot : %f \n", fDot);
	printf("mtxRotate : (%f, %f, %f, %f) \n", mtxRotate._11, mtxRotate._12, mtxRotate._13, mtxRotate._14);
	printf("mtxRotate : (%f, %f, %f, %f) \n", mtxRotate._21, mtxRotate._22, mtxRotate._23, mtxRotate._24);
	printf("mtxRotate : (%f, %f, %f, %f) \n", mtxRotate._31, mtxRotate._32, mtxRotate._33, mtxRotate._34);
	printf("mtxRotate : (%f, %f, %f, %f) \n", mtxRotate._41, mtxRotate._42, mtxRotate._43, mtxRotate._44);*/
	D3DXMatrixMultiply(&m_d3dxmtxWorld, &mtxRotate, &m_d3dxmtxWorld);
	/*printf("Post Player Position : (%lf, %lf, %lf) \n", GetPosition().x, GetPosition().y, GetPosition().z);*/
	//printf("World Position : (%lf, %lf, %lf) \n", m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43);
	/*printf("_________________________________________ \n");*/
}

//void CPlayer::Jump(CVoxelTerrain *pVoxelTerrain)
//{
//	if (-0.1f <= m_d3dxvVelocity.y && m_d3dxvVelocity.y <= 0.1f)
//	{
//		m_d3dxvVelocity.y = 5.0f;
//	}
//}

void CPlayer::MovingUnderPhysicalEnvironment(CScene *pScene, CVoxelTerrain *pVoxelTerrain, float fTimeElapsed)
{
	static D3DXVECTOR3 d3dxvSearchDir[23] =
	{
		D3DXVECTOR3(0.0f, 0.0f, 1.0f),					// 상
		D3DXVECTOR3(0.0f, 0.0f, -1.0f),					// 하
		D3DXVECTOR3(1.0f, 0.0f, 0.0f),					// 우
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f),					// 좌
		
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
		D3DXVECTOR3(0.0f, -1.0f, 0.0f),

		D3DXVECTOR3(1.0f, -1.0f, -1.0f),	D3DXVECTOR3(0.0f, -1.0f, -1.0f),	D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
		D3DXVECTOR3(1.0f, -1.0f, 0.0f),											D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(1.0f, -1.0f, 1.0f),		D3DXVECTOR3(0.0f, -1.0f, 1.0f),		D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
		
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(1.0f, 1.0f, -1.0f),		D3DXVECTOR3(0.0f, 1.0f, -1.0f),		D3DXVECTOR3(-1.0f, 1.0f, -1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 0.0f),											D3DXVECTOR3(-1.0f, 1.0f, 0.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),		D3DXVECTOR3(0.0f, 1.0f, 1.0f),		D3DXVECTOR3(-1.0f, 1.0f, 1.0f)
	};

	// 세계의 중력을 받는다.
	D3DXVECTOR3 d3dxvPostV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR2 d3dxvVelocityXZ = D3DXVECTOR2(0.0f, 0.0f);
	D3DXVECTOR3 d3dxvG = D3DXVECTOR3(0.0f, GRAVITATIONAL_ACCELERATION, 0.0f);
	D3DXVECTOR3 d3dxvMovingDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//printf("Post Velocity : (%f, %f, %f) \n", d3dxvPostV.x, d3dxvPostV.y, d3dxvPostV.z);

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
	bool bIsSlopeVoxel = false;
	bool bIsFloor = false;
	bool bIsJumped = false;

	for (int i = 0; i < 24; i++)
	{
		d3dxvInspectionIdx.x = d3dxvColLayerRow.x + d3dxvSearchDir[i].x;
		d3dxvInspectionIdx.y = d3dxvColLayerRow.y + d3dxvSearchDir[i].y;
		d3dxvInspectionIdx.z = d3dxvColLayerRow.z + d3dxvSearchDir[i].z;

		if ((0 <= d3dxvInspectionIdx.x && d3dxvInspectionIdx.x < pVoxelTerrain->m_iMaxCol)
			&& (0 <= d3dxvInspectionIdx.y && d3dxvInspectionIdx.y < pVoxelTerrain->m_iMaxLayer)
			&& (0 <= d3dxvInspectionIdx.z && d3dxvInspectionIdx.z < pVoxelTerrain->m_iMaxRow))
		{

			if ((pCollidedVoxel = pVoxelTerrain->m_ppVoxelObjectsForCollision[
				(int)((d3dxvInspectionIdx.y) * pVoxelTerrain->m_iLayerStride
					+ (d3dxvInspectionIdx.z) * pVoxelTerrain->m_iRowStride
					+ (d3dxvInspectionIdx.x))]) != NULL)
			{
				if (CPhysicalCollision::IsCollided(
					&CPhysicalCollision::MoveAABB(this->m_pMesh->m_AABB, GetPosition()),
					&CPhysicalCollision::MoveAABB(pCollidedVoxel->m_pMesh->m_AABB, pCollidedVoxel->GetPosition())))
				{
					D3DXVECTOR3 d3dxvRevision(0.0f, 0.0f, 0.0f);

					if (i < 4)		// 평범한 복셀에 한해서
					{
						if (!pCollidedVoxel->m_bIsSlope)
						{
							if (i < 2)
							{
								m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z)) * D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z));
								m_d3dxvVelocity.z *= pCollidedVoxel->m_fReflection;
								//d3dxvPostV.y = m_d3dxvVelocity.y;
								//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

								d3dxvRevision.z = ((pCollidedVoxel->GetPosition().z - d3dxvSearchDir[i].z * pVoxelTerrain->m_fCubeDepth / 2.0f)
									- (GetPosition().z + d3dxvSearchDir[i].z * m_pMesh->m_AABB.m_d3dxvMax.z));
								//d3dxvPostV.z = 0.0f;
							}
							else
							{
								m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f)) * D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f));
								m_d3dxvVelocity.x *= pCollidedVoxel->m_fReflection;
								//d3dxvPostV.y = m_d3dxvVelocity.y;
								//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

								d3dxvRevision.x = ((pCollidedVoxel->GetPosition().x + d3dxvSearchDir[i].x * pVoxelTerrain->m_fCubeWidth / 2.0f)
									- (GetPosition().x - d3dxvSearchDir[i].x * m_pMesh->m_AABB.m_d3dxvMax.x));
								//d3dxvPostV.x = 0.0f;
							}
							/*Moving(d3dxvRevision);
							m_CameraOperator.MoveCameraOperator(d3dxvRevision);*/
						}
						else
						{
							if (i < 2)			// 하
							{
								/*printf("pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal (%f, %f, %f) \n",
									pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.x,
									pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.y,
									pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.z);*/
								if (0.98f < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z), &pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal))
								{
									m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z)) * D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z));
									m_d3dxvVelocity.z *= pCollidedVoxel->m_fReflection;
									//d3dxvPostV.y = m_d3dxvVelocity.y;
									//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

									d3dxvRevision.z = ((pCollidedVoxel->GetPosition().z - d3dxvSearchDir[i].z * pVoxelTerrain->m_fCubeDepth / 2.0f)
										- (GetPosition().z + d3dxvSearchDir[i].z * m_pMesh->m_AABB.m_d3dxvMax.z));
									//d3dxvPostV.z = 0.0f;
								}	
							}
							else                // 좌
							{
								if (0.98f < D3DXVec3Dot(&D3DXVECTOR3(d3dxvSearchDir[i].x, 0.0f, 0.0f), &pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal))
								{
									m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f)) * D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f));
									m_d3dxvVelocity.x *= pCollidedVoxel->m_fReflection;
									//d3dxvPostV.y = m_d3dxvVelocity.y;
									//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

									d3dxvRevision.x = ((pCollidedVoxel->GetPosition().x + d3dxvSearchDir[i].x * pVoxelTerrain->m_fCubeWidth / 2.0f)
										- (GetPosition().x - d3dxvSearchDir[i].x * m_pMesh->m_AABB.m_d3dxvMax.x));
								}
							}
						}
						Moving(d3dxvRevision);
						m_CameraOperator.MoveCameraOperator(d3dxvRevision);
					}
					else if (i < 14)
					{
						//
						float fDistance = 0.0f;
						if (!pCollidedVoxel->m_bIsSlope)
						{
							if (-0.2f < m_d3dxvVelocity.y && m_d3dxvVelocity.y < 0.2f)
							{
								m_d3dxvVelocity.y = 0.0f;
							}
							else
							{
								m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)) * D3DXVECTOR3(0.0f, 1.0f, 0.0f));
								m_d3dxvVelocity.y *= pCollidedVoxel->m_fReflection;
							}
							//printf("m_d3dxvVelocity.y : %f \n", m_d3dxvVelocity.y);
							d3dxvRevision.y = ((pCollidedVoxel->GetPosition().y + pVoxelTerrain->m_fCubeHeight / 2.0f)
								- (GetPosition().y + m_pMesh->m_AABB.m_d3dxvMin.y));
							if (pCollidedVoxel->m_eVoxelType == VOXEL_TYPE_SWAMP_0) d3dxvRevision.y += -0.1f;
						}
						else //pCollidedVoxel->m_bIsSlope)
						{
							fDistance = pCollidedVoxel->GetHeightByPlayerPos(GetPosition()) - (GetPosition().y + (m_pMesh->m_AABB.m_d3dxvMin.y));
							if (fDistance < -0.0f) break;// i = 13;
							m_d3dxvVelocity.y = 0.0f;
							d3dxvRevision.y = fDistance;
							if(i == 4 || i == 5) bIsSlopeVoxel = true;
							
						}//printf("ddd\n");
				
						//printf("Velocity : %f, %f, %f\n", m_d3dxvVelocity.x, m_d3dxvVelocity.y, m_d3dxvVelocity.z);
						// 마찰력을 적용한다.
						d3dxvVelocityXZ = -D3DXVECTOR2(m_d3dxvVelocity.x, m_d3dxvVelocity.z);
						float fLengthVelocityXZ = D3DXVec2Length(&d3dxvVelocityXZ);
						D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
						float fDeceleration = pCollidedVoxel->m_fFriction * fTimeElapsed;
						fDeceleration += PLAYER_FRICTION * fTimeElapsed;
						if (fDeceleration > fLengthVelocityXZ) fDeceleration = fLengthVelocityXZ;
						d3dxvVelocityXZ = d3dxvVelocityXZ * fDeceleration;
						m_d3dxvVelocity.x += d3dxvVelocityXZ.x;
						m_d3dxvVelocity.z += d3dxvVelocityXZ.y;
						
						//printf("%f \n", fDis*tance);
						if (m_d3dxvMoveDir.y != 0.0f && pCollidedVoxel->m_eVoxelType != VOXEL_TYPE_SWAMP_0) bIsJumped = true;//m_d3dxvVelocity.y = 4.95f;
						Moving(d3dxvRevision);
						m_CameraOperator.MoveCameraOperator(d3dxvRevision);
						/*printf("<<<<<<<<<<<<<<<<<COLLIDE!!!!>>>>>>>>>>>>>>>>>>>>>\n");*/
						bIsFloor = true;
						i = 13;
						//break;
					}
					else if(i < 23)
					{
						m_d3dxvVelocity.y = 0.0f;
						//d3dxvRevision.x = d3dxvRevision.z = 0.0f;
						d3dxvRevision.y = ((pCollidedVoxel->GetPosition().y - pVoxelTerrain->m_fCubeHeight / 2.0f)
							- (GetPosition().y + m_pMesh->m_AABB.m_d3dxvMax.y));
						Moving(d3dxvRevision);
						m_CameraOperator.MoveCameraOperator(d3dxvRevision);

						if ((0 <= d3dxvColLayerRow.x && d3dxvColLayerRow.x < pVoxelTerrain->m_iMaxCol)
							&& (0 <= d3dxvColLayerRow.y && d3dxvColLayerRow.y < pVoxelTerrain->m_iMaxLayer)
							&& (0 <= d3dxvColLayerRow.z && d3dxvColLayerRow.z < pVoxelTerrain->m_iMaxRow))
						{
							if ((pCollidedVoxel = pVoxelTerrain->m_ppVoxelObjectsForCollision[
								(int)((d3dxvColLayerRow.y) * pVoxelTerrain->m_iLayerStride
									+ d3dxvColLayerRow.z * pVoxelTerrain->m_iRowStride
									+ d3dxvColLayerRow.x)]) != NULL)
							{
								if (CPhysicalCollision::IsCollided(
									&CPhysicalCollision::MoveAABB(this->m_pMesh->m_AABB, GetPosition()),
									&CPhysicalCollision::MoveAABB(pCollidedVoxel->m_pMesh->m_AABB, pCollidedVoxel->GetPosition())))
								{
									if (pCollidedVoxel->m_bIsSlope)
									{
										D3DXVECTOR2 d3dxvFrontNormalXZ =
											D3DXVECTOR2(pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxFrontNormal.x,
												pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxFrontNormal.z);
										D3DXVec2Normalize(&d3dxvFrontNormalXZ, &d3dxvFrontNormalXZ);
										//d3dxvFrontNormalXZ * 0.1f;

										d3dxvRevision.x = round(d3dxvFrontNormalXZ.x) == 0.0f ? 0.0f : ((d3dxvFrontNormalXZ.x * 0.5f + pCollidedVoxel->GetPosition().x) - GetPosition().x) * 0.9;
										d3dxvRevision.y = 0.0f;
										d3dxvRevision.z = round(d3dxvFrontNormalXZ.y) == 0.0f ? 0.0f : ((d3dxvFrontNormalXZ.y * 0.5f + pCollidedVoxel->GetPosition().z) - GetPosition().z) * 0.9;

										round(d3dxvFrontNormalXZ.x) != 0.0f ? m_d3dxvVelocity.x = 0.0f : m_d3dxvVelocity.z = 0.0f;

										printf("d3dxvFrontNormalXZ.x y : %f %f\n ", d3dxvFrontNormalXZ.x, d3dxvFrontNormalXZ.y);
										printf("d3dxvRevision : (%f, %f, %f)\n", d3dxvRevision.x, d3dxvRevision.y, d3dxvRevision.z);
										Moving(d3dxvRevision);
										m_CameraOperator.MoveCameraOperator(d3dxvRevision);
										//if (m_d3dxvMoveDir.y != 0.0f) m_d3dxvVelocity.y = 4.95f;

										bIsSlopeVoxel = true;
									}
								}
							}
								bIsJumped = false;
								break;
						}
						else break;
					}
				}
			}
		}
	}
	
	//printf("Player Bottom : %f \n", GetPosition().y + m_pMesh->m_AABB.m_d3dxvMin.y);
	d3dxvPostV = m_d3dxvVelocity;			

	// 외부에서 가해진 이동 속도를 갱신한다
	D3DXVECTOR3 d3dxvMoveDir = D3DXVECTOR3(m_d3dxvMoveDir.x, 0.0f, m_d3dxvMoveDir.z);
	D3DXVec3Normalize(&d3dxvMoveDir, &d3dxvMoveDir);
	d3dxvMovingDir = PLAYER_ACCELERATION_MAGNITUDE * d3dxvMoveDir * fTimeElapsed;// +d3dxvPostV;

																				   // 최대 속도를 제한한다.
	d3dxvVelocityXZ = D3DXVECTOR2(d3dxvMovingDir.x, d3dxvMovingDir.z);

	/*if (D3DXVec2Length(&d3dxvVelocityXZ) > PLAYER_MAX_VELOCITY_MAGNITUDE)
	{
		D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
		d3dxvVelocityXZ *= PLAYER_MAX_VELOCITY_MAGNITUDE;
	}*/
	d3dxvPostV.x += d3dxvVelocityXZ.x;
	d3dxvPostV.z += d3dxvVelocityXZ.y;

	// 속도에 중력가속도를 적용한다.
	d3dxvPostV += d3dxvG * fTimeElapsed;	

	// 최대 속도 제한
	if (d3dxvPostV.y > PLAYER_MAX_VELOCITY_MAGNITUDE)
	{
		d3dxvPostV.y = PLAYER_MAX_VELOCITY_MAGNITUDE;
	}

	//printf("[bIsFloor : %d] \n", bIsFloor);
	if (!bIsFloor)
	{
		// 바람 저항을 계산한다.
		//d3dxvVelocityXZ = D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
		//d3dxvVelocityXZ *= (AIR_RESISTANCE_COEFFICIENT);
		d3dxvVelocityXZ = -D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
		float fLengthVelocityXZ = D3DXVec2Length(&d3dxvVelocityXZ);
		D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
		float fDeceleration = AIR_RESISTANCE_COEFFICIENT * fTimeElapsed;
		//d3dxvVelocityXZ *= (pCollidedVoxel->m_fDamping * PLAYER_FRICTION);
		if (fDeceleration > fLengthVelocityXZ) fDeceleration = fLengthVelocityXZ;
		d3dxvVelocityXZ = d3dxvVelocityXZ * fDeceleration;
		d3dxvPostV.x += d3dxvVelocityXZ.x;
		d3dxvPostV.z += d3dxvVelocityXZ.y;
		//d3dxvPostV.x = d3dxvVelocityXZ.x;
		//d3dxvPostV.z = d3dxvVelocityXZ.y;
	}
	

	d3dxvVelocityXZ = D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);

	if (bIsSlopeVoxel)
	{
		if (D3DXVec2Length(&d3dxvVelocityXZ) > D3DXVec3Length(&D3DXVECTOR3(0.0f, d3dxvPostV.y, 0.0f)))
		{
			d3dxvPostV.y = -D3DXVec2Length(&d3dxvVelocityXZ);
		}
	}
	if (bIsJumped)
	{
		d3dxvPostV.y = m_d3dxvVelocity.y = 1.95;//4.95f;// +d3dxvPostV.y;
	}

	// 최대 속도 제한
	if (D3DXVec2Length(&d3dxvVelocityXZ) > PLAYER_MAX_VELOCITY_MAGNITUDE)
	{
		D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
		d3dxvVelocityXZ *= PLAYER_MAX_VELOCITY_MAGNITUDE;	
	}

	// 최종 속도 생성
	d3dxvPostV.x = d3dxvVelocityXZ.x;
	d3dxvPostV.z = d3dxvVelocityXZ.y;

	// 이동량 갱신
	d3dxvMovingDir = ((d3dxvPostV + m_d3dxvVelocity) * fTimeElapsed) / 2.0f;

	// 실제 이동
	Moving(d3dxvMovingDir);
	m_CameraOperator.MoveCameraOperator(d3dxvMovingDir);

	// 현재 속도 갱신
	m_d3dxvVelocity = d3dxvPostV;
	//printf("Player Bottome : %.6f \n", GetPosition().y - 0.5f);

	// 이동 방향 초기화
	m_d3dxvMoveDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//d3dxvVelocityXZ = D3DXVECTOR2(m_d3dxvVelocity.x, m_d3dxvVelocity.z);
	//printf("Player Velocity : %f\n", D3DXVec2Length(&d3dxvVelocityXZ));
	//printf("Velocity : (%.5f, %.5f, %.5f) \n", m_d3dxvVelocity.x, m_d3dxvVelocity.y, m_d3dxvVelocity.z);
	//printf("-----------------------------\n");
}

void CPlayer::ProofreadLocalAxis(void)
{
	D3DXVECTOR3 d3dxvNormalizedAxis;

	D3DXVec3Normalize(&d3dxvNormalizedAxis, &GetLook());
	SetLook(d3dxvNormalizedAxis);
	SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXVec3Cross(&d3dxvNormalizedAxis, &GetUp(), &GetLook());
	D3DXVec3Normalize(&d3dxvNormalizedAxis, &d3dxvNormalizedAxis);
	SetRight(d3dxvNormalizedAxis);

	m_CameraOperator.ProofreadLocalAxis();
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
