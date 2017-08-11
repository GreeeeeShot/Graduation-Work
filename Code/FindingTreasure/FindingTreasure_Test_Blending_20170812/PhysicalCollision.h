#pragma once

class CPlayer;
class CGameObject;

struct AABB
{
	D3DXVECTOR3 m_d3dxvMax;
	D3DXVECTOR3 m_d3dxvMin;
	CGameObject* m_pRenderObject = NULL;
};

struct PhysicalVectorQuantity
{
	D3DXVECTOR3 m_d3dxvMoveF;		// 앞으로 가는 힘
	float m_fMass;					// 케릭터의 질량
	D3DXVECTOR3 m_d3dxvVelocity;	// 케릭터 속도
	float m_fJumpdVelocityY;			// 점프할 때의 순간 속도
};

class CPhysicalCollision
{
public:
	CPhysicalCollision();
	~CPhysicalCollision();

	static AABB UpdateAABB(AABB aabb, D3DXMATRIX *pd3dxmtxToWorld);
	static AABB MoveAABB(AABB aabb, D3DXVECTOR3 d3dxvShift);
	static bool IsCollided(AABB *pObj, D3DXVECTOR3 d3dxvPos);
	static bool IsCollided(AABB *pObj1, AABB *pObj2);
	static void ImpurseBasedCollisionResolution(CPlayer *pPlayerA, CPlayer *pPlayerB);
	static void FloatOnWater(CPlayer *pPlayer, float fElapsedTime = 0.0f, float fWaterY = 0.5f, float fDensity = 1.0f, float fAreaOffset = 5.0f, float fDrag = 0.05f);
};

