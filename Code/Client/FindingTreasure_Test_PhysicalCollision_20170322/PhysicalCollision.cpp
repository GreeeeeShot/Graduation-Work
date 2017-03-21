#include "stdafx.h"
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
	if (pObj1->m_d3dxvMin.x > pObj2->m_d3dxvMax.x) return false;
	if (pObj1->m_d3dxvMin.y > pObj2->m_d3dxvMax.y) return false;
	if (pObj1->m_d3dxvMin.z > pObj2->m_d3dxvMax.z) return false;
	if (pObj1->m_d3dxvMax.x < pObj2->m_d3dxvMin.x) return false;
	if (pObj1->m_d3dxvMax.y < pObj2->m_d3dxvMin.y) return false;
	if (pObj1->m_d3dxvMax.z < pObj2->m_d3dxvMin.z) return false;

	return true;
}