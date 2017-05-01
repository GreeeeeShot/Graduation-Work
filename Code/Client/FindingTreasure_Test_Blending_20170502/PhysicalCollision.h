#pragma once

class CPlayer;

struct AABB
{
	D3DXVECTOR3 m_d3dxvMax;
	D3DXVECTOR3 m_d3dxvMin;
};

class CPhysicalCollision
{
public:
	CPhysicalCollision();
	~CPhysicalCollision();

	static AABB UpdateAABB(AABB aabb, D3DXMATRIX *pd3dxmtxToWorld);
	static AABB MoveAABB(AABB aabb, D3DXVECTOR3 d3dxvShift);
	static bool IsCollided(AABB *pObj1, AABB *pObj2);
	static void ImpurseBasedCollisionResolution(CPlayer *pPlayerA, CPlayer *pPlayerB);
};

