#include "stdafx.h"
#include "Voxel.h"

CVoxel::~CVoxel()
{
}

float CVoxel::GetHeightByPlayerPos(D3DXVECTOR3 d3dxvPlayerPos)
{
	float fU, fV;
	D3DXVECTOR3 p0, p1, p2;		// p0 : Up 라인 보간 점, p1 : Down 라인 보간 점, p2 플레이어가 위치해야 될 SlopeFace 위 최종 점.

	D3DXVECTOR3 d3dxvMovedDownNegative;
	D3DXVECTOR3 d3dxvMovedDownPositive;
	D3DXVECTOR3 d3dxvMovedUpNegative;
	D3DXVECTOR3 d3dxvMovedUpPositive;

	D3DXVec3TransformCoord(&d3dxvMovedDownNegative, &m_pMesh->m_CubeSlopeFace.m_d3dxvDownNegative, &m_d3dxmtxWorld);
	D3DXVec3TransformCoord(&d3dxvMovedDownPositive, &m_pMesh->m_CubeSlopeFace.m_d3dxvDownPositive, &m_d3dxmtxWorld);
	D3DXVec3TransformCoord(&d3dxvMovedUpNegative, &m_pMesh->m_CubeSlopeFace.m_d3dxvUpNegative, &m_d3dxmtxWorld);
	D3DXVec3TransformCoord(&d3dxvMovedUpPositive, &m_pMesh->m_CubeSlopeFace.m_d3dxvUpPositive, &m_d3dxmtxWorld);

	if (m_eVoxelDir == VOXEL_SLOPE_DIR_FRONT || m_eVoxelDir == VOXEL_SLOPE_DIR_REAR)
	{
		fU = (d3dxvPlayerPos.x - d3dxvMovedDownNegative.x) / (d3dxvMovedDownPositive.x - d3dxvMovedDownNegative.x);
		fV = (d3dxvPlayerPos.z - d3dxvMovedDownPositive.z) / (d3dxvMovedUpPositive.z - d3dxvMovedDownPositive.z);

		/*printf("PlayerPos : (%f %f %f) \n", d3dxvPlayerPos.x, d3dxvPlayerPos.y, d3dxvPlayerPos.z);
		printf("d3dxvMovedDownNegative : (%f %f %f) \n", d3dxvMovedDownNegative.x, d3dxvMovedDownNegative.y, d3dxvMovedDownNegative.z);
		printf("d3dxvMovedDownPositive : (%f %f %f) \n", d3dxvMovedDownPositive.x, d3dxvMovedDownPositive.y, d3dxvMovedDownPositive.z);
		printf("d3dxvMovedUpNegative : (%f %f %f) \n", d3dxvMovedUpNegative.x, d3dxvMovedUpNegative.y, d3dxvMovedUpNegative.z);
		printf("d3dxvMovedUpPositive : (%f %f %f) \n", d3dxvMovedUpPositive.x, d3dxvMovedUpPositive.y, d3dxvMovedUpPositive.z);
	
		printf("fU %f, fV %f \n", fU, fV);*/
		p0 = d3dxvMovedUpNegative * (1.0f - fU) + d3dxvMovedUpPositive * fU;
		p1 = d3dxvMovedDownNegative * (1.0f - fU) + d3dxvMovedDownPositive * fU;
		p2 = p1 * (1.0f - fV) + p0 * fV;
	}
	else
	{
		fU = (d3dxvPlayerPos.x - d3dxvMovedDownNegative.x) / (d3dxvMovedUpNegative.x - d3dxvMovedDownNegative.x);
		fV = (d3dxvPlayerPos.z - d3dxvMovedDownPositive.z) / (d3dxvMovedDownNegative.z - d3dxvMovedDownPositive.z);

		/*printf("PlayerPos : (%f %f %f) \n", d3dxvPlayerPos.x, d3dxvPlayerPos.y, d3dxvPlayerPos.z);
		printf("d3dxvMovedDownNegative : (%f %f %f) \n", d3dxvMovedDownNegative.x, d3dxvMovedDownNegative.y, d3dxvMovedDownNegative.z);
		printf("d3dxvMovedDownPositive : (%f %f %f) \n", d3dxvMovedDownPositive.x, d3dxvMovedDownPositive.y, d3dxvMovedDownPositive.z);
		printf("d3dxvMovedUpNegative : (%f %f %f) \n", d3dxvMovedUpNegative.x, d3dxvMovedUpNegative.y, d3dxvMovedUpNegative.z);
		printf("d3dxvMovedUpPositive : (%f %f %f) \n", d3dxvMovedUpPositive.x, d3dxvMovedUpPositive.y, d3dxvMovedUpPositive.z);

		printf("fU %f, fV %f \n", fU, fV);*/
		p0 = d3dxvMovedDownNegative * (1.0f - fU) + d3dxvMovedUpNegative * fU;
		p1 = d3dxvMovedDownPositive * (1.0f - fU) + d3dxvMovedUpPositive * fU;
		p2 = p0 * (1.0f - fV) +  p1* fV;
	}

	printf("Height : %f", p2.y);

	return p2.y;
}