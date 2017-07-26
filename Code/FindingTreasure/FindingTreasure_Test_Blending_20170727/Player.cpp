#include "stdafx.h"
#include "Player.h"
#include "CameraOperator.h"
#include "PhysicalCollision.h"


CPlayer::CPlayer()
{
	// m_pCamera = NULL;
	//InitCameraOperator();
	m_bIsActive = true;
	m_BelongType = BELONG_TYPE_INDIVIDUAL;
	m_d3dxvMoveDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fFriction = PLAYER_FRICTION;
	for (int i = 0; i < 5; i++)
	{
		m_VoxelPocket[i].m_pVoxel = NULL;
		m_VoxelPocket[i].m_bIsActive = false;
	}
	// m_pHavingVoxel = NULL;
	// m_bIsHavingVoxel = false;
	m_iVoxelPocketSlotIdx = 0;

	//m_action = IDLE;

	m_bIsPushed = false;
	m_pLiftingPlayer = NULL;
	m_bIsDigOrInstall = false;
	move = false;
}

CPlayer::~CPlayer()
{
	// if (m_pCamera) delete m_pCamera;

}

void CPlayer::InitPlayer(D3DXVECTOR3 d3dxvPos)
{
	m_d3dxvMoveDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fFriction = PLAYER_FRICTION;
	for (int i = 0; i < 5; i++)
	{
		m_VoxelPocket[i].m_pVoxel = NULL;
		m_VoxelPocket[i].m_bIsActive = false;
	}
	// m_pHavingVoxel = NULL;
	// m_bIsHavingVoxel = false;
	m_iVoxelPocketSlotIdx = 0;

	m_bIsPushed = false;
	m_pLiftingPlayer = NULL;
	m_bIsDigOrInstall = false;
	SetPosition(d3dxvPos);
	InitCameraOperator();
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
#define DIG_COMPLETE_TIME	0.4f

	static float fDigTime = 0.0f;
	static int iLinearIdx = -1;
	static int i = 0;

	static D3DXVECTOR3 d3dxvSearchDir[2] =
	{
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// �ڽ��� ��ġ
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),					// ��
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

	if (!m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel)			// ������ �ִ� ������ ���ٸ�, ���� ������ ã�´�.
	{
		D3DXVECTOR3 d3dxvSearchPoint;

		D3DXVec3Normalize(&d3dxvSearchPoint, &GetLook());
		d3dxvSearchPoint *= 0.65f;
		d3dxvSearchPoint += GetPosition();
		d3dxvSearchPoint.y -= 0.655f;
		// ���� �����ʰ��� �浹 üũ�� �Ѵ�.
		D3DXMATRIX d3dxmtxFromPosToIdx;
		D3DXVECTOR4 d3dxvColLayerRow;

		D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
		D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &pVoxelTerrain->m_d3dxmtxFromIdxToPos);
		D3DXVec3Transform(&d3dxvColLayerRow, &GetPosition(), &d3dxmtxFromPosToIdx);

		CVoxel *pCollidedVoxel;
		D3DXVECTOR3 d3dxvInspectionIdx;			// �ݸ��� �ڽ��� ��ġ�� �ε��� ��ǥ
		//D3DXVECTOR3 d3dxvInstallPos;			// ������ ��ġ�� ��ġ ��ǥ

		// Ž�� ������ �ε���ȭ �Ѵ�.
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
							// �ش� ������ �����Ѵ�.
							m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel = pCollidedVoxel;
							printf("iLinearIdx : %d ", iLinearIdx);
							// pCollidedVoxel->SetActive(false);
							printf("delete!\n");
							break;
						}
					}
					else
					{
						// �ش� ������ �����Ѵ�.
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
	else		// ������ �����ϰ� �ִٸ�,
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
#define INSTALL_COMPLETE_TIME	0.4f

	static float fInstallTime = 0.0f;
	static int iLinearIdx = -1;
	static int i = 0;

	static D3DXVECTOR3 d3dxvSearchDir[2] =
	{
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),					// �ڽ��� ��ġ
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),					// ��
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

	if (!m_VoxelPocket[m_iVoxelPocketSlotIdx].m_pVoxel->m_bIsVisible)			// ������ ��ġ���� �ʾҴٸ�. ��ġ ��ġ�� ã�´�.
	{
		D3DXVECTOR3 d3dxvSearchPoint;

		D3DXVec3Normalize(&d3dxvSearchPoint, &GetLook());
		d3dxvSearchPoint *= 0.65f;
		d3dxvSearchPoint += GetPosition();
		d3dxvSearchPoint.y -= 0.655f;
		// ���� �����ʰ��� �浹 üũ�� �Ѵ�.
		D3DXMATRIX d3dxmtxFromPosToIdx;
		D3DXVECTOR4 d3dxvColLayerRow;

		D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
		D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &pVoxelTerrain->m_d3dxmtxFromIdxToPos);
		D3DXVec3Transform(&d3dxvColLayerRow, &GetPosition(), &d3dxmtxFromPosToIdx);


		// �ڽ��� ��ġ�� �ε���ȭ �Ѵ�.
		d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
		d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
		d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);

		int iPlayerIdxY = d3dxvColLayerRow.y;

		CVoxel *pCollidedVoxel;
		D3DXVECTOR3 d3dxvInspectionIdx;			// �ݸ��� �ڽ��� ��ġ�� �ε��� ��ǥ
		D3DXVECTOR3 d3dxvInstallPos;			// ������ ��ġ�� ��ġ ��ǥ

		// ���鿡 ����ִ� ���, 
		// Ž�� ������ �ε���ȭ �Ѵ�.
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
							// �ش� ��ġ�� ������ �ִ� ������ ��ġ�Ѵ�.
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
						// �ش� ��ġ�� ������ �ִ� ������ ��ġ�Ѵ�.
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
	else		// ���� ��ġ�� Ȱ��ȭ�Ǿ��ٸ�.
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

