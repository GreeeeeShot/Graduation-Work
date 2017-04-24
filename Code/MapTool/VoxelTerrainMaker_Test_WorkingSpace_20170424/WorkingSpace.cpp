#include "stdafx.h"
#include "VoxelTerrain.h"
#include "ShaderResource.h"
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

	m_iLayerStride = 0;
	m_iRowStride = 0;

	m_pLocalCoord = NULL;
	m_pWorldCoord = NULL;
	m_pWorkingSpace = NULL;
	m_pWorkingBoard = NULL;
	m_iWorkingBoardCurLayer = 1;

	m_ppVoxelObjects = NULL;
	m_pShader = CShaderResource::pTexturedLightingShader;
	D3DXMatrixIdentity(&m_d3dxmtxFromIdxToPos);
	D3DXMatrixIdentity(&m_d3dxmtxFromPosToIdx);

	ZeroMemory(m_iBlueprint, sizeof(int) * BLUEPRINT_LAYER * BLUEPRINT_ROW * BLUEPRINT_COL);

	m_iSelectedMtrl = VOXEL_TYPE_GRASS_0;
	m_eViewMode = VIEW_MODE_ALL;
}


CWorkingSpace::~CWorkingSpace()
{
	if(m_pLocalCoord) delete m_pLocalCoord;
	if(m_pWorldCoord) delete m_pWorldCoord;
	if(m_pWorkingSpace) delete m_pWorkingSpace;
	if(m_pWorkingBoard) delete m_pWorkingBoard;

	if (m_ppVoxelObjects)
	{
		for (int i = 0; i < m_nMaxLayer * m_nMaxRow * m_nMaxCol; i++)
		{
			if (m_ppVoxelObjects[i]) delete m_ppVoxelObjects[i];
		}
		delete[]m_ppVoxelObjects;
	}
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

	m_iLayerStride = m_nMaxCol * m_nMaxRow;
	m_iRowStride = m_nMaxCol;

	m_ppVoxelObjects = new CVoxel*[m_nMaxLayer * m_nMaxRow * m_nMaxCol];
	ZeroMemory(m_ppVoxelObjects, sizeof(CVoxel*) * (m_nMaxLayer * m_nMaxRow * m_nMaxCol));
	
	m_pLocalCoord = new CGameObject();
	m_pLocalCoord->SetMesh(CMeshResource::pLocalCoordMesh);
	m_pLocalCoord->SetShader(CShaderResource::pDiffusedShader);

	m_pWorldCoord = new CGameObject();
	m_pWorldCoord->SetMesh(CMeshResource::pWorldCoordMesh);
	m_pWorldCoord->SetShader(CShaderResource::pDiffusedShader);

	m_pWorkingSpace = new CGameObject();
	CMesh *pWorkingSpace = new CDiffusedWorkingSpaceMesh(pd3dDevice, m_nMaxCol * m_fCubeWidth, m_nMaxLayer * m_fCubeHeight, m_nMaxRow * m_fCubeDepth);
	m_pWorkingSpace->SetMesh(pWorkingSpace);
	m_pWorkingSpace->SetShader(CShaderResource::pDiffusedShader);
	m_pWorkingSpace->SetPosition(m_fOffsetXFromLocal * m_fCubeWidth, m_fOffsetYFromLocal * m_fCubeHeight, m_fOffsetZFromLocal * m_fCubeDepth);
	
	m_pWorkingBoard = new CGameObject();
	CMesh *pWorkingBoard = new CDiffusedWorkingBoardMesh(pd3dDevice, m_nMaxCol, m_fCubeWidth, m_nMaxRow, m_fCubeDepth);
	m_pWorkingBoard->SetMesh(pWorkingBoard);
	m_pWorkingBoard->SetShader(CShaderResource::pDiffusedShader);
	m_pWorkingBoard->SetPosition(m_fOffsetXFromLocal * m_fCubeWidth, m_fOffsetYFromLocal * m_fCubeHeight, m_fOffsetZFromLocal * m_fCubeDepth);

	m_d3dxmtxFromIdxToPos._11 = -m_fCubeWidth;
	m_d3dxmtxFromIdxToPos._22 = m_fCubeHeight;
	m_d3dxmtxFromIdxToPos._33 = m_fCubeDepth;

	m_d3dxmtxFromIdxToPos._41 = (m_nMaxCol - 1) * m_fCubeWidth / 2.0f + m_fOffsetXFromLocal;
	// m_d3dxmtxFromIdxToPos._42 = -(m_iMaxLayer - 1) * m_fCubeHeight / 2.0f + m_fOffsetYFromLocal;
	m_d3dxmtxFromIdxToPos._42 = m_fCubeHeight / 2.0f + m_fOffsetYFromLocal;
	m_d3dxmtxFromIdxToPos._43 = -(m_nMaxRow - 1) * m_fCubeDepth / 2.0f + m_fOffsetZFromLocal;

	D3DXMatrixIdentity(&m_d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&m_d3dxmtxFromPosToIdx, NULL, &m_d3dxmtxFromIdxToPos);
	
	m_iSelectedMtrl = VOXEL_TYPE_GRASS_0;
}

