#pragma once
#include "Shader.h"
#include "Object.h"
#include "Voxel.h"

#define BLUEPRINT_LAYER						50	// ��
#define BLUEPRINT_ROW						100	// ��
#define BLUEPRINT_COL						100	// ��

#define DAMPING_WOOD_0						0.05f
#define DAMPING_GRASS_0						9.0f

#define DAMPING_UPHILL						9.0f
#define DAMPING_NULL						0.0f

#define REFLECTION_WOOD_0					0.5f
#define REFLECTION_GRASS_0					0.36f
#define REFLECTION_NULL						0.0f

//���赵�� ���� ������ ������� ����ü
class CBlueprint
{
public:
	CBlueprint();
	~CBlueprint();

	int m_iBlueprint[BLUEPRINT_LAYER][BLUEPRINT_ROW][BLUEPRINT_COL];
	int m_nVoxelObjectsNum;
	int m_nMaxLayer;
	int m_nMaxCol;
	int m_nMaxRow;
	float m_fCubeWidth;
	float m_fCubeHeight;
	float m_fCubeDepth;
	float m_fOffsetXFromLocal;
	float m_fOffsetYFromLocal;
	float m_fOffsetZFromLocal;

	// txt�� ����� �� ������ �����´�.
	CBlueprint* LoadBlueprint(const char *strBlueprintFileName);
};

class CVoxelTerrain
{
private:
	int m_nVoxelObjectsNum;
	D3DXMATRIX m_d3dxmtxWorld;
	CShader *m_pShader;

public:
	CVoxelTerrain();
	~CVoxelTerrain();

	int m_iMaxLayer;
	int m_iMaxRow;
	int m_iMaxCol;

	int m_iLayerStride;
	int m_iRowStride;

	float m_fOffsetXFromLocal;
	float m_fOffsetYFromLocal;
	float m_fOffsetZFromLocal;

	float m_fCubeHeight;
	float m_fCubeWidth;
	float m_fCubeDepth;

	CVoxel** m_ppVoxelObjectsForCollision;
	CVoxel** m_ppVoxelObjectsForRendering;

	D3DXMATRIX m_d3dxmtxFromIdxToPos;

	void CreateVoxelTerrain(CBlueprint* pBlueprint, CShader* pShader);
	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();
	void Render(ID3D11DeviceContext *pd3dDeviceContext);
};