void CPlayer::ChangeVoxelPocketSlotIdx(int iIdx)
{
	if (m_VoxelPocket[iIdx].m_bIsActive && m_VoxelPocket[iIdx].m_pVoxel->m_bIsSlope)
	{
		VOXEL_TYPE eVoxelType;
		eVoxelType = (VOXEL_TYPE)((m_VoxelPocket[iIdx].m_pVoxel->m_eVoxelType - VOXEL_TYPE_UPHILL_0 + 1) % (VOXEL_TYPE_UPHILL_3 - VOXEL_TYPE_UPHILL_0 + 1) + VOXEL_TYPE_UPHILL_0);
		CVoxel* pChangedVoxel = CVoxelTerrain::GetVoxelForType(eVoxelType);
		m_VoxelPocket[iIdx].m_pVoxel->m_eVoxelType = pChangedVoxel->m_eVoxelType;
		m_VoxelPocket[iIdx].m_pVoxel->m_eVoxelDir = pChangedVoxel->m_eVoxelDir;
		m_VoxelPocket[iIdx].m_pVoxel->SetMesh(pChangedVoxel->m_pMesh);
		m_VoxelPocket[iIdx].m_pVoxel->SetTexture(pChangedVoxel->m_pTexture);
		delete pChangedVoxel;
	}

	m_iVoxelPocketSlotIdx = iIdx;
}

void CPlayer::PushPlayer(CPlayer *pPlayer)
{
	D3DXVECTOR3 d3dxvPushVelocity;
	float fDot;
	float fAttackRange;


	if (pPlayer == this) return;
	else if (pPlayer)
	{
		d3dxvPushVelocity = pPlayer->GetPosition() - this->GetPosition();

		if (D3DXVec3Length(&d3dxvPushVelocity) > 1.5f) return;

		d3dxvPushVelocity.y = 0.0f;

		D3DXVec3Normalize(&d3dxvPushVelocity, &d3dxvPushVelocity);

		fDot = D3DXVec3Dot(&d3dxvPushVelocity, &this->GetLook());
		if (fDot >= 1.0f) fDot = 1.0f;
		else if (fDot <= -1.0f) fDot = -1.0f;
		float fAttackRange = acosf(fDot);

		if (D3DXToDegree(fAttackRange) > 60.0f) return;

		D3DXVECTOR2 d3dxvRevisionVelocity =
			D3DXVECTOR2(pPlayer->GetPosition().x - this->GetPosition().x, pPlayer->GetPosition().z - this->GetPosition().z);

		D3DXVec2Normalize(&d3dxvRevisionVelocity, &d3dxvRevisionVelocity);
		d3dxvRevisionVelocity *= PLAYER_MAX_VELOCITY_MAGNITUDE;

		pPlayer->m_d3dxvVelocity.x = d3dxvRevisionVelocity.x;
		pPlayer->m_d3dxvVelocity.z = d3dxvRevisionVelocity.y;
	}
}