void CWorkingSpace::ReviseWorkingSpace(ID3D11Device *pd3dDevice, D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal)
{
	/*D3DXVECTOR3 oldMaxColLayerRow = D3DXVECTOR3(m_nMaxCol, m_nMaxLayer, m_nMaxRow);
	D3DXVECTOR3 oldCubeSize = D3DXVECTOR3(m_fCubeWidth, m_fCubeHeight, m_fCubeDepth);
	D3DXVECTOR3 oldOffsetFromLocal = D3DXVECTOR3(m_fOffsetXFromLocal, m_fOffsetYFromLocal, m_fOffsetZFromLocal);*/

	if (m_nMaxLayer > d3dxvWorkingSpaceSize.y)
	{
		for (int layer = (int)d3dxvWorkingSpaceSize.y - 1; layer < m_nMaxLayer; layer++)
		{
			for (int row = 0; row < m_nMaxRow; row++)
			{
				for (int col = 0; col < m_nMaxCol; col++)
				{
					if (m_ppVoxelObjects[layer * m_iLayerStride + row * m_iRowStride + col])
					{
						printf("더 이상 줄일 수 없습니다. \n");
						return;
					}
				}
			}
		}
	}
	if (m_nMaxCol > d3dxvWorkingSpaceSize.x || m_nMaxRow > d3dxvWorkingSpaceSize.z)
	{
		for (int layer = 0; layer < m_nMaxLayer; layer++)
		{
			for (int i = 0, row = (int)(d3dxvWorkingSpaceSize.z - m_nMaxRow) / 2.0f; i < m_nMaxRow; i++)
			{
				for (int j = 0, col = (int)(d3dxvWorkingSpaceSize.x - m_nMaxCol) / 2.0f; j < m_nMaxCol; j++)
				{
					if ((col + j < 0 || col + j >= (int)(d3dxvWorkingSpaceSize.x)) || (row + i < 0 || row + i >= (int)(d3dxvWorkingSpaceSize.z)) )
					{
						if (m_ppVoxelObjects[layer * m_iLayerStride + i * m_iRowStride + j])
						{
							printf("더 이상 줄일 수 없습니다. \n");
							return;
						}
					}
				}
			}
		}
	}

	m_d3dxmtxFromIdxToPos._11 = -d3dxvCubeSize.x;
	m_d3dxmtxFromIdxToPos._22 = d3dxvCubeSize.y;
	m_d3dxmtxFromIdxToPos._33 = d3dxvCubeSize.z;

	m_d3dxmtxFromIdxToPos._41 = (d3dxvWorkingSpaceSize.x - 1.0f) * d3dxvCubeSize.x / 2.0f + d3dxvOffsetFromLocal.x;
	// m_d3dxmtxFromIdxToPos._42 = -(m_iMaxLayer - 1) * m_fCubeHeight / 2.0f + m_fOffsetYFromLocal;
	m_d3dxmtxFromIdxToPos._42 = d3dxvCubeSize.y / 2.0f + d3dxvOffsetFromLocal.y;
	m_d3dxmtxFromIdxToPos._43 = -(d3dxvWorkingSpaceSize.z - 1.0f) * d3dxvCubeSize.z / 2.0f + d3dxvOffsetFromLocal.z;

	D3DXMatrixIdentity(&m_d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&m_d3dxmtxFromPosToIdx, NULL, &m_d3dxmtxFromIdxToPos);

	CVoxel ** ppTempVoxelObject = new CVoxel*[(int)(d3dxvWorkingSpaceSize.x * d3dxvWorkingSpaceSize.y * d3dxvWorkingSpaceSize.z)];
	ZeroMemory(ppTempVoxelObject, sizeof(CVoxel*) * (int)(d3dxvWorkingSpaceSize.x * d3dxvWorkingSpaceSize.y * d3dxvWorkingSpaceSize.z));
	int iLinearIdx = 0;
	D3DXVECTOR4 d3dxvPosition;
	D3DXVECTOR3 d3dxvIdx;

	for (int layer = 0; layer < m_nMaxLayer; layer++)
	{
		for (int i = 0, row = (d3dxvWorkingSpaceSize.z - m_nMaxRow) / 2.0f; i < m_nMaxRow; i++)
		{
			for (int j = 0, col = (int)(d3dxvWorkingSpaceSize.x - m_nMaxCol) / 2.0f; j < m_nMaxCol; j++)
			{
				if (0 <= col + j && col + j < (int)d3dxvWorkingSpaceSize.x && (0 <= row + i && row + i < (int)d3dxvWorkingSpaceSize.z))
				{
					if (m_ppVoxelObjects[layer * m_iLayerStride + i * m_iRowStride + j])
					{
						ppTempVoxelObject[iLinearIdx = layer * d3dxvWorkingSpaceSize.z * d3dxvWorkingSpaceSize.x + (row + i) * d3dxvWorkingSpaceSize.x + col + j] = m_ppVoxelObjects[layer * m_iLayerStride + i * m_iRowStride + j];
						d3dxvIdx = D3DXVECTOR3(col + j, layer, row + i);
						D3DXVec3Transform(&d3dxvPosition, &d3dxvIdx, &m_d3dxmtxFromIdxToPos);
						ppTempVoxelObject[iLinearIdx]->SetPosition(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);
						printf("Temp : (%d %d %d) : Voxel : (%d %d %d) \n", col + j, layer, row + i, j, layer, i);
					}
				}
			}
		}
	}

	if (m_pLocalCoord) delete m_pLocalCoord;
	if (m_pWorldCoord) delete m_pWorldCoord;
	if (m_pWorkingSpace) delete m_pWorkingSpace;
	if (m_pWorkingBoard) delete m_pWorkingBoard;
	m_iWorkingBoardCurLayer = 1;

	if (m_ppVoxelObjects)
	{
		delete[]m_ppVoxelObjects;
		m_ppVoxelObjects = NULL;
	}
	

	m_nMaxLayer = d3dxvWorkingSpaceSize.y;
	m_nMaxCol = d3dxvWorkingSpaceSize.x;
	m_nMaxRow = d3dxvWorkingSpaceSize.z;
	m_fCubeWidth = d3dxvCubeSize.x;
	m_fCubeHeight = d3dxvCubeSize.y;
	m_fCubeDepth = d3dxvCubeSize.z;
	m_fOffsetXFromLocal = d3dxvOffsetFromLocal.x;
	m_fOffsetYFromLocal = d3dxvOffsetFromLocal.y;
	m_fOffsetZFromLocal = d3dxvOffsetFromLocal.z;

	m_iLayerStride = m_nMaxCol * m_nMaxRow;
	m_iRowStride = m_nMaxCol;

	/*m_ppVoxelObjects = new CVoxel*[m_nMaxLayer * m_nMaxRow * m_nMaxCol];
	ZeroMemory(m_ppVoxelObjects, sizeof(CVoxel*) * (m_nMaxLayer * m_nMaxRow * m_nMaxCol));*/
	m_ppVoxelObjects = ppTempVoxelObject;

	m_pLocalCoord = new CGameObject();
	m_pLocalCoord->SetMesh(CMeshResource::pLocalCoordMesh);
	m_pLocalCoord->SetShader(CShaderResource::pDiffusedShader);

	m_pWorldCoord = new CGameObject();
	m_pWorldCoord->SetMesh(CMeshResource::pWorldCoordMesh);
	m_pWorldCoord->SetShader(CShaderResource::pDiffusedShader);

	m_pWorkingSpace = new CGameObject();
	CMesh *pWorkingSpace = new CDiffusedWorkingSpaceMesh(pd3dDevice, m_nMaxCol * m_fCubeWidth, m_nMaxLayer * m_fCubeHeight, m_nMaxRow * m_fCubeDepth);
	m_pWorkingSpace->SetMesh(pWorkingSpace);
	m_pWorkingSpace->SetShader(CShaderResource::pDiffusedShader);
	m_pWorkingSpace->SetPosition(m_fOffsetXFromLocal * m_fCubeWidth, m_fOffsetYFromLocal * m_fCubeHeight, m_fOffsetZFromLocal * m_fCubeDepth);

	m_pWorkingBoard = new CGameObject();
	CMesh *pWorkingBoard = new CDiffusedWorkingBoardMesh(pd3dDevice, m_nMaxCol, m_fCubeWidth, m_nMaxRow, m_fCubeDepth);
	m_pWorkingBoard->SetMesh(pWorkingBoard);
	m_pWorkingBoard->SetShader(CShaderResource::pDiffusedShader);
	m_pWorkingBoard->SetPosition(m_fOffsetXFromLocal * m_fCubeWidth, m_fOffsetYFromLocal * m_fCubeHeight, m_fOffsetZFromLocal * m_fCubeDepth);

	//m_iSelectedMtrl = VOXEL_TYPE_GRASS_0;
}

