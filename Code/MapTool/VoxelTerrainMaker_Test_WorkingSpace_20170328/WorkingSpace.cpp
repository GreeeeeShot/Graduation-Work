#include "stdafx.h"
#include "VoxelTerrain.h"
#include "WorkingSpace.h"


CWorkingSpace::CWorkingSpace()
{
	m_nMaxLayer = 0;
	m_nMaxCol = 0;
	m_nMaxRow = 0;
	m_fCubeWidth = 0;
	m_fCubeHeight = 0;
	m_fCubeDepth = 0;
	m_fOffsetXFromLocal = 0;
	m_fOffsetYFromLocal = 0;
	m_fOffsetZFromLocal = 0;

	m_pLocalCoord = NULL;
	m_pWorldCoord = NULL;
	m_pWorkingSpace = NULL;
	m_pWorkingBoard = NULL;

	m_ppVoxelObjects = NULL;
	D3DXMatrixIdentity(&m_d3dxmtxFromIdxToPos);

	ZeroMemory(m_iBlueprint, sizeof(int) * BLUEPRINT_LAYER * BLUEPRINT_ROW * BLUEPRINT_COL);
}


CWorkingSpace::~CWorkingSpace()
{
	if(m_pLocalCoord) delete m_pLocalCoord;
	if(m_pWorldCoord) delete m_pWorldCoord;
	if(m_pWorkingSpace) delete m_pWorkingSpace;
	if(m_pWorkingBoard) delete m_pWorkingBoard;

	if(m_ppVoxelObjects) delete[]m_ppVoxelObjects;
}

void CWorkingSpace::CreateWorkingSpace(ID3D11Device *pd3dDevice, D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal)
{
	m_nMaxLayer = d3dxvWorkingSpaceSize.y;
	m_nMaxCol = d3dxvWorkingSpaceSize.x;
	m_nMaxRow = d3dxvWorkingSpaceSize.z;
	m_fCubeWidth = d3dxvCubeSize.x;
	m_fCubeHeight = d3dxvCubeSize.y;
	m_fCubeDepth = d3dxvCubeSize.z;
	m_fOffsetXFromLocal = d3dxvOffsetFromLocal.x;
	m_fOffsetYFromLocal = d3dxvOffsetFromLocal.y;
	m_fOffsetZFromLocal = d3dxvOffsetFromLocal.z;

	CShader *pDiffusedShader = new CShader();
	pDiffusedShader->CreateShader(pd3dDevice);
	
	m_pLocalCoord = new CGameObject();
	m_pLocalCoord->SetMesh(CMeshResource::pLocalCoordMesh);
	m_pLocalCoord->SetShader(pDiffusedShader);

	m_pWorldCoord = new CGameObject();
	m_pWorldCoord->SetMesh(CMeshResource::pWorldCoordMesh);
	m_pWorldCoord->SetShader(pDiffusedShader);

	m_pWorkingSpace = new CGameObject();
	CMesh *pWorkingSpace = new CDiffusedWorkingSpaceMesh(pd3dDevice, m_nMaxCol * m_fCubeWidth, m_nMaxLayer * m_fCubeHeight, m_nMaxRow * m_fCubeDepth);
	m_pWorkingSpace->SetMesh(pWorkingSpace);
	m_pWorkingSpace->SetShader(pDiffusedShader);
	
	m_pWorkingBoard = new CGameObject();
	CMesh *pWorkingBoard = new CDiffusedWorkingBoardMesh(pd3dDevice, m_nMaxCol, m_fCubeWidth, m_nMaxRow, m_fCubeDepth);
	m_pWorkingBoard->SetMesh(pWorkingBoard);
	m_pWorkingBoard->SetShader(pDiffusedShader);

	m_d3dxmtxFromIdxToPos._11 = -m_fCubeWidth;
	m_d3dxmtxFromIdxToPos._22 = m_fCubeHeight;
	m_d3dxmtxFromIdxToPos._33 = m_fCubeDepth;

	m_d3dxmtxFromIdxToPos._41 = (m_nMaxCol - 1) * m_fCubeWidth / 2.0f + m_fOffsetXFromLocal;
	// m_d3dxmtxFromIdxToPos._42 = -(m_iMaxLayer - 1) * m_fCubeHeight / 2.0f + m_fOffsetYFromLocal;
	m_d3dxmtxFromIdxToPos._42 = m_fCubeHeight / 2.0f + m_fOffsetYFromLocal;
	m_d3dxmtxFromIdxToPos._43 = -(m_nMaxRow - 1) * m_fCubeDepth / 2.0f + m_fOffsetZFromLocal;
}