void CPlayer::LiftPlayer(CPlayer *pPlayer, CVoxelTerrain *pVoxelTerrain)
{
	if (pPlayer->m_pLiftingPlayer) return;

	D3DXVECTOR3 d3dxvDistance;
	float fDot;
	D3DXMATRIX d3dxmtxFromPosToIdx;
	D3DXVECTOR4 d3dxvColLayerRow;
	D3DXVECTOR3 d3dxvMiddleCollisionPivot;
	const float fPivotOffset = 0.00001f;

	if (pPlayer == this) return;
	else if (pPlayer)
	{
		d3dxvDistance = pPlayer->GetPosition() - this->GetPosition();

		if (D3DXVec3Length(&d3dxvDistance) > 1.5f) return;

		D3DXVec3Normalize(&d3dxvDistance, &d3dxvDistance);

		fDot = D3DXVec3Dot(&d3dxvDistance, &this->GetLook());
		if (fDot >= 1.0f) fDot = 1.0f;
		else if (fDot <= -1.0f) fDot = -1.0f;
		float fAttackRange = acosf(fDot);

		if (D3DXToDegree(fAttackRange) > 60.0f) return;


		D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
		D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &pVoxelTerrain->m_d3dxmtxFromIdxToPos);

		d3dxvMiddleCollisionPivot = D3DXVECTOR3(GetPosition().x, GetPosition().y + fPivotOffset, GetPosition().z);

		D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvMiddleCollisionPivot, &d3dxmtxFromPosToIdx);

		d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
		d3dxvColLayerRow.y = round(d3dxvColLayerRow.y) + 1;
		d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);

		if ((0 <= d3dxvColLayerRow.x && d3dxvColLayerRow.x < pVoxelTerrain->m_iMaxCol)
			&& (0 <= d3dxvColLayerRow.y && d3dxvColLayerRow.y < pVoxelTerrain->m_iMaxLayer)
			&& (0 <= d3dxvColLayerRow.z && d3dxvColLayerRow.z < pVoxelTerrain->m_iMaxRow))
		{

			if (pVoxelTerrain->m_ppVoxelObjectsForCollision[
				(int)((d3dxvColLayerRow.y) * pVoxelTerrain->m_iLayerStride
					+ (d3dxvColLayerRow.z) * pVoxelTerrain->m_iRowStride
					+ (d3dxvColLayerRow.x))]) return;
		}
		
			
		pPlayer->m_pLiftingPlayer = this;

		pPlayer->m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 d3dxvPos = GetPosition();

		d3dxvPos.y += (pPlayer->m_pMesh->m_AABB.m_d3dxvMax.y + m_pMesh->m_AABB.m_d3dxvMax.y + 0.2f);
		pPlayer->SetPosition(d3dxvPos);
	}
}

void CPlayer::BeRelievedFromLiftingPlayer(void)
{
	if (!m_pLiftingPlayer) return;
	
	if (!m_pLiftingPlayer->m_bIsPushed)
	{
		m_pLiftingPlayer = NULL;
	}
}

void CPlayer::BeDraggedAwayByLiftingPlayer(float fTimeElapsed)
{
	if (!m_pLiftingPlayer) return;

	// �ӵ� ����
	m_d3dxvVelocity = m_pLiftingPlayer->m_d3dxvVelocity;
	D3DXVECTOR3 d3dxvPos = m_pLiftingPlayer->GetPosition();

	// ��ġ ����
	d3dxvPos.y += (m_pLiftingPlayer->m_pMesh->m_AABB.m_d3dxvMax.y + m_pMesh->m_AABB.m_d3dxvMax.y + 0.2f);
	SetPosition(d3dxvPos);

	// ȸ�� ����
	m_d3dxvMoveDir = m_pLiftingPlayer->GetLook();

	RotateMoveDir(fTimeElapsed);
}

void CPlayer::PushPlayers(CPlayer **ppPlayers, int nConnectedPlayersNum)
{
	D3DXVECTOR3 d3dxvPushVelocity;
	float fDot;
	float fAttackRange;

	for (int i = 0; i < nConnectedPlayersNum; i++)
	{
		if (ppPlayers[i] == this) continue;
		else if (ppPlayers[i])
		{
			d3dxvPushVelocity = ppPlayers[i]->GetPosition() - this->GetPosition();

			if (D3DXVec3Length(&d3dxvPushVelocity) > 1.5f) continue;

			d3dxvPushVelocity.y = 0.0f;

			D3DXVec3Normalize(&d3dxvPushVelocity, &d3dxvPushVelocity);

			fDot = D3DXVec3Dot(&d3dxvPushVelocity, &this->GetLook());
			if (fDot >= 1.0f) fDot = 1.0f;
			else if (fDot <= -1.0f) fDot = -1.0f;
			float fAttackRange = acosf(fDot);

			if (D3DXToDegree(fAttackRange) > 60.0f) continue;

			D3DXVECTOR2 d3dxvRevisionVelocity = 
				D3DXVECTOR2(ppPlayers[i]->GetPosition().x - this->GetPosition().x, ppPlayers[i]->GetPosition().z - this->GetPosition().z);

			D3DXVec2Normalize(&d3dxvRevisionVelocity, &d3dxvRevisionVelocity);
			d3dxvRevisionVelocity *= PLAYER_MAX_VELOCITY_MAGNITUDE;

			ppPlayers[i]->m_d3dxvVelocity.x = d3dxvRevisionVelocity.x;
			ppPlayers[i]->m_d3dxvVelocity.z = d3dxvRevisionVelocity.y;
		}
	}
}