void CWorkingSpace::SaveWorkingSpaceToFile(const char* pFileName)
{
	FILE *bluePrintFp;

	bluePrintFp = fopen(pFileName, "w");

	if (bluePrintFp == NULL)
	{
		exit(1);
	}

	fprintf(bluePrintFp, " %d %d %d %f %f %f %f %f %f \n",
		m_nMaxLayer, m_nMaxRow, m_nMaxCol,
		m_fCubeWidth, m_fCubeHeight, m_fCubeDepth,
		m_fOffsetXFromLocal, m_fOffsetYFromLocal, m_fOffsetZFromLocal);
	for (int layer = 0; layer < m_nMaxLayer; layer++)
	{
		for (int row = 0; row < m_nMaxRow; row++)
		{
			for (int col = 0; col < m_nMaxCol; col++)
			{
				fprintf(bluePrintFp, " %d",m_iBlueprint[layer][row][col]);	//재질값을 입력받는다.
				printf("%d", m_iBlueprint[layer][row][col]);										//printf("%d", newbp->bluePrint[layer][breadth][length]);
			}
			fprintf(bluePrintFp, " \n");
		}
		fprintf(bluePrintFp, " \n");
	}

	fclose(bluePrintFp);
}

void CWorkingSpace::ChangeMaterial(int iMtrl)
{
	m_iSelectedMtrl = iMtrl;
}

void CWorkingSpace::ChangeViewMode(VIEW_MODE eViewMode)
{
	m_eViewMode = eViewMode;
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
		m_iWorkingBoardCurLayer++;
		m_pWorkingBoard->m_d3dxmtxWorld._42 = m_pWorkingBoard->GetPosition().y + m_fCubeHeight;
	}
	//printf("m_pWorkingBoard->m_d3dxmtxWorld._42 : %f \n", m_pWorkingBoard->m_d3dxmtxWorld._42);
}

void CWorkingSpace::MoveDownWorkingBoard(void)
{
	if (0.0f >= m_pWorkingBoard->GetPosition().y) return;

	m_iWorkingBoardCurLayer--;
	m_pWorkingBoard->m_d3dxmtxWorld._42 = m_pWorkingBoard->GetPosition().y - m_fCubeHeight;
	//printf("m_pWorkingBoard->m_d3dxmtxWorld._42 : %f \n", m_pWorkingBoard->m_d3dxmtxWorld._42);
}

