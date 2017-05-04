#include "stdafx.h"
#include "TextureResource.h"
#include "Voxel.h"
#include "MeshResource.h"
#include "ShaderResource.h"
#include "VoxelTerrain.h"

//float g_fFriction[16] = { DAMPING_GRASS_0, DAMPING_WOOD_0, DAMPING_UPHILL, DAMPING_UPHILL, DAMPING_UPHILL, DAMPING_UPHILL, DAMPING_NULL };
//float g_fReflection[16] = { REFLECTION_GRASS_0, REFLECTION_WOOD_0 , REFLECTION_NULL, REFLECTION_NULL, REFLECTION_NULL, REFLECTION_NULL, REFLECTION_NULL};
//float g_fSideReflection[16] = {};

CBlueprint::CBlueprint()
{
	m_iMapTotalNum = 0;
	m_iBlueprint[BLUEPRINT_LAYER][BLUEPRINT_ROW][BLUEPRINT_COL];
	m_nVoxelObjectsNum = 0;
	m_nMaxLayer = 0;
	m_nMaxCol = 0;
	m_nMaxRow = 0;
	m_fCubeWidth = 0.0f;
	m_fCubeHeight = 0.0f;
	m_fCubeDepth = 0.0f;
	m_fOffsetXFromLocal = 0.0f;
	m_fOffsetYFromLocal = 0.0f;
	m_fOffsetZFromLocal = 0.0f;
}

CBlueprint::~CBlueprint()
{

}

CBlueprint*  CBlueprint::LoadBlueprint(const char *strBlueprintFileName)
{
	FILE* bluePrintFp;

	 fopen_s(&bluePrintFp, strBlueprintFileName, "r");

	if (bluePrintFp == NULL)
	{
		exit(1);
	}

	fscanf_s(bluePrintFp, "%d", &m_iMapTotalNum);
	
	for (int i = 0; i < m_iMapTotalNum; i++)
	{
		fscanf_s(bluePrintFp, " %d %d %d %f %f %f %f %f %f",
			&m_nMaxLayer, &m_nMaxRow, &m_nMaxCol,
			&m_fCubeWidth, &m_fCubeHeight, &m_fCubeDepth,
			&m_fOffsetXFromLocal, &m_fOffsetYFromLocal, &m_fOffsetZFromLocal);//,
																			  //&m_fAxisX, &m_fAxisY, &m_fAxisZ);

		for (int layer = 0; layer < m_nMaxLayer; layer++)
		{
			for (int row = 0; row < m_nMaxRow; row++)
			{
				for (int col = 0; col < m_nMaxCol; col++)
				{
					fscanf_s(bluePrintFp, " %d", &(m_iBlueprint[layer][row][col]));	//텍스쳐 값을 입력받는다.
					if (m_iBlueprint[layer][row][col]) m_nVoxelObjectsNum++;
				}
			}
		}
	}

	fclose(bluePrintFp);

	return this;
}

CVoxelTerrain::CVoxelTerrain()
{
	m_nVoxelObjectsNum = 0;
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	D3DXMatrixIdentity(&m_d3dxmtxFromIdxToPos);
	D3DXMatrixIdentity(&m_d3dxmtxFromPosToIdx);
	m_pShader = NULL;

	m_ppVoxelObjectsForCollision = NULL;
	//m_ppVoxelObjectsForRendering = NULL;
}


CVoxelTerrain::~CVoxelTerrain()
{
	/*if (m_ppVoxelObjectsForRendering)
	{
		for (int i = 0; i < m_nVoxelObjectsNum; i++)
		{
			delete m_ppVoxelObjectsForRendering[i];
		}
		delete []m_ppVoxelObjectsForRendering;
	}*/

	

	for (int i = 0; i < VOXEL_TYPE_NUM; i++)
	{
		std::vector<CVoxel*>::iterator iter = m_vecVoxelObjectsForRendering[i].begin();
		std::vector<CVoxel*>::iterator endIer = m_vecVoxelObjectsForRendering[i].end();
		for (; iter != m_vecVoxelObjectsForRendering[i].end(); ++iter)
		{
			delete (*iter);
		}
		m_vecVoxelObjectsForRendering[i].clear();
	}

	if (m_ppVoxelObjectsForCollision) delete[]m_ppVoxelObjectsForCollision;
}

CVoxel* CVoxelTerrain::GetVoxelForType(VOXEL_TYPE eVoxelType)
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