void CPlayer::RotateMoveDir(float fTimeElapsed)
{
	if (m_d3dxvMoveDir.x == 0.0f && m_d3dxvMoveDir.z == 0.0f) {
		SetFBXAnimForType(PIRATE_ANIM_TYPE_IDLE);
		return;
	}
	/*printf("Moving Dir : (%lf, %lf, %lf) \n", d3dxvMovingDir.x, d3dxvMovingDir.y, d3dxvMovingDir.z);*/
	SetFBXAnimForType(PIRATE_ANIM_TYPE_WALK);
	//D3DXVec3Normalize(&m_d3dxvMoveDir, &m_d3dxvMoveDir);
	D3DXMATRIX mtxRotate;

	// ȸ������ ���Ѵ�.
	D3DXVECTOR3 d3dxvCross;
	D3DXVec3Cross(&d3dxvCross, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &m_CameraOperator.GetLook());
	//D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	// ȸ������ ���Ѵ�.
	float fDot = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &m_CameraOperator.GetLook());
	if (fDot >= 1.0f) fDot = 1.0f;
	else if (fDot <= -1.0f) fDot = -1.0f;
	float fRotationRadianForProofreading = acosf(fDot);
	fRotationRadianForProofreading = d3dxvCross.y > 0.0f ? fRotationRadianForProofreading : -fRotationRadianForProofreading;

	D3DXMatrixRotationAxis(&mtxRotate, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), fRotationRadianForProofreading);

	D3DXVECTOR3 d3dxvMoveDir = D3DXVECTOR3(m_d3dxvMoveDir.x, 0.0f, m_d3dxvMoveDir.z);
	D3DXVec3TransformNormal(&d3dxvMoveDir, &d3dxvMoveDir, &mtxRotate);		// ������ ��ġ ����
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