void CWorkingSpace::MoveUpVoxelTerrain(void)
{
	int iLinearIdx = 0;
	D3DXVECTOR3 d3dxvPos;

	for (int layer = m_nMaxLayer - 1; layer >= 0; layer--)
	{
		for (int row = 0; row < m_nMaxRow; row++)
		{
			for (int col = 0; col < m_nMaxCol; col++)
			{
				if (layer == m_nMaxLayer - 1 && m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + col]) return;

				if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + col])
				{
					d3dxvPos = m_ppVoxelObjects[iLinearIdx]->GetPosition();
					m_ppVoxelObjects[iLinearIdx]->SetPosition(d3dxvPos.x, d3dxvPos.y + m_fCubeHeight, d3dxvPos.z);
					m_ppVoxelObjects[(layer + 1) * m_iLayerStride + row * m_iRowStride + col] = m_ppVoxelObjects[iLinearIdx];
					m_ppVoxelObjects[iLinearIdx] = NULL;
				}
			}
		}
	}
}
void CWorkingSpace::MoveDownVoxelTerrain(void)
{
	int iLinearIdx = 0;
	D3DXVECTOR3 d3dxvPos;

	for (int layer = 0; layer < m_nMaxLayer; layer++)
	{
		for (int row = 0; row < m_nMaxRow; row++)
		{
			for (int col = 0; col < m_nMaxCol; col++)
			{
				if (layer == 0 && m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + col]) return;

				if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + col])
				{
					d3dxvPos = m_ppVoxelObjects[iLinearIdx]->GetPosition();
					m_ppVoxelObjects[iLinearIdx]->SetPosition(d3dxvPos.x, d3dxvPos.y - m_fCubeHeight, d3dxvPos.z);
					m_ppVoxelObjects[(layer - 1) * m_iLayerStride + row * m_iRowStride + col] = m_ppVoxelObjects[iLinearIdx];
					m_ppVoxelObjects[iLinearIdx] = NULL;
				}
			}
		}
	}
}
void CWorkingSpace::MoveForwardVoxelTerrain(void)
{
	int iLinearIdx = 0;
	D3DXVECTOR3 d3dxvPos;

	for (int layer = 0; layer < m_nMaxLayer; layer++)
	{
		for (int col = 0; col < m_nMaxCol; col++)
		{
			if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + (m_nMaxRow - 1) * m_iRowStride + col]) return;
		}
	}

	for (int row = m_nMaxRow - 2; row >= 0; row--)
	{
		for (int layer = 0; layer < m_nMaxLayer; layer++)
		{
			for (int col = 0; col < m_nMaxCol; col++)
			{
				if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + col])
				{
					d3dxvPos = m_ppVoxelObjects[iLinearIdx]->GetPosition();
					m_ppVoxelObjects[iLinearIdx]->SetPosition(d3dxvPos.x, d3dxvPos.y, d3dxvPos.z + m_fCubeDepth);
					m_ppVoxelObjects[layer * m_iLayerStride + (row + 1) * m_iRowStride + col] = m_ppVoxelObjects[iLinearIdx];
					m_ppVoxelObjects[iLinearIdx] = NULL;
				}
			}
		}
	}
}
void CWorkingSpace::MoveRearVoxelTerrain(void)
{
	int iLinearIdx = 0;
	D3DXVECTOR3 d3dxvPos;

	for (int layer = 0; layer < m_nMaxLayer; layer++)
	{
		for (int col = 0; col < m_nMaxCol; col++)
		{
			if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + 0 * m_iRowStride + col]) return;
		}
	}

	for (int row = 1; row < m_nMaxRow; row++)
	{
		for (int layer = 0; layer < m_nMaxLayer; layer++)
		{
			for (int col = 0; col < m_nMaxCol; col++)
			{
				if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + col])
				{
					d3dxvPos = m_ppVoxelObjects[iLinearIdx]->GetPosition();
					m_ppVoxelObjects[iLinearIdx]->SetPosition(d3dxvPos.x, d3dxvPos.y, d3dxvPos.z - m_fCubeDepth);
					m_ppVoxelObjects[layer * m_iLayerStride + (row - 1) * m_iRowStride + col] = m_ppVoxelObjects[iLinearIdx];
					m_ppVoxelObjects[iLinearIdx] = NULL;
				}
			}
		}
	}
}
void CWorkingSpace::MoveRightVoxelTerrain(void)
{
	int iLinearIdx = 0;
	D3DXVECTOR3 d3dxvPos;

	for (int layer = 0; layer < m_nMaxLayer; layer++)
	{
		for (int row = 0; row < m_nMaxRow; row++)
		{
			if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + 0]) return;
		}
	}

	for (int col = 1; col < m_nMaxCol; col++)
	{
		for (int layer = 0; layer < m_nMaxLayer; layer++)
		{
			for (int row = 0; row < m_nMaxRow; row++)
			{
				if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + col])
				{
					d3dxvPos = m_ppVoxelObjects[iLinearIdx]->GetPosition();
					m_ppVoxelObjects[iLinearIdx]->SetPosition(d3dxvPos.x + m_fCubeWidth, d3dxvPos.y, d3dxvPos.z);
					m_ppVoxelObjects[layer * m_iLayerStride + row * m_iRowStride + (col - 1)] = m_ppVoxelObjects[iLinearIdx];
					m_ppVoxelObjects[iLinearIdx] = NULL;
				}
			}
		}
	}
}
void CWorkingSpace::MoveLeftVoxelTerrain(void)
{
	int iLinearIdx = 0;
	D3DXVECTOR3 d3dxvPos;

	for (int layer = 0; layer < m_nMaxLayer; layer++)
	{
		for (int row = 0; row < m_nMaxRow; row++)
		{
			if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + (m_nMaxCol - 1)]) return;
		}
	}

	for (int col = m_nMaxCol - 2; col >= 0; col--)
	{
		for (int layer = 0; layer < m_nMaxLayer; layer++)
		{
			for (int row = 0; row < m_nMaxRow; row++)
			{
				if (m_ppVoxelObjects[iLinearIdx = layer * m_iLayerStride + row * m_iRowStride + col])
				{
					d3dxvPos = m_ppVoxelObjects[iLinearIdx]->GetPosition();
					m_ppVoxelObjects[iLinearIdx]->SetPosition(d3dxvPos.x - m_fCubeWidth, d3dxvPos.y, d3dxvPos.z);
					m_ppVoxelObjects[layer * m_iLayerStride + row * m_iRowStride + (col + 1)] = m_ppVoxelObjects[iLinearIdx];
					m_ppVoxelObjects[iLinearIdx] = NULL;
				}
			}
		}
	}
}

