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

void CPlayer::RotateMoveDir(void)
{
	if ((-0.01f < m_d3dxvMoveDir.x && m_d3dxvMoveDir.x < 0.01f) && (-0.01f < m_d3dxvMoveDir.y && m_d3dxvMoveDir.y < 0.01f) && (-0.01f < m_d3dxvMoveDir.z && m_d3dxvMoveDir.z < 0.01f)) return;
	/*printf("Moving Dir : (%lf, %lf, %lf) \n", d3dxvMovingDir.x, d3dxvMovingDir.y, d3dxvMovingDir.z);*/

	D3DXVec3Normalize(&m_d3dxvMoveDir, &m_d3dxvMoveDir);
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

	D3DXVec3TransformNormal(&m_d3dxvMoveDir, &m_d3dxvMoveDir, &mtxRotate);		// 보정된 위치 벡터
	D3DXVec3Cross(&d3dxvCross, &GetLook(), &m_d3dxvMoveDir);
	D3DXVec3Normalize(&d3dxvCross, &d3dxvCross);

	fDot = D3DXVec3Dot(&GetLook(), &m_d3dxvMoveDir);
	fRotationRadianForProofreading = acosf(fDot);
	fRotationRadianForProofreading = d3dxvCross.y > 0.0f ? fRotationRadianForProofreading : -fRotationRadianForProofreading;
	fRotationRadianForProofreading = D3DXToRadian(D3DXToDegree(fRotationRadianForProofreading) / PLAYER_SOFT_ROTATION_INTERVAL_NUM);
	
	D3DXMatrixRotationAxis(&mtxRotate, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), fRotationRadianForProofreading);
	/*printf("Prev Player Position : (%lf, %lf, %lf) \n", GetPosition().x, GetPosition().y, GetPosition().z);*/
	D3DXMatrixMultiply(&m_d3dxmtxWorld, &mtxRotate, &m_d3dxmtxWorld);
	/*printf("Post Player Position : (%lf, %lf, %lf) \n", GetPosition().x, GetPosition().y, GetPosition().z);
	printf("World Position : (%lf, %lf, %lf) \n", m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43);
	printf("_________________________________________ \n");*/
}

void CPlayer::MovingUnderPhysicalEnvironment(CScene *pScene, CVoxelTerrain *pVoxelTerrain, float fTimeElapsed)
{
	static D3DXVECTOR3 d3dxvSearchDir[14] =
	{
		D3DXVECTOR3(0.0f, 0.0f, 1.0f),					// 상
		D3DXVECTOR3(0.0f, 0.0f, -1.0f),					// 하
		D3DXVECTOR3(1.0f, 0.0f, 0.0f),					// 우
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f),					// 좌
		
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
		D3DXVECTOR3(1.0f, -1.0f, -1.0f),   D3DXVECTOR3(0.0f, -1.0f, -1.0f),   D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
		D3DXVECTOR3(1.0f, -1.0f, 0.0f),      D3DXVECTOR3(0.0f, -1.0f, 0.0f),      D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(1.0f, -1.0f, 1.0f),      D3DXVECTOR3(0.0f, -1.0f, 1.0f),      D3DXVECTOR3(-1.0f, -1.0f, 1.0f)
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


	for (int i = 0; i < 14; i++)
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
					D3DXVECTOR3 d3dxvRevision(0.0f, 0.0f, 0.0f);

					if (i < 4)
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
						Moving(d3dxvRevision);
						m_CameraOperator.MoveCameraOperator(d3dxvRevision);
					}
					else if (i < 14)
					{
						m_d3dxvVelocity = m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)) * D3DXVECTOR3(0.0f, 1.0f, 0.0f));
						m_d3dxvVelocity.y *= pCollidedVoxel->m_fReflection;

						// 마찰력을 적용한다.
						d3dxvVelocityXZ = D3DXVECTOR2(m_d3dxvVelocity.x, m_d3dxvVelocity.z);
						d3dxvVelocityXZ *= (pCollidedVoxel->m_fDamping * PLAYER_DAMPING_VALUE);
						m_d3dxvVelocity.x = d3dxvVelocityXZ.x;
						m_d3dxvVelocity.z = d3dxvVelocityXZ.y;

						//printf("y V : %f\n", d3dxvPostV.y);

						d3dxvRevision.y = ((pCollidedVoxel->GetPosition().y + pVoxelTerrain->m_fCubeHeight / 2.0f)
							- (GetPosition().y + m_pMesh->m_AABB.m_d3dxvMin.y));
						Moving(d3dxvRevision);
						m_CameraOperator.MoveCameraOperator(d3dxvRevision);
						/*printf("<<<<<<<<<<<<<<<<<COLLIDE!!!!>>>>>>>>>>>>>>>>>>>>>\n");*/

						i = 13;
					}
				}
			}
		}
	}

	d3dxvPostV = m_d3dxvVelocity;			

	// 외부에서 가해진 이동 속도를 갱신한다
	D3DXVec3Normalize(&m_d3dxvMoveDir, &m_d3dxvMoveDir);
	d3dxvMovingDir = PLAYER_ACCELERATION_MAGNITUDE * m_d3dxvMoveDir * fTimeElapsed;// +d3dxvPostV;

																				   // 최대 속도를 제한한다.
	d3dxvVelocityXZ = D3DXVECTOR2(d3dxvMovingDir.x, d3dxvMovingDir.z);

	if (D3DXVec2Length(&d3dxvVelocityXZ) > PLAYER_MAX_VELOCITY_MAGNITUDE)
	{
		D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
		d3dxvVelocityXZ *= PLAYER_MAX_VELOCITY_MAGNITUDE;
	}
	d3dxvPostV.x += d3dxvVelocityXZ.x;
	d3dxvPostV.z += d3dxvVelocityXZ.y;

	// 속도에 중력가속도를 적용한다.
	d3dxvPostV = d3dxvG * fTimeElapsed + d3dxvPostV;
	// 바람 저항을 계산한다.
	d3dxvVelocityXZ = D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
	d3dxvVelocityXZ *= (AIR_RESISTANCE_COEFFICIENT * PLAYER_DAMPING_VALUE);
	//d3dxvPostV.x = d3dxvVelocityXZ.x;
	//d3dxvPostV.z = d3dxvVelocityXZ.y;

	//d3dxvVelocityXZ = D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
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

	// 이동 방향 초기화
	m_d3dxvMoveDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	d3dxvVelocityXZ = D3DXVECTOR2(m_d3dxvVelocity.x, m_d3dxvVelocity.z);
	/*printf("Player Velocity : %f\n", D3DXVec2Length(&d3dxvVelocityXZ));
	printf("Velocity : (%f, %f, %f) \n", m_d3dxvVelocity.x, m_d3dxvVelocity.y, m_d3dxvVelocity.z);
	printf("-----------------------------\n");*/
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
