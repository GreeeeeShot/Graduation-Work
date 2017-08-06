#include "stdafx.h"
#include "Player.h"
#include "PhysicalCollision.h"
#include "EffectManager.h"


CPhysicalCollision::CPhysicalCollision()
{
}


CPhysicalCollision::~CPhysicalCollision()
{
}

AABB CPhysicalCollision::UpdateAABB(AABB aabb, D3DXMATRIX *pd3dxmtxToWorld)
{
	return aabb;
}

AABB CPhysicalCollision::MoveAABB(AABB aabb, D3DXVECTOR3 d3dxvShift)
{
	AABB temp;

	temp.m_d3dxvMax = aabb.m_d3dxvMax + d3dxvShift;
	temp.m_d3dxvMin = aabb.m_d3dxvMin + d3dxvShift;

	return temp;
}

bool CPhysicalCollision::IsCollided(AABB *pObj, D3DXVECTOR3 d3dxvPos)
{
	if (d3dxvPos.x <= pObj->m_d3dxvMin.x || d3dxvPos.x >= pObj->m_d3dxvMax.x) return false;
	if (d3dxvPos.y <= pObj->m_d3dxvMin.y || d3dxvPos.y >= pObj->m_d3dxvMax.y) return false;
	if (d3dxvPos.z <= pObj->m_d3dxvMin.z || d3dxvPos.z >= pObj->m_d3dxvMax.z) return false;
	
	return true;
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
	//printf("pPlayerA Pos : %f %f %f \n", pPlayerA->GetPosition().x, pPlayerA->GetPosition().y, pPlayerA->GetPosition().z);
	float fRadiusA = 0.3f;//pPlayerA->m_pMesh->m_AABB.m_d3dxvMax.x;
	float fRadiusB = 0.3f;//pPlayerB->m_pMesh->m_AABB.m_d3dxvMax.x;

	if (fDistance > fRadiusA + fRadiusB) return;

	D3DXVECTOR3 d3dxvToB = pPlayerB->GetPosition() - pPlayerA->GetPosition();
	CEffectManager::RegisterEffectManager(new CCollisionParticleEffect(pPlayerA, pPlayerA->GetPosition() - d3dxvToB / 2.0f));
	CEffectManager::RegisterEffectManager(new CCollisionParticleEffect(pPlayerB, pPlayerB->GetPosition() + d3dxvToB / 2.0f));

	D3DXVec3Normalize(&d3dxvToB, &d3dxvToB);

	D3DXVECTOR3 d3dxvProofDistance = D3DXVECTOR3(-((fRadiusA * d3dxvToB + fRadiusB * d3dxvToB) - fDistance * d3dxvToB));
	pPlayerA->Moving(d3dxvProofDistance);
	pPlayerA->m_CameraOperator.MoveCameraOperator(d3dxvProofDistance);

	D3DXVECTOR3 d3dxvContactNormal;
	D3DXVECTOR3 d3dxvNormalVelocity;

	// 접촉 노멀을 구한다.
	D3DXVec3Normalize(&d3dxvContactNormal, &(pPlayerB->GetPosition() - pPlayerA->GetPosition()));

	// 노멀 속도를 구한다.
	D3DXVECTOR2 d3dxvRevisionVelocity;
	printf("pPlayerA %f %f %f \n", pPlayerA->m_d3dxvVelocity.x, pPlayerA->m_d3dxvVelocity.y, pPlayerA->m_d3dxvVelocity.z);
	printf("pPlayerB %f %f %f \n", pPlayerB->m_d3dxvVelocity.x, pPlayerB->m_d3dxvVelocity.y, pPlayerB->m_d3dxvVelocity.z);
	if (fabs(pPlayerA->m_d3dxvVelocity.x) <= 0.1f && fabs(pPlayerA->m_d3dxvVelocity.z) <= 0.1f)
	{
		d3dxvRevisionVelocity = D3DXVECTOR2(pPlayerB->GetPosition().x - pPlayerA->GetPosition().x, pPlayerB->GetPosition().z - pPlayerA->GetPosition().z);
		D3DXVec2Normalize(&d3dxvRevisionVelocity, &d3dxvRevisionVelocity);
		pPlayerA->m_d3dxvVelocity.x = 3.0f * (d3dxvRevisionVelocity.x);
		pPlayerA->m_d3dxvVelocity.z = 3.0f * (d3dxvRevisionVelocity.y);

		printf("pPlayerA V: %f %f \n", pPlayerA->m_d3dxvVelocity.x, pPlayerA->m_d3dxvVelocity.z);
	}

	if (fabs(pPlayerB->m_d3dxvVelocity.x) <= 0.1f && fabs(pPlayerB->m_d3dxvVelocity.z) <= 0.1f)
	{
		d3dxvRevisionVelocity = D3DXVECTOR2(pPlayerA->GetPosition().x - pPlayerB->GetPosition().x, pPlayerA->GetPosition().z - pPlayerB->GetPosition().z);
		D3DXVec2Normalize(&d3dxvRevisionVelocity, &d3dxvRevisionVelocity);
		pPlayerB->m_d3dxvVelocity.x = 3.0f * (d3dxvRevisionVelocity.x);
		pPlayerB->m_d3dxvVelocity.z = 3.0f * (d3dxvRevisionVelocity.y);
		printf("pPlayerB V: %f %f \n", pPlayerB->m_d3dxvVelocity.x, pPlayerB->m_d3dxvVelocity.z);
		
	}

	d3dxvNormalVelocity = D3DXVec3Dot(&pPlayerA->m_d3dxvVelocity, &d3dxvContactNormal) * d3dxvContactNormal;

	/*D3DXVECTOR3 d3dxvRevisionVelocity = d3dxvNormalVelocity;

	d3dxvRevisionVelocity.y = d3dxvNormalVelocity.y < 0 ? -d3dxvNormalVelocity.y : d3dxvNormalVelocity.y;*/

	/*float fDot = D3DXVec3Dot(&d3dxvRevisionVelocity, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	if (fDot >= 1.0f) fDot = 1.0f;
	else if (fDot <= -1.0f) fDot = -1.0f;*/

	D3DXVECTOR3 d3dxvJ = -(pPlayerA->m_d3dxvVelocity - pPlayerB->m_d3dxvVelocity) * (1.0f + 4.5f) * (PLAYER_MASS * (PLAYER_MASS) / (PLAYER_MASS + (PLAYER_MASS)));

	pPlayerA->m_d3dxvVelocity = d3dxvJ * (1.0f / PLAYER_MASS) + pPlayerA->m_d3dxvVelocity;
	pPlayerB->m_d3dxvVelocity = -d3dxvJ * (1.0f / (PLAYER_MASS) ) + pPlayerB->m_d3dxvVelocity;

	if (pPlayerA->m_d3dxvVelocity.y > PLAYER_MAX_VELOCITY_MAGNITUDE) pPlayerA->m_d3dxvVelocity.y = PLAYER_MAX_VELOCITY_MAGNITUDE;
	if (pPlayerB->m_d3dxvVelocity.y > PLAYER_MAX_VELOCITY_MAGNITUDE) pPlayerB->m_d3dxvVelocity.y = PLAYER_MAX_VELOCITY_MAGNITUDE;

	pPlayerA->m_pLiftingPlayer = pPlayerB->m_pLiftingPlayer = NULL;
}