//void CPlayer::MovingUnderPhysicalEnvironment(CScene *pScene, CVoxelTerrain *pVoxelTerrain, float fTimeElapsed)
//{
//	static D3DXVECTOR3 d3dxvSearchDir[23] =
//	{
//		D3DXVECTOR3(0.0f, 0.0f, 1.0f),					// ��
//		D3DXVECTOR3(0.0f, 0.0f, -1.0f),					// ��
//		D3DXVECTOR3(1.0f, 0.0f, 0.0f),					// ��
//		D3DXVECTOR3(-1.0f, 0.0f, 0.0f),					// ��
//		
//		D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
//		D3DXVECTOR3(0.0f, -1.0f, 0.0f),
//
//		D3DXVECTOR3(1.0f, -1.0f, -1.0f),	D3DXVECTOR3(0.0f, -1.0f, -1.0f),	D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
//		D3DXVECTOR3(1.0f, -1.0f, 0.0f),											D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
//		D3DXVECTOR3(1.0f, -1.0f, 1.0f),		D3DXVECTOR3(0.0f, -1.0f, 1.0f),		D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
//		
//		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
//		D3DXVECTOR3(1.0f, 1.0f, -1.0f),		D3DXVECTOR3(0.0f, 1.0f, -1.0f),		D3DXVECTOR3(-1.0f, 1.0f, -1.0f),
//		D3DXVECTOR3(1.0f, 1.0f, 0.0f),											D3DXVECTOR3(-1.0f, 1.0f, 0.0f),
//		D3DXVECTOR3(1.0f, 1.0f, 1.0f),		D3DXVECTOR3(0.0f, 1.0f, 1.0f),		D3DXVECTOR3(-1.0f, 1.0f, 1.0f)
//	};
//
//	// ������ �߷��� �޴´�.
//	D3DXVECTOR3 d3dxvPostV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//	D3DXVECTOR2 d3dxvVelocityXZ = D3DXVECTOR2(0.0f, 0.0f);
//	D3DXVECTOR3 d3dxvG = D3DXVECTOR3(0.0f, GRAVITATIONAL_ACCELERATION, 0.0f);
//	D3DXVECTOR3 d3dxvMovingDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//
//	//printf("Post Velocity : (%f, %f, %f) \n", d3dxvPostV.x, d3dxvPostV.y, d3dxvPostV.z);
//
//	// ���� �����ʰ��� �浹 üũ�� �Ѵ�.
//	D3DXMATRIX d3dxmtxFromPosToIdx;
//	D3DXVECTOR4 d3dxvColLayerRow;
//
//	D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
//	D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &pVoxelTerrain->m_d3dxmtxFromIdxToPos);
//	D3DXVec3Transform(&d3dxvColLayerRow, &GetPosition(), &d3dxmtxFromPosToIdx);
//
//	d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
//	d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
//	d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);
//
//	CVoxel *pCollidedVoxel;
//	D3DXVECTOR3 d3dxvInspectionIdx;
//	bool bIsSlopeVoxel = false;
//	bool bIsFloor = false;
//	bool bIsJumped = false;
//
//	for (int i = 0; i < 24; i++)
//	{
//		d3dxvInspectionIdx.x = d3dxvColLayerRow.x + d3dxvSearchDir[i].x;
//		d3dxvInspectionIdx.y = d3dxvColLayerRow.y + d3dxvSearchDir[i].y;
//		d3dxvInspectionIdx.z = d3dxvColLayerRow.z + d3dxvSearchDir[i].z;
//
//		if ((0 <= d3dxvInspectionIdx.x && d3dxvInspectionIdx.x < pVoxelTerrain->m_iMaxCol)
//			&& (0 <= d3dxvInspectionIdx.y && d3dxvInspectionIdx.y < pVoxelTerrain->m_iMaxLayer)
//			&& (0 <= d3dxvInspectionIdx.z && d3dxvInspectionIdx.z < pVoxelTerrain->m_iMaxRow))
//		{
//
//			if ((pCollidedVoxel = pVoxelTerrain->m_ppVoxelObjectsForCollision[
//				(int)((d3dxvInspectionIdx.y) * pVoxelTerrain->m_iLayerStride
//					+ (d3dxvInspectionIdx.z) * pVoxelTerrain->m_iRowStride
//					+ (d3dxvInspectionIdx.x))]) != NULL)
//			{
//				if (CPhysicalCollision::IsCollided(
//					&CPhysicalCollision::MoveAABB(this->m_pMesh->m_AABB, GetPosition()),
//					&CPhysicalCollision::MoveAABB(pCollidedVoxel->m_pMesh->m_AABB, pCollidedVoxel->GetPosition())))
//				{
//					D3DXVECTOR3 d3dxvRevision(0.0f, 0.0f, 0.0f);
//
//					if (i < 4)		// ����� ������ ���ؼ�
//					{
//						if (!pCollidedVoxel->m_bIsSlope)
//						{
//							if (i < 2)
//							{
//								m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z)) * D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z));
//								m_d3dxvVelocity.z *= pCollidedVoxel->m_fReflection;
//								//d3dxvPostV.y = m_d3dxvVelocity.y;
//								//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;
//
//								d3dxvRevision.z = ((pCollidedVoxel->GetPosition().z - d3dxvSearchDir[i].z * pVoxelTerrain->m_fCubeDepth / 2.0f)
//									- (GetPosition().z + d3dxvSearchDir[i].z * m_pMesh->m_AABB.m_d3dxvMax.z));
//								//d3dxvPostV.z = 0.0f;
//							}
//							else
//							{
//								m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f)) * D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f));
//								m_d3dxvVelocity.x *= pCollidedVoxel->m_fReflection;
//								//d3dxvPostV.y = m_d3dxvVelocity.y;
//								//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;
//
//								d3dxvRevision.x = ((pCollidedVoxel->GetPosition().x + d3dxvSearchDir[i].x * pVoxelTerrain->m_fCubeWidth / 2.0f)
//									- (GetPosition().x - d3dxvSearchDir[i].x * m_pMesh->m_AABB.m_d3dxvMax.x));
//								//d3dxvPostV.x = 0.0f;
//							}
//							/*Moving(d3dxvRevision);
//							m_CameraOperator.MoveCameraOperator(d3dxvRevision);*/
//						}
//						else
//						{
//							if (i < 2)			// ��
//							{
//								/*printf("pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal (%f, %f, %f) \n",
//									pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.x,
//									pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.y,
//									pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.z);*/
//								if (0.98f < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z), &pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal))
//								{
//									m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z)) * D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z));
//									m_d3dxvVelocity.z *= pCollidedVoxel->m_fReflection;
//									//d3dxvPostV.y = m_d3dxvVelocity.y;
//									//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;
//
//									d3dxvRevision.z = ((pCollidedVoxel->GetPosition().z - d3dxvSearchDir[i].z * pVoxelTerrain->m_fCubeDepth / 2.0f)
//										- (GetPosition().z + d3dxvSearchDir[i].z * m_pMesh->m_AABB.m_d3dxvMax.z));
//									//d3dxvPostV.z = 0.0f;
//								}	
//							}
//							else                // ��
//							{
//								if (0.98f < D3DXVec3Dot(&D3DXVECTOR3(d3dxvSearchDir[i].x, 0.0f, 0.0f), &pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal))
//								{
//									m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f)) * D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f));
//									m_d3dxvVelocity.x *= pCollidedVoxel->m_fReflection;
//									//d3dxvPostV.y = m_d3dxvVelocity.y;
//									//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;
//
//									d3dxvRevision.x = ((pCollidedVoxel->GetPosition().x + d3dxvSearchDir[i].x * pVoxelTerrain->m_fCubeWidth / 2.0f)
//										- (GetPosition().x - d3dxvSearchDir[i].x * m_pMesh->m_AABB.m_d3dxvMax.x));
//								}
//							}
//						}
//						Moving(d3dxvRevision);
//						m_CameraOperator.MoveCameraOperator(d3dxvRevision);
//					}
//					else if (i < 14)
//					{
//						//
//						float fDistance = 0.0f;
//						if (!pCollidedVoxel->m_bIsSlope)
//						{
//							if (-0.2f < m_d3dxvVelocity.y && m_d3dxvVelocity.y < 0.2f)
//							{
//								m_d3dxvVelocity.y = 0.0f;
//							}
//							else
//							{
//								m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)) * D3DXVECTOR3(0.0f, 1.0f, 0.0f));
//								m_d3dxvVelocity.y *= pCollidedVoxel->m_fReflection;
//							}
//							//printf("m_d3dxvVelocity.y : %f \n", m_d3dxvVelocity.y);
//							d3dxvRevision.y = ((pCollidedVoxel->GetPosition().y + pVoxelTerrain->m_fCubeHeight / 2.0f)
//								- (GetPosition().y + m_pMesh->m_AABB.m_d3dxvMin.y));
//							if (pCollidedVoxel->m_eVoxelType == VOXEL_TYPE_SWAMP_0) d3dxvRevision.y += -0.1f;
//						}
//						else //pCollidedVoxel->m_bIsSlope)
//						{
//							fDistance = pCollidedVoxel->GetHeightByPlayerPos(GetPosition()) - (GetPosition().y + (m_pMesh->m_AABB.m_d3dxvMin.y));
//							if (fDistance < -0.0f) break;// i = 13;
//							m_d3dxvVelocity.y = 0.0f;
//							d3dxvRevision.y = fDistance;
//							if(i == 4 || i == 5) bIsSlopeVoxel = true;
//							
//						}//printf("ddd\n");
//				
//						//printf("Velocity : %f, %f, %f\n", m_d3dxvVelocity.x, m_d3dxvVelocity.y, m_d3dxvVelocity.z);
//						// �������� �����Ѵ�.
//						d3dxvVelocityXZ = -D3DXVECTOR2(m_d3dxvVelocity.x, m_d3dxvVelocity.z);
//						float fLengthVelocityXZ = D3DXVec2Length(&d3dxvVelocityXZ);
//						D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
//						float fDeceleration = pCollidedVoxel->m_fFriction * fTimeElapsed;
//						fDeceleration += PLAYER_FRICTION * fTimeElapsed;
//						if (fDeceleration > fLengthVelocityXZ) fDeceleration = fLengthVelocityXZ;
//						d3dxvVelocityXZ = d3dxvVelocityXZ * fDeceleration;
//						m_d3dxvVelocity.x += d3dxvVelocityXZ.x;
//						m_d3dxvVelocity.z += d3dxvVelocityXZ.y;
//						
//						//printf("%f \n", fDis*tance);
//						if (m_d3dxvMoveDir.y != 0.0f && pCollidedVoxel->m_eVoxelType != VOXEL_TYPE_SWAMP_0) bIsJumped = true;//m_d3dxvVelocity.y = 4.95f;
//						Moving(d3dxvRevision);
//						m_CameraOperator.MoveCameraOperator(d3dxvRevision);
//						/*printf("<<<<<<<<<<<<<<<<<COLLIDE!!!!>>>>>>>>>>>>>>>>>>>>>\n");*/
//						bIsFloor = true;
//						i = 13;
//						//break;
//					}
//					else if(i < 23)
//					{
//						m_d3dxvVelocity.y = 0.0f;
//						//d3dxvRevision.x = d3dxvRevision.z = 0.0f;
//						d3dxvRevision.y = ((pCollidedVoxel->GetPosition().y - pVoxelTerrain->m_fCubeHeight / 2.0f)
//							- (GetPosition().y + m_pMesh->m_AABB.m_d3dxvMax.y));
//						Moving(d3dxvRevision);
//						m_CameraOperator.MoveCameraOperator(d3dxvRevision);
//
//						if ((0 <= d3dxvColLayerRow.x && d3dxvColLayerRow.x < pVoxelTerrain->m_iMaxCol)
//							&& (0 <= d3dxvColLayerRow.y && d3dxvColLayerRow.y < pVoxelTerrain->m_iMaxLayer)
//							&& (0 <= d3dxvColLayerRow.z && d3dxvColLayerRow.z < pVoxelTerrain->m_iMaxRow))
//						{
//							if ((pCollidedVoxel = pVoxelTerrain->m_ppVoxelObjectsForCollision[
//								(int)((d3dxvColLayerRow.y) * pVoxelTerrain->m_iLayerStride
//									+ d3dxvColLayerRow.z * pVoxelTerrain->m_iRowStride
//									+ d3dxvColLayerRow.x)]) != NULL)
//							{
//								if (CPhysicalCollision::IsCollided(
//									&CPhysicalCollision::MoveAABB(this->m_pMesh->m_AABB, GetPosition()),
//									&CPhysicalCollision::MoveAABB(pCollidedVoxel->m_pMesh->m_AABB, pCollidedVoxel->GetPosition())))
//								{
//									if (pCollidedVoxel->m_bIsSlope)
//									{
//										D3DXVECTOR2 d3dxvFrontNormalXZ =
//											D3DXVECTOR2(pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxFrontNormal.x,
//												pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxFrontNormal.z);
//										D3DXVec2Normalize(&d3dxvFrontNormalXZ, &d3dxvFrontNormalXZ);
//										//d3dxvFrontNormalXZ * 0.1f;
//
//										d3dxvRevision.x = round(d3dxvFrontNormalXZ.x) == 0.0f ? 0.0f : ((d3dxvFrontNormalXZ.x * 0.5f + pCollidedVoxel->GetPosition().x) - GetPosition().x) * 0.9;
//										d3dxvRevision.y = 0.0f;
//										d3dxvRevision.z = round(d3dxvFrontNormalXZ.y) == 0.0f ? 0.0f : ((d3dxvFrontNormalXZ.y * 0.5f + pCollidedVoxel->GetPosition().z) - GetPosition().z) * 0.9;
//
//										round(d3dxvFrontNormalXZ.x) != 0.0f ? m_d3dxvVelocity.x = 0.0f : m_d3dxvVelocity.z = 0.0f;
//
//										printf("d3dxvFrontNormalXZ.x y : %f %f\n ", d3dxvFrontNormalXZ.x, d3dxvFrontNormalXZ.y);
//										printf("d3dxvRevision : (%f, %f, %f)\n", d3dxvRevision.x, d3dxvRevision.y, d3dxvRevision.z);
//										Moving(d3dxvRevision);
//										m_CameraOperator.MoveCameraOperator(d3dxvRevision);
//										//if (m_d3dxvMoveDir.y != 0.0f) m_d3dxvVelocity.y = 4.95f;
//
//										bIsSlopeVoxel = true;
//									}
//								}
//							}
//								bIsJumped = false;
//								break;
//						}
//						else break;
//					}
//				}
//			}
//		}
//	}
//	
//	//printf("Player Bottom : %f \n", GetPosition().y + m_pMesh->m_AABB.m_d3dxvMin.y);
//	d3dxvPostV = m_d3dxvVelocity;			
//
//	// �ܺο��� ������ �̵� �ӵ��� �����Ѵ�
//	D3DXVECTOR3 d3dxvMoveDir = D3DXVECTOR3(m_d3dxvMoveDir.x, 0.0f, m_d3dxvMoveDir.z);
//	D3DXVec3Normalize(&d3dxvMoveDir, &d3dxvMoveDir);
//	d3dxvMovingDir = PLAYER_ACCELERATION_MAGNITUDE * d3dxvMoveDir * fTimeElapsed;// +d3dxvPostV;
//
//																				   // �ִ� �ӵ��� �����Ѵ�.
//	d3dxvVelocityXZ = D3DXVECTOR2(d3dxvMovingDir.x, d3dxvMovingDir.z);
//
//	/*if (D3DXVec2Length(&d3dxvVelocityXZ) > PLAYER_MAX_VELOCITY_MAGNITUDE)
//	{
//		D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
//		d3dxvVelocityXZ *= PLAYER_MAX_VELOCITY_MAGNITUDE;
//	}*/
//	d3dxvPostV.x += d3dxvVelocityXZ.x;
//	d3dxvPostV.z += d3dxvVelocityXZ.y;
//
//	// �ӵ��� �߷°��ӵ��� �����Ѵ�.
//	d3dxvPostV += d3dxvG * fTimeElapsed;	
//
//	// �ִ� �ӵ� ����
//	if (d3dxvPostV.y > PLAYER_MAX_VELOCITY_MAGNITUDE)
//	{
//		d3dxvPostV.y = PLAYER_MAX_VELOCITY_MAGNITUDE;
//	}
//
//	//printf("[bIsFloor : %d] \n", bIsFloor);
//	if (!bIsFloor)
//	{
//		// �ٶ� ������ ����Ѵ�.
//		//d3dxvVelocityXZ = D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
//		//d3dxvVelocityXZ *= (AIR_RESISTANCE_COEFFICIENT);
//		d3dxvVelocityXZ = -D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
//		float fLengthVelocityXZ = D3DXVec2Length(&d3dxvVelocityXZ);
//		D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
//		float fDeceleration = AIR_RESISTANCE_COEFFICIENT * fTimeElapsed;
//		//d3dxvVelocityXZ *= (pCollidedVoxel->m_fDamping * PLAYER_FRICTION);
//		if (fDeceleration > fLengthVelocityXZ) fDeceleration = fLengthVelocityXZ;
//		d3dxvVelocityXZ = d3dxvVelocityXZ * fDeceleration;
//		d3dxvPostV.x += d3dxvVelocityXZ.x;
//		d3dxvPostV.z += d3dxvVelocityXZ.y;
//		//d3dxvPostV.x = d3dxvVelocityXZ.x;
//		//d3dxvPostV.z = d3dxvVelocityXZ.y;
//	}
//	
//
//	d3dxvVelocityXZ = D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
//
//	if (bIsSlopeVoxel)
//	{
//		if (D3DXVec2Length(&d3dxvVelocityXZ) > D3DXVec3Length(&D3DXVECTOR3(0.0f, d3dxvPostV.y, 0.0f)))
//		{
//			d3dxvPostV.y = -D3DXVec2Length(&d3dxvVelocityXZ);
//		}
//	}
//	if (bIsJumped)
//	{
//		d3dxvPostV.y = m_d3dxvVelocity.y = 1.95;//4.95f;// +d3dxvPostV.y;
//	}
//
//	// �ִ� �ӵ� ����
//	if (D3DXVec2Length(&d3dxvVelocityXZ) > PLAYER_MAX_VELOCITY_MAGNITUDE)
//	{
//		D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
//		d3dxvVelocityXZ *= PLAYER_MAX_VELOCITY_MAGNITUDE;	
//	}
//
//	// ���� �ӵ� ����
//	d3dxvPostV.x = d3dxvVelocityXZ.x;
//	d3dxvPostV.z = d3dxvVelocityXZ.y;
//
//	// �̵��� ����
//	d3dxvMovingDir = ((d3dxvPostV + m_d3dxvVelocity) * fTimeElapsed) / 2.0f;
//
//	// ���� �̵�
//	Moving(d3dxvMovingDir);
//	m_CameraOperator.MoveCameraOperator(d3dxvMovingDir);
//
//	// ���� �ӵ� ����
//	m_d3dxvVelocity = d3dxvPostV;
//	//printf("Player Bottome : %.6f \n", GetPosition().y - 0.5f);
//
//	// �̵� ���� �ʱ�ȭ
//	m_d3dxvMoveDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//	//d3dxvVelocityXZ = D3DXVECTOR2(m_d3dxvVelocity.x, m_d3dxvVelocity.z);
//	//printf("Player Velocity : %f\n", D3DXVec2Length(&d3dxvVelocityXZ));
//	//printf("Velocity : (%.5f, %.5f, %.5f) \n", m_d3dxvVelocity.x, m_d3dxvVelocity.y, m_d3dxvVelocity.z);
//	//printf("-----------------------------\n");
//}

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
	//�÷��̾��� ���� ī�޶��� UpdateShaderVariables() ��� �Լ��� ȣ���Ѵ�.
	// if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dDeviceContext);
	m_CameraOperator.UpdateShaderVariables(pd3dDeviceContext);
}

