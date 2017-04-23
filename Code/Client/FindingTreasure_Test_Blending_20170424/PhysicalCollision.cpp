#include "stdafx.h"
#include "Player.h"
#include "PhysicalCollision.h"


CPhysicalCollision::CPhysicalCollision()
{
}


CPhysicalCollision::~CPhysicalCollision()
{
}

AABB CPhysicalCollision::UpdateAABB(AABB aabb, D3DXMATRIX *pd3dxmtxToWorld)
{
}

AABB CPhysicalCollision::MoveAABB(AABB aabb, D3DXVECTOR3 d3dxvShift)
{
	AABB temp;

	temp.m_d3dxvMax = aabb.m_d3dxvMax + d3dxvShift;
	temp.m_d3dxvMin = aabb.m_d3dxvMin + d3dxvShift;

	return temp;
}

bool CPhysicalCollision::IsCollided(AABB *pObj1, AABB *pObj2)
{
	if (pObj1->m_d3dxvMin.x >= pObj2->m_d3dxvMax.x) return false;
	if (pObj1->m_d3dxvMin.y >= pObj2->m_d3dxvMax.y) return false;
	if (pObj1->m_d3dxvMin.z >= pObj2->m_d3dxvMax.z) return false;
	if (pObj1->m_d3dxvMax.x <= pObj2->m_d3dxvMin.x) return false;
	if (pObj1->m_d3dxvMax.y <= pObj2->m_d3dxvMin.y) return false;
	if (pObj1->m_d3dxvMax.z <= pObj2->m_d3dxvMin.z) return false;

	return true;
}

void CPhysicalCollision::ImpurseBasedCollisionResolution(CPlayer *pPlayerA, CPlayer *pPlayerB)
{
	if (pPlayerA == NULL || pPlayerB == NULL) return;

	// 충돌을 확인한다.
	float fDistance = D3DXVec3Length(&(pPlayerA->GetPosition() - pPlayerB->GetPosition()));
	float fRadiusA = pPlayerA->m_pMesh->m_AABB.m_d3dxvMax.x;
	float fRadiusB = pPlayerB->m_pMesh->m_AABB.m_d3dxvMax.x;

	if (fDistance >= fRadiusA + fRadiusB) return;

	D3DXVECTOR3 d3dxvToB = pPlayerB->GetPosition() - pPlayerA->GetPosition();

	D3DXVec3Normalize(&d3dxvToB, &d3dxvToB);

	D3DXVECTOR3 d3dxvProofDistance = D3DXVECTOR3(-((fRadiusA * d3dxvToB + fRadiusB * d3dxvToB) - fDistance * d3dxvToB));
	pPlayerA->Moving(d3dxvProofDistance);
	pPlayerA->m_CameraOperator.MoveCameraOperator(d3dxvProofDistance);

	D3DXVECTOR3 d3dxvContactNormal;
	D3DXVECTOR3 d3dxvNormalVelocity;

	// 접촉 노멀을 구한다.
	D3DXVec3Normalize(&d3dxvContactNormal, &(pPlayerB->GetPosition() - pPlayerA->GetPosition()));

	// 노멀 속도를 구한다.
	d3dxvNormalVelocity = D3DXVec3Dot(&pPlayerA->m_d3dxvVelocity, &d3dxvContactNormal) * d3dxvContactNormal;

	D3DXVECTOR3 d3dxvJ = -(pPlayerA->m_d3dxvVelocity - pPlayerB->m_d3dxvVelocity) * (1.0f + 1.0f) * (PLAYER_MASS * (PLAYER_MASS / 2.8f) / (PLAYER_MASS + PLAYER_MASS/ 2.8f));

	pPlayerA->m_d3dxvVelocity = d3dxvJ * (1.0f / PLAYER_MASS) + pPlayerA->m_d3dxvVelocity;
	pPlayerB->m_d3dxvVelocity = -d3dxvJ * (1.0f / (PLAYER_MASS/ 2.8f) ) + pPlayerB->m_d3dxvVelocity;
}