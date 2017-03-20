#include "stdafx.h"
#include "TextureResource.h"
#include "Voxel.h"
#include "MeshResource.h"
#include "VoxelTerrain.h"

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
	m_pShader = NULL;
	m_fGravitationalAcceleration = GRAVITATIONAL_ACCELERATION;

	m_ppVoxelObjects = NULL;
}


CVoxelTerrain::~CVoxelTerrain()
{
}

void CVoxelTerrain::CreateVoxelTerrain(CBlueprint* pBlueprint, CShader* pShader)
{
	if (!pBlueprint->m_nVoxelObjectsNum) exit(-1);

	m_ppVoxelObjects = new CVoxel*[pBlueprint->m_nVoxelObjectsNum];

	int iMaxLayer = pBlueprint->m_nMaxLayer;
	int iMaxRow = pBlueprint->m_nMaxRow;
	int iMaxCol = pBlueprint->m_nMaxCol;

	float fOffsetXFromLocal = pBlueprint->m_fOffsetXFromLocal;
	float fOffsetYFromLocal = pBlueprint->m_fOffsetYFromLocal;
	float fOffsetZFromLocal = pBlueprint->m_fOffsetZFromLocal;

	float fCubeHeight = pBlueprint->m_fCubeHeight;
	float fCubeWidth = pBlueprint->m_fCubeWidth;
	float fCubeDepth = pBlueprint->m_fCubeDepth;

	CMaterial *pStandardMaterial = new CMaterial();
	pStandardMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pStandardMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	int iSelectedTexture;

	for (int layer = 0; layer < pBlueprint->m_nMaxLayer; layer++)
	{
		for (int row = 0; row < pBlueprint->m_nMaxRow ; row++)
		{
			for (int col = 0; col < pBlueprint->m_nMaxCol ; col++)
			{
				if (iSelectedTexture = pBlueprint->m_iBlueprint[layer][row][col])
				{
					m_ppVoxelObjects[m_nVoxelObjectsNum] = new CVoxel();
					m_ppVoxelObjects[m_nVoxelObjectsNum]->SetShader(pShader);
					m_ppVoxelObjects[m_nVoxelObjectsNum]->SetMaterial(pStandardMaterial);
					CTexture *pTexture = new CTexture(1);
					pTexture->SetTexture(0, CTextureResource::ppd3dTexture[iSelectedTexture-1], CTextureResource::ppd3dSamplerState[SAMPLER_WRAP_MODE_0]);
					m_ppVoxelObjects[m_nVoxelObjectsNum]->SetTexture(pTexture);
					m_ppVoxelObjects[m_nVoxelObjectsNum]->SetMesh(CMeshResource::pVoxelMesh);
					m_ppVoxelObjects[m_nVoxelObjectsNum]->SetPosition(
						-(iMaxCol - 1) * fCubeWidth / 2.0f + fCubeWidth * col + fOffsetXFromLocal,
						-(iMaxLayer - 1) * fCubeHeight / 2.0f + fCubeHeight * layer + fOffsetYFromLocal,
						-(iMaxRow - 1) * fCubeDepth / 2.0f + fCubeDepth * row + fOffsetZFromLocal);

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
		m_ppVoxelObjects[i]->Render(pd3dDeviceContext);
	}
}