void CWorkingSpace::CreateRayVector(CCamera *pCamera, int iMouseX, int iMouseY, D3DXVECTOR3 *pRayPos, D3DXVECTOR3 *pRayDir)
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

	*pRayPos = v3RaysStart;
	*pRayDir = v3RaysDirect;
}

void CWorkingSpace::CreateVoxelObject(ID3D11Device *pd3dDevice, CCamera *pCamera, int iMouseX, int iMouseY)
{
	D3DXVECTOR3 v3RaysStart;
	D3DXVECTOR3 v3RaysDirect;
	
	CreateRayVector(pCamera, iMouseX, iMouseY, &v3RaysStart, &v3RaysDirect);

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
	D3DXVECTOR3 d3dxvColCoord;

	D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &m_d3dxmtxFromIdxToPos);

	if (D3DXIntersectTri(&d3dxvUpP1, &d3dxvUpP2, &d3dxvUpP3, &v3RaysStart, &v3RaysDirect, &fU, &fV, &fDistance))
	{
		d3dxvColCoord = d3dxvUpP1 + fU * (d3dxvUpP2 - d3dxvUpP1) + fV * (d3dxvUpP3 - d3dxvUpP1);
		printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else if (D3DXIntersectTri(&d3dxvDownP1, &d3dxvDownP2, &d3dxvDownP3, &v3RaysStart, &v3RaysDirect, &fU, &fV, &fDistance))
	{
		d3dxvColCoord = d3dxvDownP1 + fU * (d3dxvDownP2 - d3dxvDownP1) + fV * (d3dxvDownP3 - d3dxvDownP1);
		printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else return;

	D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvColCoord, &d3dxmtxFromPosToIdx);

	d3dxvColLayerRow.x = roundf(d3dxvColLayerRow.x);
	d3dxvColLayerRow.y = roundf(d3dxvColLayerRow.y + m_fCubeHeight / 2.0f);
	d3dxvColLayerRow.z = roundf(d3dxvColLayerRow.z);

	printf("d3dxvColLayerRow(round) : (%f, %f, %f) \n", d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z);

	m_iBlueprint[(int)d3dxvColLayerRow.y][(int)d3dxvColLayerRow.z][(int)d3dxvColLayerRow.x] = m_iSelectedMtrl;

	int iLinearIdx = (int)d3dxvColLayerRow.y * m_iLayerStride + (int)d3dxvColLayerRow.z * m_iRowStride + (int)d3dxvColLayerRow.x;
	if (m_ppVoxelObjects[iLinearIdx]) delete m_ppVoxelObjects[iLinearIdx];

	m_ppVoxelObjects[iLinearIdx] = GetVoxelForType((VOXEL_TYPE)m_iSelectedMtrl);
	m_ppVoxelObjects[iLinearIdx]->SetShader(CShaderResource::pTexturedLightingShader);
	m_ppVoxelObjects[iLinearIdx]->SetMaterial(CMaterialResource::pStandardMaterial);
	/*m_ppVoxelObjects[iLinearIdx]->SetMesh(CMeshResource::ppRegisteredVoxelMesh[m_iSelectedMtrl]);
	m_ppVoxelObjects[iLinearIdx]->SetTexture(CTextureResource::ppRegisteredTexture[m_iSelectedMtrl])*/;
	D3DXVECTOR4 d3dxvPosition;
	D3DXVec3Transform(&d3dxvPosition, &D3DXVECTOR3(d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z), &m_d3dxmtxFromIdxToPos);
	m_ppVoxelObjects[iLinearIdx]->SetPosition(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);
	printf("d3dxvPosition : (%f, %f, %f) \n", d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);
}

