#pragma once
#include "Shader.h"
#include "Object.h"
#include "Voxel.h"

#define BLUEPRINT_LAYER						50	// 개
#define BLUEPRINT_ROW						100	// 개
#define BLUEPRINT_COL						100	// 개

//설계도에 대한 정보를 담기위한 구조체
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

	// txt로 저장된 맵 파일을 가져온다.
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
	//CVoxel** m_ppVoxelObjectsForRendering;
	std::vector<CVoxel*> m_vecVoxelObjectsForRendering[VOXEL_TYPE_NUM];		// 0번째 인덱스는 사용하지 않는다.

	D3DXMATRIX m_d3dxmtxFromIdxToPos;

	void CreateVoxelTerrain(CBlueprint* pBlueprint, CShader* pShader);
	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	CVoxel* GetVoxelForType(VOXEL_TYPE eVoxelType);
	D3DXVECTOR3 GetPosition();
	void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

