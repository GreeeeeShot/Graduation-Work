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

	
	/*CCubeHeightInterpolationFace m_CubeHeightInterpolationFace;*/

	//���� �����͸� �������ϴ� ��� �Լ��� �����Ѵ�.
public:
	AABB m_AABB;
	CCubeFaceNormal m_CubeFaceNormal;
	CCubeSlopeFace m_CubeSlopeFace;

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed) {}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CTexturedRectMesh : public CMesh
{
public:
	CTexturedRectMesh(ID3D11Device *pd3dDevice,
		float fWidth = 2.0f, float fHeight = 2.0f);
	~CTexturedRectMesh();

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
};

class CTexturedSpritePieceMesh : public CMesh
{
public:
	CTexturedSpritePieceMesh(ID3D11Device *pd3dDevice, float fPieceW, float fPieceH, int nPieceNum, int iExtractionIdx);
	~CTexturedSpritePieceMesh();

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
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

class CTexturedLightingGridMesh : public CLightingMesh
{
protected:
	int m_iWidth;
	int m_iLength;

	//D3DXVECTOR3 m_d3dxvScale;

	float m_fStartX;
	float m_fStartZ;

	float m_fIntervalX;
	float m_fIntervalZ;

public:
	CTexturedLightingGridMesh(ID3D11Device *pd3dDevice,
		float fStartX, float fStartZ,
		float fIntervalX, float fIntervalZ,
		int nWidth, int nLength);
	~CTexturedLightingGridMesh();

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	//virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	//���� �޽��� ���̸� ��ȭ��Ų��.
	virtual void Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed);
};

class CMeshResource
{
public:
	CMeshResource();
	~CMeshResource();

	static CMesh *pPirateMesh;
	static CMesh *pSnowWomanMesh;
	static CMesh* pWaveMesh;

	static CMesh **ppRegisteredVoxelMesh;
	static CMesh *pStandardVoxelMesh;
	static CMesh *pUphill000Mesh;
	static CMesh *pUphill090Mesh;
	static CMesh *pUphill180Mesh;
	static CMesh *pUphill270Mesh;

	static CMesh *pUI_VoxelPocketMesh;

	static void CreateMeshResource(ID3D11Device *pd3dDevice);
};
