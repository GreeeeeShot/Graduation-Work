#pragma once
#include "Object.h"
#include "Camera.h"
#include "LightResource.h"
//#include "Voxel.h"
//#include "VoxelTerrain.h"

class CBlueprint;

class CWorkingSpace
{
private:
	CShader *m_pShader;

public:
	int m_nMaxLayer;
	int m_nMaxCol;
	int m_nMaxRow;
	float m_fCubeWidth;
	float m_fCubeHeight;
	float m_fCubeDepth;
	float m_fOffsetXFromLocal;
	float m_fOffsetYFromLocal;
	float m_fOffsetZFromLocal;

	int m_iLayerStride;
	int m_iRowStride;

	CGameObject *m_pLocalCoord;
	CGameObject *m_pWorldCoord;
	CGameObject *m_pWorkingSpace;
	CGameObject *m_pWorkingBoard;

	CVoxel** m_ppVoxelObjects;
	D3DXMATRIX m_d3dxmtxFromIdxToPos;
	D3DXMATRIX m_d3dxmtxFromPosToIdx;
	int m_iBlueprint[BLUEPRINT_LAYER][BLUEPRINT_ROW][BLUEPRINT_COL];

	int m_iSelectedMtrl;

	CWorkingSpace();
	~CWorkingSpace();

	void CreateWorkingSpace(ID3D11Device *pd3dDevice, D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal);
	void SaveWorkingSpaceToFile(const char* pFileName);
	void ChangeMaterial(int iMtrl);
	CBlueprint* CreateBlueprint(void);

	void MoveUpWorkingBoard(void);
	void MoveDownWorkingBoard(void);

	void CreateRayVector(CCamera *pCamera, int iMouseX, int iMouseY, D3DXVECTOR3 *pRayPos, D3DXVECTOR3 *pRayDir);
	void CreateVoxelObject(ID3D11Device *pd3dDevice, CCamera *pCamera, int iMouseX, int iMouseY);
	void DestroyVoxelObject(ID3D11Device *pd3dDevice, CCamera *pCamera, int iMouseX, int iMouseY);

	void CreateVoxelObjectByPaint(ID3D11Device *pd3dDevice, CCamera *pCamera, int iFixedX, int iFixedY, int iMouseX, int iMouseY);
	void DestroyVoxelObjectByPaint(ID3D11Device *pd3dDevice, CCamera *pCamera, int iFixedX, int iFixedY, int iMouseX, int iMouseY);

	CVoxel* GetVoxelForType(VOXEL_TYPE eVoxelType);
	void Render(ID3D11DeviceContext *pd3dDeviceContext);

	// void MakeBlueprint(const char* strBPFileName);
};

