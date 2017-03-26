#pragma once
#include "Object.h"
#include "Voxel.h"

class CWorkingSpace
{
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

	CGameObject *m_pLocalCoord;
	CGameObject *m_pWorldCoord;
	CGameObject *m_pWorkingSpace;
	CGameObject *m_pWorkingBoard;

	CVoxel** m_ppVoxelObjects;

	void CreateWorkingSpace(ID3D11Device *pd3dDevice, D3DXVECTOR3 d3dxvWorkingSpaceSize, D3DXVECTOR3 d3dxvCubeSize, D3DXVECTOR3 d3dxvOffsetFromLocal);
	
	void MoveUpWorkingBoard(void);
	void MoveDownWorkingBoard(void);

	/*D3DXVECTOR3 CreateRayVector(ID3D11Device *pd3dDevice, CCamera *pCamera, int iMouseX, int iMouseY);
	void CreateVoxelObejct(ID3D11Device *pd3dDevice, CCamera *pCamera, int iMouseX, int iMouseY);*/

	void Render(ID3D11DeviceContext *pd3dDeviceContext);

	// void MakeBlueprint(const char* strBPFileName);

	CWorkingSpace();
	~CWorkingSpace();
};