CBlueprint* CWorkingSpace::CreateBlueprint(void)
{
	CBlueprint* bp = new CBlueprint();

	//int m_iBlueprint[BLUEPRINT_LAYER][BLUEPRINT_ROW][BLUEPRINT_COL];
	//int m_nVoxelObjectsNum;
	bp->m_nMaxLayer = m_nMaxLayer;
	bp->m_nMaxCol = m_nMaxCol;
	bp->m_nMaxRow = m_nMaxRow;
	bp->m_fCubeWidth = m_fCubeWidth;
	bp->m_fCubeHeight = m_fCubeHeight;
	bp->m_fCubeDepth = m_fCubeDepth;
	bp->m_fOffsetXFromLocal = m_fOffsetXFromLocal;
	bp->m_fOffsetYFromLocal = m_fOffsetYFromLocal;
	bp->m_fOffsetZFromLocal = m_fOffsetZFromLocal;

	for (int layer = 0; layer < m_nMaxLayer; layer++)
	{
		for (int row = 0; row < m_nMaxRow; row++)
		{
			for (int col = 0; col < m_nMaxCol; col++)
			{
				if (bp->m_iBlueprint[layer][row][col] = m_iBlueprint[layer][row][col])
				{
					bp->m_nVoxelObjectsNum++;
				}
			}
		}
	}

	return bp;
}

void CWorkingSpace::MoveUpWorkingBoard(void)
{
	if (m_nMaxLayer * m_fCubeHeight > m_pWorkingBoard->GetPosition().y + m_fCubeHeight)
	{
		m_pWorkingBoard->m_d3dxmtxWorld._42 = m_pWorkingBoard->GetPosition().y + m_fCubeHeight;
	}
	//printf("m_pWorkingBoard->m_d3dxmtxWorld._42 : %f \n", m_pWorkingBoard->m_d3dxmtxWorld._42);
}

void CWorkingSpace::MoveDownWorkingBoard(void)
{
	if (0.0f >= m_pWorkingBoard->GetPosition().y) return;

	m_pWorkingBoard->m_d3dxmtxWorld._42 = m_pWorkingBoard->GetPosition().y - m_fCubeHeight;
	//printf("m_pWorkingBoard->m_d3dxmtxWorld._42 : %f \n", m_pWorkingBoard->m_d3dxmtxWorld._42);
}

D3DXVECTOR3 CWorkingSpace::CreateRayVector(CCamera *pCamera, int iMouseX, int iMouseY)
{
	D3DXVECTOR4 v4RaysStart(0, 0, 0, 1);
	D3DXVECTOR4 v4RaysDirect(iMouseX, iMouseY, 0, 1);
	D3DXMATRIX mtxScreenInv;
	D3DXMATRIX mtxProjInv;
	D3DXMATRIX mtxViewInv;
	D3DXMATRIX mtxMergInv;

	D3DXMatrixInverse(&mtxScreenInv, NULL, &pCamera->GetScreenMatrix());
	D3DXMatrixInverse(&mtxProjInv, NULL, &pCamera->GetProjectionMatrix());
	D3DXMatrixInverse(&mtxViewInv, NULL, &pCamera->GetViewMatrix());

	D3DXMatrixMultiply(&mtxMergInv, &mtxScreenInv, &mtxProjInv);

	D3DXVec3Transform(&v4RaysDirect, &(D3DXVECTOR3)v4RaysDirect, &mtxMergInv);
	v4RaysDirect.x /= v4RaysDirect.w; v4RaysDirect.y /= v4RaysDirect.w;
	v4RaysDirect.z /= v4RaysDirect.w; v4RaysDirect.w /= v4RaysDirect.w;
	v4RaysDirect -= v4RaysStart;

	D3DXVec3Transform(&v4RaysStart, &(D3DXVECTOR3)v4RaysStart, &mtxViewInv);
	D3DXVECTOR3 v3RaysStart(v4RaysStart.x, v4RaysStart.y, v4RaysStart.z);

	D3DXVECTOR3 v3RaysDirect;
	D3DXVec3TransformNormal(&v3RaysDirect, &(D3DXVECTOR3)v4RaysDirect, &mtxViewInv);

	D3DXVec3Normalize(&v3RaysDirect, &v3RaysDirect);

	return v3RaysDirect;
}

