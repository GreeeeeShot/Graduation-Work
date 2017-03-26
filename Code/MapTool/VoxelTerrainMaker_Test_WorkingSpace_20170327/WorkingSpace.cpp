#include "stdafx.h"
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

	m_pLocalCoord = NULL;
	m_pWorldCoord = NULL;
	m_pWorkingSpace = NULL;
	m_pWorkingBoard = NULL;

	m_ppVoxelObjects = NULL;
}


CWorkingSpace::~CWorkingSpace()
{
	if(m_pLocalCoord) delete m_pLocalCoord;
	if(m_pWorldCoord) delete m_pWorldCoord;
	if(m_pWorkingSpace) delete m_pWorkingSpace;
	if(m_pWorkingBoard) delete m_pWorkingBoard;

	if(m_ppVoxelObjects) delete[]m_ppVoxelObjects;
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

	CShader *pDiffusedShader = new CShader();
	pDiffusedShader->CreateShader(pd3dDevice);
	
	m_pLocalCoord = new CGameObject();
	m_pLocalCoord->SetMesh(CMeshResource::pLocalCoordMesh);
	m_pLocalCoord->SetShader(pDiffusedShader);

	m_pWorldCoord = new CGameObject();
	m_pWorldCoord->SetMesh(CMeshResource::pWorldCoordMesh);
	m_pWorldCoord->SetShader(pDiffusedShader);

	m_pWorkingSpace = new CGameObject();
	CMesh *pWorkingSpace = new CDiffusedWorkingSpaceMesh(pd3dDevice, m_nMaxCol * m_fCubeWidth, m_nMaxLayer * m_fCubeHeight, m_nMaxRow * m_fCubeDepth);
	m_pWorkingSpace->SetMesh(pWorkingSpace);
	m_pWorkingSpace->SetShader(pDiffusedShader);
	
	m_pWorkingBoard = new CGameObject();
	CMesh *pWorkingBoard = new CDiffusedWorkingBoardMesh(pd3dDevice, m_nMaxCol, m_fCubeWidth, m_nMaxRow, m_fCubeDepth);
	m_pWorkingBoard->SetMesh(pWorkingBoard);
	m_pWorkingBoard->SetShader(pDiffusedShader);

	// 복셀 오브젝트는 나중에.

}

void CWorkingSpace::MoveUpWorkingBoard(void)
{
	if (m_nMaxLayer * m_fCubeHeight > m_pWorkingBoard->GetPosition().y + m_fCubeHeight)
	{
		m_pWorkingBoard->m_d3dxmtxWorld._42 = m_pWorkingBoard->GetPosition().y + m_fCubeHeight;
	}
	//printf("m_pWorkingBoard->m_d3dxmtxWorld._42 : %f \n", m_pWorkingBoard->m_d3dxmtxWorld._42);
}

void CWorkingSpace::MoveDownWorkingBoard(void)
{
	if (0.0f >= m_pWorkingBoard->GetPosition().y) return;

	m_pWorkingBoard->m_d3dxmtxWorld._42 = m_pWorkingBoard->GetPosition().y - m_fCubeHeight;
	//printf("m_pWorkingBoard->m_d3dxmtxWorld._42 : %f \n", m_pWorkingBoard->m_d3dxmtxWorld._42);
}

void CWorkingSpace::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if(m_pLocalCoord) m_pLocalCoord->Render(pd3dDeviceContext);
	if(m_pWorldCoord) m_pWorldCoord->Render(pd3dDeviceContext);
	if(m_pWorkingBoard) m_pWorkingBoard->Render(pd3dDeviceContext);
	//printf("m_pWorkingBoard->GetPosition().y : %f \n", m_pWorkingBoard->GetPosition().y);
	if(m_pWorkingSpace) m_pWorkingSpace->Render(pd3dDeviceContext);
}