void CWorkingSpace::DestroyVoxelObject(ID3D11Device *pd3dDevice, CCamera *pCamera, int iMouseX, int iMouseY)
{
	D3DXVECTOR3 v3RaysStart;
	D3DXVECTOR3 v3RaysDirect;

	CreateRayVector(pCamera, iMouseX, iMouseY, &v3RaysStart, &v3RaysDirect);

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
	D3DXVECTOR3 d3dxvColCoord;

	D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &m_d3dxmtxFromIdxToPos);

	if (D3DXIntersectTri(&d3dxvUpP1, &d3dxvUpP2, &d3dxvUpP3, &v3RaysStart, &v3RaysDirect, &fU, &fV, &fDistance))
	{
		d3dxvColCoord = d3dxvUpP1 + fU * (d3dxvUpP2 - d3dxvUpP1) + fV * (d3dxvUpP3 - d3dxvUpP1);
		printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else if (D3DXIntersectTri(&d3dxvDownP1, &d3dxvDownP2, &d3dxvDownP3, &v3RaysStart, &v3RaysDirect, &fU, &fV, &fDistance))
	{
		d3dxvColCoord = d3dxvDownP1 + fU * (d3dxvDownP2 - d3dxvDownP1) + fV * (d3dxvDownP3 - d3dxvDownP1);
		printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else return;

	D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvColCoord, &d3dxmtxFromPosToIdx);

	d3dxvColLayerRow.x = roundf(d3dxvColLayerRow.x);
	d3dxvColLayerRow.y = roundf(d3dxvColLayerRow.y + m_fCubeHeight / 2.0f);
	d3dxvColLayerRow.z = roundf(d3dxvColLayerRow.z);

	printf("d3dxvColLayerRow(round) : (%f, %f, %f) \n", d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z);

	m_iBlueprint[(int)d3dxvColLayerRow.y][(int)d3dxvColLayerRow.z][(int)d3dxvColLayerRow.x] = 0;

	int iLinearIdx = (int)d3dxvColLayerRow.y * m_iLayerStride + (int)d3dxvColLayerRow.z * m_iRowStride + (int)d3dxvColLayerRow.x;
	if (m_ppVoxelObjects[iLinearIdx])
	{
		delete m_ppVoxelObjects[iLinearIdx];
		m_ppVoxelObjects[iLinearIdx] = NULL;
	}
}

