#pragma once
#include "LightResource.h"
#include "TextureResource.h"
#include "PhysicalCollision.h"

enum VOXEL_SLOPE_DIR { VOXEL_SLOPE_DIR_FRONT, VOXEL_SLOPE_DIR_RIGHT, VOXEL_SLOPE_DIR_LEFT, VOXEL_SLOPE_DIR_REAR };

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

//struct InterpolationFaceXY				// Vertical
//{
//	D3DXVECTOR2 m_d3dxNegativeX;		// 플레이어 x 기반 높이 구하기
//	D3DXVECTOR2 m_d3dxPositiveX;
//};
//
//struct InterpolationFaceYZ				// Horizonal
//{
//	D3DXVECTOR2 m_d3dxPositiveZ;		// 플레이어 z 기반 높이 구하기
//	D3DXVECTOR2 m_d3dxNegativeZ;
//};
//
//
//class CCubeHeightInterpolationFace
//{
//public :
//	bool m_bIsVerticalAllocated;
//	InterpolationFaceXY m_XY;
//	InterpolationFaceYZ m_YZ;
//
//	CCubeHeightInterpolationFace(bool bIsVerticalAllocated = true) : m_bIsVerticalAllocated(bIsVerticalAllocated) {}
//
//	float GetHeightByPlayerPos(D3DXMATRIX mtxObjWorld, D3DXVECTOR3 d3dxPlayerPos)
//	{
//		D3DXVECTOR2 d3dxMovedNegative, d3dxMovedPositive;
//		float fHeight = 0;
//		float fU;
//
//		if (m_bIsVerticalAllocated)
//		{
//			d3dxMovedNegative.x = m_XY.m_d3dxNegativeX.x + mtxObjWorld._41;
//			d3dxMovedNegative.y = m_XY.m_d3dxNegativeX.y + mtxObjWorld._42;
//
//			d3dxMovedPositive.x = m_XY.m_d3dxPositiveX.x + mtxObjWorld._41;
//			d3dxMovedPositive.y = m_XY.m_d3dxPositiveX.y + mtxObjWorld._42;
//
//			fU = (d3dxPlayerPos.x - d3dxMovedNegative.x) / (d3dxMovedPositive.x - d3dxMovedNegative.x);
//			fHeight = d3dxMovedNegative.y * (1.0f - fU) + d3dxMovedPositive.y * fU;
//		}
//		else
//		{
//			d3dxMovedNegative.x = m_YZ.m_d3dxNegativeZ.x + mtxObjWorld._43;			// x는 이때 z로 취급한다.
//			d3dxMovedNegative.y = m_YZ.m_d3dxNegativeZ.y + mtxObjWorld._42;
//
//			d3dxMovedPositive.x = m_YZ.m_d3dxPositiveZ.x + mtxObjWorld._43;
//			d3dxMovedPositive.y = m_YZ.m_d3dxPositiveZ.y + mtxObjWorld._42;
//
//			fU = (d3dxPlayerPos.z - d3dxMovedNegative.x) / (d3dxMovedPositive.x - d3dxMovedNegative.x);
//			fHeight = d3dxMovedNegative.y * (1.0f - fU) + d3dxMovedPositive.y * fU;
//		}
//
//		return fHeight;
//	}
//};

class CMesh
{
public:
	//CMesh 클래스의 생성자와 소멸자를 선언한다. 
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

	//CMesh 클래스 객체의 참조(Reference)와 관련된 멤버 변수와 함수를 선언한다.
private:
	int m_nReferences;

public:
	void AddRef();
	void Release();

protected:
	//정점의 요소들을 나타내는 버퍼들을 입력조립기에 전달하기 위한 버퍼이다. 
	ID3D11Buffer **m_ppd3dVertexBuffers;

	int m_nVertices;
	D3DXVECTOR3 * m_pd3dxvPositions;
	//정점의 위치 벡터를 저장하기 위한 버퍼에 대한 인터페이스 포인터이다. 
	ID3D11Buffer *m_pd3dPositionBuffer;

	//정점을 조립하기 위해 필요한 버퍼의 개수이다. 
	int m_nBuffers;
	//정점의 요소들을 나타내는 버퍼들의 원소의 바이트 수를 나타내는 배열이다. 
	UINT *m_pnVertexStrides;
	//정점의 요소들을 나타내는 버퍼들의 시작 위치(바이트 수)를 나타내는 배열이다. 
	UINT *m_pnVertexOffsets;

	/*인덱스 버퍼(인덱스의 배열)에 대한 인터페이스 포인터이다. 인덱스 버퍼는 정점 버퍼(배열)에 대한 인덱스를 가진다.*/
	ID3D11Buffer *m_pd3dIndexBuffer;
	//인덱스 버퍼가 포함하는 인덱스의 개수이다. 
	UINT m_nIndices;
	//인덱스 버퍼에서 메쉬를 표현하기 위해 사용되는 시작 인덱스이다. 
	UINT m_nStartIndex;
	//각 인덱스에 더해질 인덱스이다. 
	int m_nBaseVertex;

	//정점 데이터가 어떤 프리미티브를 표현하고 있는 가를 나타내는 멤버 변수를 선언한다.
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	/*래스터라이저 상태 객체에 대한 인터페이스 포인터를 선언한다. 래스터라이저 상태 객체는 “CGameObject" 클래스의 멤버로 추가하여도 된다. 래스터라이저 상태 객체가 메쉬의 멤버일 때와 게임 객체의 멤버일 때의 의미상의 차이를 생각해보기를 바란다.*/
	ID3D11RasterizerState *m_pd3dRasterizerState;

	
	/*CCubeHeightInterpolationFace m_CubeHeightInterpolationFace;*/

	//정점 데이터를 렌더링하는 멤버 함수를 선언한다.
public:
	AABB m_AABB;
	CCubeFaceNormal m_CubeFaceNormal;
	CCubeSlopeFace m_CubeSlopeFace;

	virtual void SetRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CLightingMesh : public CMesh
{
public:
	CLightingMesh(ID3D11Device *pd3dDevice);
	virtual ~CLightingMesh();

protected:
	//조명의 영향을 계산하기 위하여 법선벡터가 필요하다.
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

class CMeshResource
{
public:
	CMeshResource();
	~CMeshResource();

	static CMesh *pPirateMesh;
	static CMesh *pSnowWomanMesh;
	static CMesh *pVoxelMesh;

	static CMesh *pUphill000Mesh;
	static CMesh *pUphill090Mesh;
	static CMesh *pUphill180Mesh;
	static CMesh *pUphill270Mesh;

	static void CreateMeshResource(ID3D11Device *pd3dDevice);
};
