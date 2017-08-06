#pragma once
//#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "Voxel.h"

#define BLUEPRINT_LAYER						50	// ��
#define BLUEPRINT_ROW						100	// ��
#define BLUEPRINT_COL						100	// ��

//���赵�� ���� ������ ������� ����ü
class CBlueprint
{
public:
	CBlueprint();
	~CBlueprint();

	int m_iMapTotalNum;
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

	// �Ϲ� ����
	CVoxel** m_ppVoxelObjectsForCollision;
	//CVoxel** m_ppVoxelObjectsForRendering;
	std::vector<CVoxel*> m_vecVoxelObjectsForRendering[VOXEL_TYPE_NUM];		// 0��° �ε����� ������� �ʴ´�.

	// �ϵ���� �ν��Ͻ� ����
	UINT m_iRectVoxelNum = 0;
	UINT m_iSlopeVoxelNum = 0;

	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;

	ID3D11Buffer *m_pd3dSlopeVoxelInstanceBuffer[4];
	ID3D11Buffer *m_pd3dRectVoxelInstanceBuffer;			// 4������ ������ ����

	D3DXMATRIX m_d3dxmtxFromIdxToPos;
	D3DXMATRIX m_d3dxmtxFromPosToIdx;

	void CreateVoxelTerrain(ID3D11Device *pd3dDevice, CBlueprint* pBlueprint);
	ID3D11Buffer *CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData);
	int GetTexture2DArrayIndexForType(VOXEL_TYPE &eVoxelType);
	void SetShader(CShader *pShader);
	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	static CVoxel* GetVoxelForType(VOXEL_TYPE eVoxelType);
	D3DXVECTOR3 GetPosition();
	bool IsInnerVoxel(CVoxel* pVoxel, CCamera* pCamera);
	void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera, bool bIsInstancing = false);
};

