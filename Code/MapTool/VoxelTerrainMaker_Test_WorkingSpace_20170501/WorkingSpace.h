#pragma once
#include "Object.h"
#include "Camera.h"
#include "LightResource.h"
//#include "Voxel.h"
//#include "VoxelTerrain.h"

enum VIEW_MODE { VIEW_MODE_ALL, VIEW_MODE_ACCUMULATE, VIEW_MODE_RELEVANT };

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
	CGameObject *m_pActivedWorkingSpace;
	CGameObject *m_pWorkingBoard;
	int m_iWorkingBoardCurLayer;

	CVoxel** m_ppVoxelObjects;
	D3DXMATRIX m_d3dxmtxFromIdxToPos;
	D3DXMATRIX m_d3dxmtxFromPosToIdx;
	int m_iBlueprint[BLUEPRINT_LAYER][BLUEPRINT_ROW][BLUEPRINT_COL];

	int m_iSelectedMtrl;
	VIEW_MODE m_eViewMode;

	CWorkingSpace();
	~CWorkingSpace();

	void CreateWorkingSpace(ID3D11Device *pd3dDevice, D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal);
	void ReviseWorkingSpace(ID3D11Device *pd3dDevice, D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal);
	void OptimizeWorkingSpace(ID3D11Device *pd3dDevice);
	
	void SaveWorkingSpaceToFile(FILE *pBluePrintFP);
	void ChangeMaterial(int iMtrl);
	void ChangeViewMode(VIEW_MODE eViewMode);
	CBlueprint* CreateBlueprint(void);

	void MoveUpWorkingBoard(void);
	void MoveDownWorkingBoard(void);

	void MoveUpVoxelTerrain(void);
	void MoveDownVoxelTerrain(void);
	void MoveForwardVoxelTerrain(void);
	void MoveRearVoxelTerrain(void);
	void MoveRightVoxelTerrain(void);
	void MoveLeftVoxelTerrain(void);

	void MoveUpWorkingSpace(void);
	void MoveDownWorkingSpace(void);
	void MoveForwardWorkingSpace(void);
	void MoveRearWorkingSpace(void);
	void MoveRightWorkingSpace(void);
	void MoveLeftWorkingSpace(void);

	void CreateRayVector(CCamera *pCamera, int iMouseX, int iMouseY, D3DXVECTOR3 *pRayPos, D3DXVECTOR3 *pRayDir);
	void CreateVoxelObject(ID3D11Device *pd3dDevice, CCamera *pCamera, int iMouseX, int iMouseY);
	void DestroyVoxelObject(ID3D11Device *pd3dDevice, CCamera *pCamera, int iMouseX, int iMouseY);

	void CreateVoxelObjectByPaint(ID3D11Device *pd3dDevice, CCamera *pCamera, int iFixedX, int iFixedY, int iMouseX, int iMouseY);
	void DestroyVoxelObjectByPaint(ID3D11Device *pd3dDevice, CCamera *pCamera, int iFixedX, int iFixedY, int iMouseX, int iMouseY);

	CVoxel* GetVoxelForType(VOXEL_TYPE eVoxelType);
	void Render(ID3D11DeviceContext *pd3dDeviceContext, bool bIsActived);

	// void MakeBlueprint(const char* strBPFileName);
};