void CVoxelTerrain::CreateVoxelTerrain(CBlueprint* pBlueprint)
{
	if (!pBlueprint->m_nVoxelObjectsNum) exit(-1);

	m_iMaxLayer = pBlueprint->m_nMaxLayer;
	m_iMaxRow = pBlueprint->m_nMaxRow;
	m_iMaxCol = pBlueprint->m_nMaxCol;

	m_iLayerStride = m_iMaxRow * m_iMaxCol;
	m_iRowStride = m_iMaxCol;

	m_ppVoxelObjectsForCollision = new CVoxel*[m_iMaxLayer * m_iMaxRow * m_iMaxCol];
	//m_ppVoxelObjectsForRendering = new CVoxel*[pBlueprint->m_nVoxelObjectsNum];
	ZeroMemory(m_ppVoxelObjectsForCollision, sizeof(CVoxel*) * (m_iMaxLayer * m_iMaxRow * m_iMaxCol));
	//ZeroMemory(m_ppVoxelObjectsForRendering, sizeof(CVoxel*) * pBlueprint->m_nVoxelObjectsNum);

	m_fOffsetXFromLocal = pBlueprint->m_fOffsetXFromLocal;
	m_fOffsetYFromLocal = pBlueprint->m_fOffsetYFromLocal;
	m_fOffsetZFromLocal = pBlueprint->m_fOffsetZFromLocal;

	m_fCubeHeight = pBlueprint->m_fCubeHeight;
	m_fCubeWidth = pBlueprint->m_fCubeWidth;
	m_fCubeDepth = pBlueprint->m_fCubeDepth;

	m_d3dxmtxFromIdxToPos._11 = -m_fCubeWidth;
	m_d3dxmtxFromIdxToPos._22 = m_fCubeHeight;
	m_d3dxmtxFromIdxToPos._33 = m_fCubeDepth;

	m_d3dxmtxFromIdxToPos._41 = (m_iMaxCol - 1) * m_fCubeWidth / 2.0f + m_fOffsetXFromLocal;
	m_d3dxmtxFromIdxToPos._42 = m_fCubeHeight / 2.0f + m_fOffsetYFromLocal;
	m_d3dxmtxFromIdxToPos._43 = -(m_iMaxRow - 1) * m_fCubeDepth / 2.0f + m_fOffsetZFromLocal;

	D3DXMatrixIdentity(&m_d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&m_d3dxmtxFromPosToIdx, NULL, &m_d3dxmtxFromIdxToPos);

	VOXEL_TYPE iSelectedType;
	D3DXVECTOR4 d3dxvPosition;
	CVoxel* pSelectedVoxel;

	m_pShader = CShaderResource::pTexturedLightingShader;

	for (int layer = 0; layer < pBlueprint->m_nMaxLayer; layer++)
	{
		for (int row = 0; row < pBlueprint->m_nMaxRow ; row++)
		{
			for (int col = 0; col < pBlueprint->m_nMaxCol ; col++)
			{
				if (iSelectedType = (VOXEL_TYPE)pBlueprint->m_iBlueprint[layer][row][col])
				{
					pSelectedVoxel = GetVoxelForType(iSelectedType);
					pSelectedVoxel->SetMaterial(CMaterialResource::pStandardMaterial);
					D3DXVec3Transform(&d3dxvPosition, &D3DXVECTOR3(col, layer, row), &m_d3dxmtxFromIdxToPos);
					pSelectedVoxel->SetPosition(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);

					if (pSelectedVoxel->m_bIsActive)
					{
						m_ppVoxelObjectsForCollision[layer * m_iLayerStride + row * m_iRowStride + col]
							= pSelectedVoxel;
					}
					m_vecVoxelObjectsForRendering[iSelectedType].push_back(pSelectedVoxel);
					m_nVoxelObjectsNum++;
				}
			}
		}
	}

}

void CVoxelTerrain::SetPosition(float x, float y, float z)
{
	m_d3dxmtxWorld._41 = x;
	m_d3dxmtxWorld._42 = y;
	m_d3dxmtxWorld._43 = z;
}

void CVoxelTerrain::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	m_d3dxmtxWorld._41 = d3dxvPosition.x;
	m_d3dxmtxWorld._42 = d3dxvPosition.y;
	m_d3dxmtxWorld._43 = d3dxvPosition.z;
}

D3DXVECTOR3 CVoxelTerrain::GetPosition()
{
	return(D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43));
}