void CWorkingSpace::CreateVoxelObjectByPaint(ID3D11Device *pd3dDevice, CCamera *pCamera, int iFixedX, int iFixedY, int iMouseX, int iMouseY)
{
	D3DXVECTOR3 v3FixedRaysStart;
	D3DXVECTOR3 v3FixedRayDirect;

	D3DXVECTOR3 v3DynamicRaysStart;
	D3DXVECTOR3 v3DynamicRayDirect;

	CreateRayVector(pCamera, iFixedX, iFixedY, &v3FixedRaysStart, &v3FixedRayDirect);
	CreateRayVector(pCamera, iMouseX, iMouseY, &v3DynamicRaysStart, &v3DynamicRayDirect);

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

	float fU, fV, fDistance;
	D3DXMATRIX d3dxmtxFromPosToIdx;
	D3DXVECTOR4 d3dxvFixedColLayerRow;
	D3DXVECTOR3 d3dxvFixedColCoord;
	D3DXVECTOR4 d3dxvDynamicColLayerRow;
	D3DXVECTOR3 d3dxvDynamicColCoord;

	D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &m_d3dxmtxFromIdxToPos);

	if (D3DXIntersectTri(&d3dxvUpP1, &d3dxvUpP2, &d3dxvUpP3, &v3FixedRaysStart, &v3FixedRayDirect, &fU, &fV, &fDistance))
	{
		d3dxvFixedColCoord = d3dxvUpP1 + fU * (d3dxvUpP2 - d3dxvUpP1) + fV * (d3dxvUpP3 - d3dxvUpP1);
		//printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else if (D3DXIntersectTri(&d3dxvDownP1, &d3dxvDownP2, &d3dxvDownP3, &v3FixedRaysStart, &v3FixedRayDirect, &fU, &fV, &fDistance))
	{
		d3dxvFixedColCoord = d3dxvDownP1 + fU * (d3dxvDownP2 - d3dxvDownP1) + fV * (d3dxvDownP3 - d3dxvDownP1);
		//printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else return;

	D3DXVec3Transform(&d3dxvFixedColLayerRow, &d3dxvFixedColCoord, &d3dxmtxFromPosToIdx);

	d3dxvFixedColLayerRow.x = roundf(d3dxvFixedColLayerRow.x);
	d3dxvFixedColLayerRow.y = roundf(d3dxvFixedColLayerRow.y + m_fCubeHeight / 2.0f);
	d3dxvFixedColLayerRow.z = roundf(d3dxvFixedColLayerRow.z);

	if (D3DXIntersectTri(&d3dxvUpP1, &d3dxvUpP2, &d3dxvUpP3, &v3DynamicRaysStart, &v3DynamicRayDirect, &fU, &fV, &fDistance))
	{
		d3dxvDynamicColCoord = d3dxvUpP1 + fU * (d3dxvUpP2 - d3dxvUpP1) + fV * (d3dxvUpP3 - d3dxvUpP1);
		//printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else if (D3DXIntersectTri(&d3dxvDownP1, &d3dxvDownP2, &d3dxvDownP3, &v3DynamicRaysStart, &v3DynamicRayDirect, &fU, &fV, &fDistance))
	{
		d3dxvDynamicColCoord = d3dxvDownP1 + fU * (d3dxvDownP2 - d3dxvDownP1) + fV * (d3dxvDownP3 - d3dxvDownP1);
		//printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else return;

	D3DXVec3Transform(&d3dxvDynamicColLayerRow, &d3dxvDynamicColCoord, &d3dxmtxFromPosToIdx);

	d3dxvDynamicColLayerRow.x = roundf(d3dxvDynamicColLayerRow.x);
	d3dxvDynamicColLayerRow.y = roundf(d3dxvDynamicColLayerRow.y + m_fCubeHeight / 2.0f);
	d3dxvDynamicColLayerRow.z = roundf(d3dxvDynamicColLayerRow.z);

	for (int row = d3dxvFixedColLayerRow.z; 
		d3dxvDynamicColLayerRow.z - d3dxvFixedColLayerRow.z >= 0? (row <= d3dxvDynamicColLayerRow.z) : (row >= d3dxvDynamicColLayerRow.z);
		d3dxvDynamicColLayerRow.z - d3dxvFixedColLayerRow.z >= 0? row++ : row--)
	{
		for (int col = d3dxvFixedColLayerRow.x;
			d3dxvDynamicColLayerRow.x - d3dxvFixedColLayerRow.x >= 0 ? (col <= d3dxvDynamicColLayerRow.x) : (col >= d3dxvDynamicColLayerRow.x);
			d3dxvDynamicColLayerRow.x - d3dxvFixedColLayerRow.x >= 0 ? col++ : col--)
		{
			m_iBlueprint[(int)d3dxvDynamicColLayerRow.y][row][col] = m_iSelectedMtrl;

			int iLinearIdx = (int)d3dxvDynamicColLayerRow.y * m_iLayerStride + (int)row * m_iRowStride + (int)col;
			if (m_ppVoxelObjects[iLinearIdx]) delete m_ppVoxelObjects[iLinearIdx];

			m_ppVoxelObjects[iLinearIdx] = GetVoxelForType((VOXEL_TYPE) m_iSelectedMtrl);
			m_ppVoxelObjects[iLinearIdx]->SetShader(CShaderResource::pTexturedLightingShader);
			m_ppVoxelObjects[iLinearIdx]->SetMaterial(CMaterialResource::pStandardMaterial);
			/*m_ppVoxelObjects[iLinearIdx]->SetMesh(CMeshResource::ppRegisteredVoxelMesh[m_iSelectedMtrl]);
			m_ppVoxelObjects[iLinearIdx]->SetTexture(CTextureResource::ppRegisteredTexture[m_iSelectedMtrl]);*/
			D3DXVECTOR4 d3dxvPosition;
			D3DXVec3Transform(&d3dxvPosition, &D3DXVECTOR3(col, d3dxvDynamicColLayerRow.y, row), &m_d3dxmtxFromIdxToPos);
			m_ppVoxelObjects[iLinearIdx]->SetPosition(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);
			printf("d3dxvPosition : (%f, %f, %f) \n", d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);
		}
	}
}

void CWorkingSpace::DestroyVoxelObjectByPaint(ID3D11Device *pd3dDevice, CCamera *pCamera, int iFixedX, int iFixedY, int iMouseX, int iMouseY)
{
	D3DXVECTOR3 v3FixedRaysStart;
	D3DXVECTOR3 v3FixedRayDirect;

	D3DXVECTOR3 v3DynamicRaysStart;
	D3DXVECTOR3 v3DynamicRayDirect;

	CreateRayVector(pCamera, iFixedX, iFixedY, &v3FixedRaysStart, &v3FixedRayDirect);
	CreateRayVector(pCamera, iMouseX, iMouseY, &v3DynamicRaysStart, &v3DynamicRayDirect);

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

	float fU, fV, fDistance;
	D3DXMATRIX d3dxmtxFromPosToIdx;
	D3DXVECTOR4 d3dxvFixedColLayerRow;
	D3DXVECTOR3 d3dxvFixedColCoord;
	D3DXVECTOR4 d3dxvDynamicColLayerRow;
	D3DXVECTOR3 d3dxvDynamicColCoord;

	D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &m_d3dxmtxFromIdxToPos);

	if (D3DXIntersectTri(&d3dxvUpP1, &d3dxvUpP2, &d3dxvUpP3, &v3FixedRaysStart, &v3FixedRayDirect, &fU, &fV, &fDistance))
	{
		d3dxvFixedColCoord = d3dxvUpP1 + fU * (d3dxvUpP2 - d3dxvUpP1) + fV * (d3dxvUpP3 - d3dxvUpP1);
		//printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else if (D3DXIntersectTri(&d3dxvDownP1, &d3dxvDownP2, &d3dxvDownP3, &v3FixedRaysStart, &v3FixedRayDirect, &fU, &fV, &fDistance))
	{
		d3dxvFixedColCoord = d3dxvDownP1 + fU * (d3dxvDownP2 - d3dxvDownP1) + fV * (d3dxvDownP3 - d3dxvDownP1);
		//printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else return;

	D3DXVec3Transform(&d3dxvFixedColLayerRow, &d3dxvFixedColCoord, &d3dxmtxFromPosToIdx);

	d3dxvFixedColLayerRow.x = roundf(d3dxvFixedColLayerRow.x);
	d3dxvFixedColLayerRow.y = roundf(d3dxvFixedColLayerRow.y + m_fCubeHeight / 2.0f);
	d3dxvFixedColLayerRow.z = roundf(d3dxvFixedColLayerRow.z);

	if (D3DXIntersectTri(&d3dxvUpP1, &d3dxvUpP2, &d3dxvUpP3, &v3DynamicRaysStart, &v3DynamicRayDirect, &fU, &fV, &fDistance))
	{
		d3dxvDynamicColCoord = d3dxvUpP1 + fU * (d3dxvUpP2 - d3dxvUpP1) + fV * (d3dxvUpP3 - d3dxvUpP1);
		//printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else if (D3DXIntersectTri(&d3dxvDownP1, &d3dxvDownP2, &d3dxvDownP3, &v3DynamicRaysStart, &v3DynamicRayDirect, &fU, &fV, &fDistance))
	{
		d3dxvDynamicColCoord = d3dxvDownP1 + fU * (d3dxvDownP2 - d3dxvDownP1) + fV * (d3dxvDownP3 - d3dxvDownP1);
		//printf("d3dxvColCoord : (%f, %f, %f) \n", d3dxvColCoord.x, d3dxvColCoord.y, d3dxvColCoord.z);
	}
	else return;

	D3DXVec3Transform(&d3dxvDynamicColLayerRow, &d3dxvDynamicColCoord, &d3dxmtxFromPosToIdx);

	d3dxvDynamicColLayerRow.x = roundf(d3dxvDynamicColLayerRow.x);
	d3dxvDynamicColLayerRow.y = roundf(d3dxvDynamicColLayerRow.y + m_fCubeHeight / 2.0f);
	d3dxvDynamicColLayerRow.z = roundf(d3dxvDynamicColLayerRow.z);

	for (int row = d3dxvFixedColLayerRow.z;
		d3dxvDynamicColLayerRow.z - d3dxvFixedColLayerRow.z >= 0 ? (row <= d3dxvDynamicColLayerRow.z) : (row >= d3dxvDynamicColLayerRow.z);
		d3dxvDynamicColLayerRow.z - d3dxvFixedColLayerRow.z >= 0 ? row++ : row--)
	{
		for (int col = d3dxvFixedColLayerRow.x;
			d3dxvDynamicColLayerRow.x - d3dxvFixedColLayerRow.x >= 0 ? (col <= d3dxvDynamicColLayerRow.x) : (col >= d3dxvDynamicColLayerRow.x);
			d3dxvDynamicColLayerRow.x - d3dxvFixedColLayerRow.x >= 0 ? col++ : col--)
		{
			m_iBlueprint[(int)d3dxvDynamicColLayerRow.y][row][col] = 0;

			int iLinearIdx = (int)d3dxvDynamicColLayerRow.y * m_iLayerStride + (int)row * m_iRowStride + (int)col;
			if (m_ppVoxelObjects[iLinearIdx]) delete m_ppVoxelObjects[iLinearIdx];
			m_ppVoxelObjects[iLinearIdx] = NULL;
		}
	}
}

CVoxel* CWorkingSpace::GetVoxelForType(VOXEL_TYPE eVoxelType)
{
	switch (eVoxelType)
	{
	case VOXEL_TYPE_GRASS_0:
		return new CVoxelGrass0();
		break;
	case VOXEL_TYPE_WOOD_0:
		return new CVoxelWood0();
		break;
	case VOXEL_TYPE_UPHILL_0:
		return new CVoxelUphill0();
		break;
	case VOXEL_TYPE_UPHILL_1:
		return new CVoxelUphill1();
		break;
	case VOXEL_TYPE_UPHILL_2:
		return new CVoxelUphill2();
		break;
	case VOXEL_TYPE_UPHILL_3:
		return new CVoxelUphill3();
		break;
	case VOXEL_TYPE_BUSH_0:
		return new CVoxelBush0();
		break;
	case VOXEL_TYPE_SWAMP_0:
		return new CVoxelSwamp0();
		break;
	case VOXEL_TYPE_JUMP_0:
		return new CVoxelJump0();
		break;
	case VOXEL_TYPE_SNOW_0:
		return new CVoxelSnow0();
		break;
	case VOXEL_TYPE_EMPTY_SNOW_0:
		return new CVoxelEmptySnow0();
		break;
	case VOXEL_TYPE_ICE_0:
		return new CVoxelIce0();
		break;
	}
}

void CWorkingSpace::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if(m_pLocalCoord) m_pLocalCoord->Render(pd3dDeviceContext);
	if(m_pWorldCoord) m_pWorldCoord->Render(pd3dDeviceContext);
	if(m_pWorkingBoard) m_pWorkingBoard->Render(pd3dDeviceContext);
	if(m_pWorkingSpace) m_pWorkingSpace->Render(pd3dDeviceContext);

	int LinearIdx = 0;

	if (m_ppVoxelObjects)
	{
		switch (m_eViewMode)
		{
		case VIEW_MODE_ALL:
			for (int i = 0; i < m_nMaxLayer * m_nMaxRow * m_nMaxCol; i++)
			{
				if (m_ppVoxelObjects[i]) m_ppVoxelObjects[i]->Render(pd3dDeviceContext);
			}
			break;
		case VIEW_MODE_ACCUMULATE:
			for (int layer = 0; layer < m_iWorkingBoardCurLayer; layer++)
			{
				for (int row = 0; row < m_nMaxRow; row++)
				{
					for (int col = 0; col < m_nMaxCol; col++)
					{
						if (m_ppVoxelObjects[LinearIdx = layer * m_iLayerStride + row * m_iRowStride + col]) m_ppVoxelObjects[LinearIdx]->Render(pd3dDeviceContext);
					}
				}
			}
			break;
		case VIEW_MODE_RELEVANT:
			for (int row = 0; row < m_nMaxRow; row++)
			{
				for (int col = 0; col < m_nMaxCol; col++)
				{
					if (m_ppVoxelObjects[LinearIdx = (m_iWorkingBoardCurLayer - 1) * m_iLayerStride + row * m_iRowStride + col]) m_ppVoxelObjects[LinearIdx]->Render(pd3dDeviceContext);
				}
			}
			break;
		}
		
	}
	
}