void CWorkingSpace::CreateVoxelObject(ID3D11Device *pd3dDevice, CCamera *pCamera, int iMouseX, int iMouseY)
{
	//D3DXVECTOR3 d3dxvRayVector = CreateRayVector(pCamera, iMouseX, iMouseY);

	D3DXVECTOR4 v4RaysStart(0, 0, 0, 1);
	D3DXVECTOR4 v4RaysDirect(iMouseX, iMouseY, 0, 1);
	D3DXMATRIX mtxScreenInv;
	D3DXMATRIX mtxProjInv;
	D3DXMATRIX mtxViewInv;
	D3DXMATRIX mtxMergInv;

	D3DXMatrixInverse(&mtxScreenInv, NULL, &pCamera->GetScreenMatrix());
	D3DXMatrixInverse(&mtxProjInv, NULL, &pCamera->GetProjectionMatrix());
	D3DXMatrixInverse(&mtxViewInv, NULL, &pCamera->GetViewMatrix());

	D3DXMatrixMultiply(&mtxMergInv, &mtxScreenInv, &mtxProjInv);

	D3DXVec3Transform(&v4RaysDirect, &(D3DXVECTOR3)v4RaysDirect, &mtxMergInv);
	v4RaysDirect.x /= v4RaysDirect.w; v4RaysDirect.y /= v4RaysDirect.w;
	v4RaysDirect.z /= v4RaysDirect.w; v4RaysDirect.w /= v4RaysDirect.w;
	v4RaysDirect -= v4RaysStart;

	D3DXVec3Transform(&v4RaysStart, &(D3DXVECTOR3)v4RaysStart, &mtxViewInv);
	D3DXVECTOR3 v3RaysStart(v4RaysStart.x, v4RaysStart.y, v4RaysStart.z);

	D3DXVECTOR3 v3RaysDirect;
	D3DXVec3TransformNormal(&v3RaysDirect, &(D3DXVECTOR3)v4RaysDirect, &mtxViewInv);

	D3DXVec3Normalize(&v3RaysDirect, &v3RaysDirect);

	AABB MovedAABB =
		CPhysicalCollision::MoveAABB(
			m_pWorkingBoard->m_pMesh->m_AABB,
			D3DXVECTOR3(0.0f, m_pWorkingBoard->GetPosition().y, 0.0f));
	printf("m_pWorkingBoard->GetPosition().y : %f, MovedAABB.m_d3dxvMax.y : %f, MovedAABB.m_d3dxvMin.y : %f \n", m_pWorkingBoard->GetPosition().y, MovedAABB.m_d3dxvMax.y, MovedAABB.m_d3dxvMin.y);

	D3DXVECTOR3 d3dxvUpP1 = D3DXVECTOR3(MovedAABB.m_d3dxvMin.x, MovedAABB.m_d3dxvMin.y, MovedAABB.m_d3dxvMax.z);
	D3DXVECTOR3 d3dxvUpP2 = D3DXVECTOR3(MovedAABB.m_d3dxvMax.x, MovedAABB.m_d3dxvMax.y, MovedAABB.m_d3dxvMax.z);
	D3DXVECTOR3 d3dxvUpP3 = D3DXVECTOR3(MovedAABB.m_d3dxvMin.x, MovedAABB.m_d3dxvMin.y, MovedAABB.m_d3dxvMin.z);

	D3DXVECTOR3 d3dxvDownP1 = D3DXVECTOR3(MovedAABB.m_d3dxvMax.x, MovedAABB.m_d3dxvMin.y, MovedAABB.m_d3dxvMin.z);
	D3DXVECTOR3 d3dxvDownP2 = D3DXVECTOR3(MovedAABB.m_d3dxvMin.x, MovedAABB.m_d3dxvMin.y, MovedAABB.m_d3dxvMin.z);
	D3DXVECTOR3 d3dxvDownP3 = D3DXVECTOR3(MovedAABB.m_d3dxvMax.x, MovedAABB.m_d3dxvMax.y, MovedAABB.m_d3dxvMax.z);

	printf("UpP1 : (%f, %f, %f) \n", d3dxvUpP1.x, d3dxvUpP1.y, d3dxvUpP1.z);
	printf("UpP2 : (%f, %f, %f) \n", d3dxvUpP2.x, d3dxvUpP2.y, d3dxvUpP2.z);
	printf("UpP3 : (%f, %f, %f) \n", d3dxvUpP3.x, d3dxvUpP3.y, d3dxvUpP3.z);

	printf("DownP1 : (%f, %f, %f) \n", d3dxvDownP1.x, d3dxvDownP1.y, d3dxvDownP1.z);
	printf("DownP2 : (%f, %f, %f) \n", d3dxvDownP2.x, d3dxvDownP2.y, d3dxvDownP2.z);
	printf("DownP3 : (%f, %f, %f) \n", d3dxvDownP3.x, d3dxvDownP3.y, d3dxvDownP3.z);

	printf("v3RaysStart : (%f, %f, %f) \n", v3RaysStart.x, v3RaysStart.y, v3RaysStart.z);
	printf("v3RaysDirect : (%f, %f, %f) \n", v3RaysDirect.x, v3RaysDirect.y, v3RaysDirect.z);
	
	float fU, fV, fDistance;
	D3DXMATRIX d3dxmtxFromPosToIdx;
	D3DXVECTOR4 d3dxvColLayerRow;

	D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &m_d3dxmtxFromIdxToPos);

	if (D3DXIntersectTri(&d3dxvUpP1, &d3dxvUpP2, &d3dxvUpP3, &v3RaysStart, &v3RaysDirect, &fU, &fV, &fDistance))
	{
		D3DXVECTOR3 d3dxvColCoord = d3dxvUpP1 + fU * (d3dxvUpP2 - d3dxvUpP1) + fV * (d3dxvUpP3 - d3dxvUpP1);
		printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
		
		D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvColCoord, &d3dxmtxFromPosToIdx);

		d3dxvColLayerRow.x = roundf(d3dxvColLayerRow.x);
		d3dxvColLayerRow.y = roundf(d3dxvColLayerRow.y+0.5);
		d3dxvColLayerRow.z = roundf(d3dxvColLayerRow.z);

		printf("d3dxvColLayerRow(round) : (%f, %f, %f) \n", d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z);

		m_iBlueprint[(int)d3dxvColLayerRow.y][(int)d3dxvColLayerRow.z][(int)d3dxvColLayerRow.x] = 1;
	}
	else if (D3DXIntersectTri(&d3dxvDownP1, &d3dxvDownP2, &d3dxvDownP3, &v3RaysStart, &v3RaysDirect, &fU, &fV, &fDistance))
	{
		D3DXVECTOR3 d3dxvColCoord = d3dxvDownP1 + fU * (d3dxvDownP2 - d3dxvDownP1) + fV * (d3dxvDownP3 - d3dxvDownP1);
		printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);

		D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvColCoord, &d3dxmtxFromPosToIdx);

		d3dxvColLayerRow.x = roundf(d3dxvColLayerRow.x);
		d3dxvColLayerRow.y = roundf(d3dxvColLayerRow.y + 0.5);
		d3dxvColLayerRow.z = roundf(d3dxvColLayerRow.z);

		printf("d3dxvColLayerRow(round) : (%f, %f, %f) \n", d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z);

		m_iBlueprint[(int)d3dxvColLayerRow.y][(int)d3dxvColLayerRow.z][(int)d3dxvColLayerRow.x] = 1;
	}
}

void CWorkingSpace::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if(m_pLocalCoord) m_pLocalCoord->Render(pd3dDeviceContext);
	if(m_pWorldCoord) m_pWorldCoord->Render(pd3dDeviceContext);
	if(m_pWorkingBoard) m_pWorkingBoard->Render(pd3dDeviceContext);
	//printf("m_pWorkingBoard->GetPosition().y : %f \n", m_pWorkingBoard->GetPosition().y);
	if(m_pWorkingSpace) m_pWorkingSpace->Render(pd3dDeviceContext);
}