void CPlayer::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
	if (m_pMesh->m_AABB.m_pRenderObject)
	{
		m_pMesh->m_AABB.m_pRenderObject->SetPosition(GetPosition());
		m_pMesh->m_AABB.m_pRenderObject->Render(pd3dDeviceContext);
	}
}

void CPlayer::SetFBXAnimForType(int eAnimType)
{
	if (m_pMesh) m_pMesh->SetFBXAnimForType(eAnimType);
}

void CPlayer::Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed)
{
	if (m_pMesh) m_pMesh->Animate(pd3dDeviceContext, fTimeElapsed);
}

void CPlayer::SetMove(float x, float y, float z)
{
	move = true;

	float vx = x - GetPosition().x;
	float vy = y - GetPosition().y;
	float vz = z - GetPosition().z;

	m_LastPos = D3DXVECTOR3(x, y, z);
	m_Vel = D3DXVECTOR3(vx / 60.0, vy / 60.0, vz / 60.0);
}

void CPlayer::FramePerMove()
{
	m_d3dxmtxWorld._41 += m_Vel.x;
	m_d3dxmtxWorld._42 += m_Vel.y;
	m_d3dxmtxWorld._43 += m_Vel.z;

	if ((m_Vel.x - GetPosition().x)*(m_Vel.x - GetPosition().x) < 1.f && (m_Vel.y - GetPosition().y)*(m_Vel.y - GetPosition().y) < 1.f && (m_Vel.z - GetPosition().z)*(m_Vel.z - GetPosition().z) < 1.f)
	{
		m_d3dxmtxWorld._41 = m_LastPos.x;
		m_d3dxmtxWorld._42 = m_LastPos.y;
		m_d3dxmtxWorld._43 = m_LastPos.z;
		move = false;
		m_Vel = D3DXVECTOR3(0, 0, 0);
	}
}

//CPiratePlayer::CPiratePlayer()
//{
//
//}
//CPiratePlayer::~CPiratePlayer()
//{
//
//}
//
//void CPiratePlayer::SetFBXAnimForType(PIRATE_ANIM_TYPE)
