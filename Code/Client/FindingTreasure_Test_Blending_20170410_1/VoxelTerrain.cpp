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

	fclose(bluePrintFp);

	return this;
}

CVoxelTerrain::CVoxelTerrain()
{
	m_nVoxelObjectsNum = 0;
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	D3DXMatrixIdentity(&m_d3dxmtxFromIdxToPos);
	m_pShader = NULL;

	m_ppVoxelObjectsForCollision = NULL;
	m_ppVoxelObjectsForRendering = NULL;
}


CVoxelTerrain::~CVoxelTerrain()
{
	if (m_ppVoxelObjectsForRendering)
	{
		for (int i = 0; i < m_nVoxelObjectsNum; i++)
		{
			delete m_ppVoxelObjectsForRendering[i];
		}
		delete []m_ppVoxelObjectsForRendering;
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

void CVoxelTerrain::CreateVoxelTerrain(CBlueprint* pBlueprint, CShader* pShader)
{
	if (!pBlueprint->m_nVoxelObjectsNum) exit(-1);

	m_iMaxLayer = pBlueprint->m_nMaxLayer;
	m_iMaxRow = pBlueprint->m_nMaxRow;
	m_iMaxCol = pBlueprint->m_nMaxCol;

	m_iLayerStride = m_iMaxRow * m_iMaxCol;
	m_iRowStride = m_iMaxCol;

	m_ppVoxelObjectsForCollision = new CVoxel*[m_iMaxLayer * m_iMaxRow * m_iMaxCol];
	m_ppVoxelObjectsForRendering = new CVoxel*[pBlueprint->m_nVoxelObjectsNum];
	ZeroMemory(m_ppVoxelObjectsForCollision, sizeof(CVoxel*) * (m_iMaxLayer * m_iMaxRow * m_iMaxCol));
	ZeroMemory(m_ppVoxelObjectsForRendering, sizeof(CVoxel*) * pBlueprint->m_nVoxelObjectsNum);

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
	// m_d3dxmtxFromIdxToPos._42 = -(m_iMaxLayer - 1) * m_fCubeHeight / 2.0f + m_fOffsetYFromLocal;
	m_d3dxmtxFromIdxToPos._42 = m_fCubeHeight / 2.0f + m_fOffsetYFromLocal;
	m_d3dxmtxFromIdxToPos._43 = -(m_iMaxRow - 1) * m_fCubeDepth / 2.0f + m_fOffsetZFromLocal;

	VOXEL_TYPE iSelectedType;
	D3DXVECTOR4 d3dxvPosition;

	for (int layer = 0; layer < pBlueprint->m_nMaxLayer; layer++)
	{
		for (int row = 0; row < pBlueprint->m_nMaxRow ; row++)
		{
			for (int col = 0; col < pBlueprint->m_nMaxCol ; col++)
			{
				if (iSelectedType = (VOXEL_TYPE)pBlueprint->m_iBlueprint[layer][row][col])
				{
					m_ppVoxelObjectsForRendering[m_nVoxelObjectsNum] = GetVoxelForType(iSelectedType);
					m_ppVoxelObjectsForRendering[m_nVoxelObjectsNum]->SetShader(CShaderResource::pTexturedLightingShader);
					m_ppVoxelObjectsForRendering[m_nVoxelObjectsNum]->SetMaterial(CMaterialResource::pStandardMaterial);
					D3DXVec3Transform(&d3dxvPosition, &D3DXVECTOR3(col, layer, row), &m_d3dxmtxFromIdxToPos);
					m_ppVoxelObjectsForRendering[m_nVoxelObjectsNum]->SetPosition(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z);

					if (m_ppVoxelObjectsForRendering[m_nVoxelObjectsNum]->m_bIsActive)
					{
						m_ppVoxelObjectsForCollision[layer * m_iLayerStride + row * m_iRowStride + col]
							= m_ppVoxelObjectsForRendering[m_nVoxelObjectsNum];
					}
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

void CVoxelTerrain::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	for (int i = 0; i < m_nVoxelObjectsNum; i++)
	{
		if(m_ppVoxelObjectsForRendering[i]->m_bIsVisible) m_ppVoxelObjectsForRendering[i]->Render(pd3dDeviceContext);
	}
}