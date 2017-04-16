#pragma once
#include "LightResource.h"
#include "TextureResource.h"
#include "PhysicalCollision.h"

enum VOXEL_SLOPE_DIR { VOXEL_SLOPE_DIR_FRONT, VOXEL_SLOPE_DIR_RIGHT, VOXEL_SLOPE_DIR_REAR, VOXEL_SLOPE_DIR_LEFT };

class CCubeFaceNormal
{
public:
	D3DXVECTOR3 m_d3dxLeftNormal;
	D3DXVECTOR3 m_d3dxRightNormal;
	D3DXVECTOR3 m_d3dxFrontNormal;
	D3DXVECTOR3 m_d3dxRearNormal;
	D3DXVECTOR3 m_d3dxTopNormal;
	D3DXVECTOR3 m_d3dxBottomNormal;

	CCubeFaceNormal();
	~CCubeFaceNormal();
};

class CCubeSlopeFace
{
public:
	D3DXVECTOR3 m_d3dxvUpNegative;
	D3DXVECTOR3 m_d3dxvUpPositive;
	D3DXVECTOR3 m_d3dxvDownNegative;
	D3DXVECTOR3 m_d3dxvDownPositive;

	CCubeSlopeFace();
	~CCubeSlopeFace();
};

class CMesh
{
public:
	//CMesh Ŭ������ �����ڿ� �Ҹ��ڸ� �����Ѵ�. 
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

	//CMesh Ŭ���� ��ü�� ����(Reference)�� ���õ� ��� ������ �Լ��� �����Ѵ�.
private:
	int m_nReferences;

public:
	void AddRef();
	void Release();

protected:
	//������ ��ҵ��� ��Ÿ���� ���۵��� �Է������⿡ �����ϱ� ���� �����̴�. 
	ID3D11Buffer **m_ppd3dVertexBuffers;

	int m_nVertices;
	D3DXVECTOR3 * m_pd3dxvPositions;
	//������ ��ġ ���͸� �����ϱ� ���� ���ۿ� ���� �������̽� �������̴�. 
	ID3D11Buffer *m_pd3dPositionBuffer;

	//������ �����ϱ� ���� �ʿ��� ������ �����̴�. 
	int m_nBuffers;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ������ ����Ʈ ���� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexStrides;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ���� ��ġ(����Ʈ ��)�� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexOffsets;

	/*�ε��� ����(�ε����� �迭)�� ���� �������̽� �������̴�. �ε��� ���۴� ���� ����(�迭)�� ���� �ε����� ������.*/
	ID3D11Buffer *m_pd3dIndexBuffer;
	//�ε��� ���۰� �����ϴ� �ε����� �����̴�. 
	UINT m_nIndices;
	//�ε��� ���ۿ��� �޽��� ǥ���ϱ� ���� ���Ǵ� ���� �ε����̴�. 
	UINT m_nStartIndex;
	//�� �ε����� ������ �ε����̴�. 
	int m_nBaseVertex;

	//���� �����Ͱ� � ������Ƽ�긦 ǥ���ϰ� �ִ� ���� ��Ÿ���� ��� ������ �����Ѵ�.
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	/*�����Ͷ����� ���� ��ü�� ���� �������̽� �����͸� �����Ѵ�. �����Ͷ����� ���� ��ü�� ��CGameObject" Ŭ������ ����� �߰��Ͽ��� �ȴ�. �����Ͷ����� ���� ��ü�� �޽��� ����� ���� ���� ��ü�� ����� ���� �ǹ̻��� ���̸� �����غ��⸦ �ٶ���.*/
	ID3D11RasterizerState *m_pd3dRasterizerState;

	//���� �����͸� �������ϴ� ��� �Լ��� �����Ѵ�.
public:
	AABB m_AABB;
	CCubeFaceNormal m_CubeFaceNormal;
	CCubeSlopeFace m_CubeSlopeFace;

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CDiffusedMesh : public CMesh
{
public:
	CDiffusedMesh(ID3D11Device *pd3dDevice);
	virtual ~CDiffusedMesh();
};

/* -------------------<<<<<< CDiffusedCoordSystemMesh >>>>>>------------------- */
class CDiffusedCoordSystemMesh : public CDiffusedMesh
{
public :
	CDiffusedCoordSystemMesh(
		ID3D11Device *pd3dDevice,
		float AxisXLength = 10.0f, float AxisYLength = 10.0f, float AxisZLength = 10.0f,
		D3DXCOLOR AxisXColor = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),
		D3DXCOLOR AxisYColor = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f),
		D3DXCOLOR AxisZColor = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)
	);
	virtual ~CDiffusedCoordSystemMesh();
};

/* -------------------<<<<<< CDiffusedWorkingSpaceMesh >>>>>>------------------- */
class CDiffusedWorkingSpaceMesh : public CDiffusedMesh
{
public: 
	CDiffusedWorkingSpaceMesh(ID3D11Device *pd3dDevice, float fWidth = 1.0f, float fHeight = 1.0f, float fDepth = 1.0f);
	virtual ~CDiffusedWorkingSpaceMesh();

	//virtual void SetRasterizeState(ID3D11Device *pd3dDevice);
};

/* -------------------<<<<<< CDiffusedWorkingBoardMesh >>>>>>------------------- */
class CDiffusedWorkingBoardMesh : public CDiffusedMesh
{
public:
	CDiffusedWorkingBoardMesh(ID3D11Device *pd3dDevice, int iWidthCount, float fWidthIntervalUnit, int iDepthCount, float fDepthIntervalUnit);
	virtual ~CDiffusedWorkingBoardMesh();

	//virtual void SetRasterizeState(ID3D11Device *pd3dDevice);
};

class CLightingMesh : public CMesh
{
public:
	CLightingMesh(ID3D11Device *pd3dDevice);
	virtual ~CLightingMesh();

protected:
	//������ ������ ����ϱ� ���Ͽ� �������Ͱ� �ʿ��ϴ�.
	ID3D11Buffer *m_pd3dNormalBuffer;

public:
	void CalculateVertexNormal(D3DXVECTOR3 *pd3dxvNormals, D3DXVECTOR3 *pd3dxvVertices, UINT *piIndices);
};

/* -------------------<<<<<< CTexturedLightingCubeMesh >>>>>>------------------- */
class CTexturedLightingCubeMesh : public CLightingMesh
{
public:
	CTexturedLightingCubeMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CTexturedLightingCubeMesh();

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	//virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CTexturedLightingUphillMesh : public CLightingMesh
{
public:
	CTexturedLightingUphillMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, VOXEL_SLOPE_DIR eVoxelDir = VOXEL_SLOPE_DIR_FRONT);
	virtual ~CTexturedLightingUphillMesh();

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	//virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CMeshResource
{
public:
	CMeshResource();
	~CMeshResource();

	static CMesh *pPirateMesh;
	static CMesh *pSnowWomanMesh;

	static CMesh *pLocalCoordMesh;
	static CMesh *pWorldCoordMesh;

	static CMesh **ppRegisteredVoxelMesh;
	static CMesh *pStandardVoxelMesh;
	static CMesh *pUphill000Mesh;
	static CMesh *pUphill090Mesh;
	static CMesh *pUphill180Mesh;
	static CMesh *pUphill270Mesh;

	static void CreateMeshResource(ID3D11Device *pd3dDevice);
};