bool CVoxelTerrain::IsInnerVoxel(CVoxel* pVoxel, CCamera* pCamera)
{
	static D3DXVECTOR3 d3dxvSearchDir[6] =
	{
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		D3DXVECTOR3(0.0f, -1.0f,0.0f)
	};

	int iSurfaceCount = 0;
	D3DXVECTOR4 d3dxvColLayerRow;
	D3DXVECTOR3 d3dxvFromVoxelToCamera;
	float fDot = 0.0f;
	float fSearchRange = 0.0f;
	
	D3DXVec3Transform(&d3dxvColLayerRow, &pVoxel->GetPosition(), &m_d3dxmtxFromPosToIdx);
	D3DXVec3Normalize(&d3dxvFromVoxelToCamera, &(pCamera->GetPosition() - pVoxel->GetPosition()));

	// 복셀의의 위치를 인덱스화 한다.
	d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
	d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
	d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);
	//printf("d3dxvColLayerRow : (%f, %f, %f) \n", d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z);
	D3DXVECTOR3 d3dxvInspectionIdx;			// 콜리젼 박스에 설치될 인덱스 좌표
	int iLinearIdx;

	for (int i = 0; i < 6; i++)
	{
		
		fDot = D3DXVec3Dot(&d3dxvFromVoxelToCamera, &D3DXVECTOR3(-d3dxvSearchDir[i].x, d3dxvSearchDir[i].y, d3dxvSearchDir[i].z));
		if (fDot >= 1.0f) fDot = 1.0f;
		else if (fDot <= -1.0f) fDot = -1.0f;
		fSearchRange = fDot;

		if (0.0f <= fSearchRange && fSearchRange <= 1.0f)
		{
			d3dxvInspectionIdx.x = d3dxvColLayerRow.x + d3dxvSearchDir[i].x;
			d3dxvInspectionIdx.y = d3dxvColLayerRow.y + d3dxvSearchDir[i].y;
			d3dxvInspectionIdx.z = d3dxvColLayerRow.z + d3dxvSearchDir[i].z;

			if ((0 <= d3dxvInspectionIdx.x && d3dxvInspectionIdx.x < m_iMaxCol)
				&& (0 <= d3dxvInspectionIdx.y && d3dxvInspectionIdx.y < m_iMaxLayer)
				&& (0 <= d3dxvInspectionIdx.z && d3dxvInspectionIdx.z < m_iMaxRow))
			{
				if (!m_ppVoxelObjectsForCollision[iLinearIdx =
					(int)((d3dxvInspectionIdx.y) * m_iLayerStride
						+ (d3dxvInspectionIdx.z) * m_iRowStride
						+ (d3dxvInspectionIdx.x))])
				{
					if (fSearchRange > 0.07f) return false;
					else
					{
						iSurfaceCount++;
						if (iSurfaceCount == 3) return true;

					}
				}
				else
				{
					if (m_ppVoxelObjectsForCollision[iLinearIdx]->m_bIsSlope) return false;
					iSurfaceCount++;
					if (iSurfaceCount == 3) return true;
				}
			}
			else
			{
				if (fSearchRange > 0.07f) return false;
				else
				{
					iSurfaceCount++;
					if (iSurfaceCount == 3) return true;
				}
			}
		}
	}

	//printf("d3dxvColLayerRow : (%f, %f, %f) \n", d3dxvColLayerRow.x, d3dxvColLayerRow.y, d3dxvColLayerRow.z);
	return true;
}

void CVoxelTerrain::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	int size;
	static CTexture *pTexture[VOXEL_TYPE_NUM] =
	{
		NULL,
		CTextureResource::pGrassTexture, CTextureResource::pWoodTexture,
		CTextureResource::pUphillTexture, CTextureResource::pUphillTexture, CTextureResource::pUphillTexture, CTextureResource::pUphillTexture,
		CTextureResource::pBushTexture, CTextureResource::pSwampTexture, CTextureResource::pJumpTexture,
		CTextureResource::pSnowTexture, CTextureResource::pEmptySnowTexture, CTextureResource::pIceTexture
	};
	m_pShader->Render(pd3dDeviceContext);

	for (int i = 1; i < VOXEL_TYPE_NUM; i++)
	{
		m_pShader->UpdateShaderVariables(pd3dDeviceContext, pTexture[i]);
		size = m_vecVoxelObjectsForRendering[i].size();
		for (int j = 0; j < size; j++)
		{
			if (m_vecVoxelObjectsForRendering[i][j]->IsVisible(pCamera))
			{
				if (!IsInnerVoxel(m_vecVoxelObjectsForRendering[i][j], pCamera))
				{
					if (m_vecVoxelObjectsForRendering[i][j]->m_bIsVisible)
					{
						m_vecVoxelObjectsForRendering[i][j]->Render(pd3dDeviceContext, m_pShader);
					}
				}
			}
		}
	}
	/*for (int i = 0; i < m_nVoxelObjectsNum; i++)
	{
		if(m_ppVoxelObjectsForRendering[i]->m_bIsVisible) m_ppVoxelObjectsForRendering[i]->Render(pd3dDeviceContext, m_pShader);
	}*/
}