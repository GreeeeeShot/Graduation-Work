#include "stdafx.h"
#include "VertexResource.h"
#include "MeshResource.h"

CCubeFaceNormal::CCubeFaceNormal()
{
	m_d3dxLeftNormal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
	m_d3dxRightNormal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_d3dxFrontNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_d3dxRearNormal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	m_d3dxTopNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_d3dxBottomNormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
}

CCubeFaceNormal::~CCubeFaceNormal()
{

}

CCubeSlopeFace::CCubeSlopeFace()
{
	m_d3dxvUpNegative = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvUpPositive = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvDownNegative = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvDownPositive = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

CCubeSlopeFace::~CCubeSlopeFace()
{

}

CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	//정점 데이터를 저장하기 위한 정점 버퍼 인터페이스 포인터를 선언한다.
	m_ppd3dVertexBuffers = NULL;
	m_nVertices = 0;

	m_pd3dxvPositions = NULL;
	//정점의 위치 벡터를 저장하기 위한 버퍼에 대한 인터페이스 포인터이다. 
	m_pd3dPositionBuffer = NULL;

	//정점을 조립하기 위해 필요한 버퍼의 개수이다. 
	m_nBuffers = 0;
	//정점의 요소들을 나타내는 버퍼들의 원소의 바이트 수를 나타내는 배열이다. 
	m_pnVertexStrides = NULL;
	//정점의 요소들을 나타내는 버퍼들의 시작 위치(바이트 수)를 나타내는 배열이다. 
	m_pnVertexOffsets = NULL;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;

	m_pd3dRasterizerState = NULL;

	m_nReferences = 0;
}

CMesh::~CMesh()
{
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

	if (m_ppd3dVertexBuffers)
	{
		for (int i = 0; i < m_nBuffers; i++) m_ppd3dVertexBuffers[i]->Release();
	}
	if (m_pnVertexStrides) delete[] m_pnVertexStrides;
	if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
}

void CMesh::AddRef()
{
	m_nReferences++;
}

void CMesh::Release()
{
	m_nReferences--;
	if (m_nReferences <= 0) delete this;
}

void CMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{

}

void CMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
}

void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	// if (m_pd3dVertexBuffer) pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer, &m_nStride, &m_nOffset);
	pd3dDeviceContext->IASetVertexBuffers(0, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	//인덱스 버퍼가 있으면 인덱스 버퍼를 디바이스 컨텍스트에 연결한다.
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	//래스터라이저 상태를 디바이스 컨텍스트에 설정한다.
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);
	/*인덱스 버퍼가 있으면 인덱스 버퍼를 사용하여 메쉬를 렌더링하고 없으면 정점 버퍼만을 사용하여 메쉬를 렌더링한다.*/
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, 0);
}

CTexturedRectMesh::CTexturedRectMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight) : CMesh(pd3dDevice)
{
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, 0.0f);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, 0.0f);

	CTexturedVertex pVertices[4];
	int i = 0;
	//직육면체의 한 면에 텍스쳐 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.

	pVertices[0] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, 0.0f), D3DXVECTOR2(0.0f, 1.0f));
	pVertices[1] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[2] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[3] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, 0.0f), D3DXVECTOR2(1.0f, 0.0f));

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CTexturedVertex);
	m_pnVertexOffsets[0] = 0;

	m_nIndices = 4;
	UINT *pIndices = new UINT[4];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 3;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	delete[] pIndices;

	SetRasterizerState(pd3dDevice);
}

CTexturedRectMesh::~CTexturedRectMesh()
{

}

void CTexturedRectMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

CTexturedSpritePieceMesh::CTexturedSpritePieceMesh(ID3D11Device *pd3dDevice, float fPieceW, float fPieceH, int nPieceNum, int iExtractionIdx) : CMesh(pd3dDevice)
{
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fPieceW*0.5f, fy = fPieceH*0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, 0.0f);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, 0.0f);

	CTexturedVertex pVertices[4];
	int i = 0;
	//직육면체의 한 면에 텍스쳐 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.

	pVertices[0] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, 0.0f), D3DXVECTOR2((float)iExtractionIdx / (float)nPieceNum, 1.0f));
	pVertices[1] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, 0.0f), D3DXVECTOR2((float)iExtractionIdx / (float)nPieceNum, 0.0f));
	pVertices[2] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, 0.0f), D3DXVECTOR2((float)(iExtractionIdx + 1) / (float)nPieceNum, 1.0f));
	pVertices[3] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, 0.0f), D3DXVECTOR2((float)(iExtractionIdx + 1) / (float)nPieceNum, 0.0f));

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CTexturedVertex);
	m_pnVertexOffsets[0] = 0;

	m_nIndices = 4;
	UINT *pIndices = new UINT[4];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 3;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	delete[] pIndices;

	SetRasterizerState(pd3dDevice);
}

CTexturedSpritePieceMesh::~CTexturedSpritePieceMesh()
{

}

void CTexturedSpritePieceMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

CLightingMesh::CLightingMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_pd3dNormalBuffer = NULL;
}

CLightingMesh::~CLightingMesh()
{
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
}

void CLightingMesh::CalculateVertexNormal(D3DXVECTOR3 *pd3dxvNormals, D3DXVECTOR3 *pd3dxvVertices, UINT *piIndices)
{
	int nTrianglesNum = m_nIndices / 3;

	for (int i = 0; i < nTrianglesNum; ++i)
	{
		// i번째 삼각형의 색인들
		int idx0 = piIndices[i * 3 + 0];
		int idx1 = piIndices[i * 3 + 1];
		int idx2 = piIndices[i * 3 + 2];

		// i번째 삼각형의 정점들
		D3DXVECTOR3 v0 = pd3dxvVertices[idx0];
		D3DXVECTOR3 v1 = pd3dxvVertices[idx1];
		D3DXVECTOR3 v2 = pd3dxvVertices[idx2];

		// 면 법선을 계산한다.
		D3DXVECTOR3 e0 = v1 - v0;
		D3DXVECTOR3 e1 = v2 - v0;
		D3DXVECTOR3 Normal;
		D3DXVec3Cross(&Normal, &e0, &e1);

		// 이 삼각형의 세 정점을 다른 삼각형들도
		// 공유하므로, 면 법선을 각 정점의 법선에 누적한다.
		pd3dxvNormals[idx0] += Normal;
		pd3dxvNormals[idx1] += Normal;
		pd3dxvNormals[idx2] += Normal;
	}

	// 이제 각 정점 v의 normal 성분은 그 정점을 공유하는 모든
	// 삼각형의 면 법선이 누적된 결과이다. 이를 정규화하면
	// 결과적으로 그 법선들의 평균이 된다.
	for (int i = 0; i < m_nVertices; i++) D3DXVec3Normalize(&pd3dxvNormals[i], &pd3dxvNormals[i]);
}

CTexturedLightingCubeMesh::CTexturedLightingCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CLightingMesh(pd3dDevice)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, fz);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, -fz);

	CTexturedLightingVertex pVertices[36];
	int i = 0;
	//직육면체의 한 면에 텍스쳐 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.

#define TEXTURE_WIDTH		384.0f
#define TEXTURE_HEIGHT		128.0f


	// 윗 면
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f , 1.0f));

	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f / 3.0f, 0.0f));

	// 아랫 면
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(2.0f/3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(2.0f/3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));

	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(2.0f/3.0f, 0.0f));

	// 옆 면

	D3DXVECTOR3 p[6];
	D3DXVECTOR2 t[6];
	D3DXVECTOR3 d3dxvNormal;
	D3DXMATRIX d3dxmtxRotation;

	t[0] = D3DXVECTOR2(1.0f/3.0f, 0.0f);
	t[1] = D3DXVECTOR2(2.0f / 3.0f, 0.0f);
	t[2] = D3DXVECTOR2(1.0f / 3.0f, 1.0f);

	t[3] = D3DXVECTOR2(2.0f / 3.0f, 1.0f);
	t[4] = D3DXVECTOR2(1.0f / 3.0f, 1.0f);
	t[5] = D3DXVECTOR2(2.0f / 3.0f, 0.0f);

	for (int r = 0; r < 4; r++)
	{
		D3DXMatrixRotationAxis(&d3dxmtxRotation, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXToRadian(90.0f * (float)r));

		d3dxvNormal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		D3DXVec3TransformNormal(&d3dxvNormal, &d3dxvNormal, &d3dxmtxRotation);
		
		p[0] = D3DXVECTOR3(-fx, +fy, -fz);
		p[1] = D3DXVECTOR3(+fx, +fy, -fz);
		p[2] = D3DXVECTOR3(-fx, -fy, -fz);

		p[3] = D3DXVECTOR3(+fx, -fy, -fz);
		p[4] = D3DXVECTOR3(-fx, -fy, -fz);
		p[5] = D3DXVECTOR3(+fx, +fy, -fz);

		for (int w = 0; w < 6; w++)
		{
			D3DXVec3TransformNormal(&p[w], &p[w], &d3dxmtxRotation);
			pVertices[i++] = CTexturedLightingVertex(p[w], d3dxvNormal, D3DXVECTOR2(t[w].x, t[w].y));
		}
	}

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedLightingVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CTexturedLightingVertex);
	m_pnVertexOffsets[0] = 0;

	SetRasterizerState(pd3dDevice);
}

CTexturedLightingCubeMesh::~CTexturedLightingCubeMesh()
{
}

void CTexturedLightingCubeMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

//void CTexturedLightingCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
//{
//	CLightingMesh::Render(pd3dDeviceContext);
//}

CTexturedLightingUphillMesh::CTexturedLightingUphillMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, VOXEL_SLOPE_DIR eVoxelDir) : CLightingMesh(pd3dDevice)
{
	m_nVertices = 24;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, fz);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, -fz);

	CTexturedLightingVertex pVertices[24];
	int i = 0;
	//직육면체의 한 면에 텍스쳐 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.

#define TOP_TEX_COORD_U			0.0f
#define TOP_TEX_COORD_V			0.0f
#define SIDE_TEX_COORD_U		257.0f/512.0f
#define SIDE_TEX_COORD_V		0.0f
#define BOTTOM_TEX_COORD_U		0.0f
#define BOTTOM_TEX_COORD_V		257.0f/512.0f
#define HALF_TEX_COORD_U		0.5f
#define HALF_TEX_COORD_V		0.5f
#define MAX_TEX_COORD_U			1.0f
#define MAX_TEX_COORD_V			1.0f

	D3DXVECTOR3 d3dxvSlopeNormal = D3DXVECTOR3(0.0f, 1.0f, 1.0f);
	D3DXVec3Normalize(&d3dxvSlopeNormal, &d3dxvSlopeNormal);
	m_CubeFaceNormal.m_d3dxFrontNormal = m_CubeFaceNormal.m_d3dxTopNormal = d3dxvSlopeNormal;

	m_CubeSlopeFace.m_d3dxvUpNegative = D3DXVECTOR3(-fx, -fy, +fz);
	m_CubeSlopeFace.m_d3dxvUpPositive = D3DXVECTOR3(+fx, -fy, +fz);
	m_CubeSlopeFace.m_d3dxvDownNegative = D3DXVECTOR3(-fx, +fy, -fz);
	m_CubeSlopeFace.m_d3dxvDownPositive = D3DXVECTOR3(+fx, +fy, -fz);


	// 앞 상
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, -fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(1.0f/3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(0.0f, 1.0f));

	// 앞 하
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, +fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(0.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(1.0f / 3.0f, 0.0f));

	// 좌
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), m_CubeFaceNormal.m_d3dxLeftNormal, D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, +fz), m_CubeFaceNormal.m_d3dxLeftNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxLeftNormal, D3DXVECTOR2(2.0f / 3.0f, 0.0f));

	// 우
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), m_CubeFaceNormal.m_d3dxRightNormal, D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, -fz), m_CubeFaceNormal.m_d3dxRightNormal, D3DXVECTOR2(1.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxRightNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));

	// 뒤 하
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(1.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));

	// 뒤 상
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(2.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(1.0f / 3.0f, 0.0f));

	// 아래 앞
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(2.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(1.0f, 0.0f));

	// 아래 뒤
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, +fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(2.0f / 3.0f, 0.0f));

	D3DXMATRIX mtxRotate;

	D3DXMatrixRotationAxis(&mtxRotate, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToRadian(90.0f * (float) eVoxelDir));

	for (int i = 0; i < m_nVertices; i++)
	{
		D3DXVec3TransformNormal(&pVertices[i].m_d3dxvPosition, &pVertices[i].m_d3dxvPosition, &mtxRotate);
		D3DXVec3TransformNormal(&pVertices[i].m_d3dxvNormal, &pVertices[i].m_d3dxvNormal, &mtxRotate);
	}
	/*printf("m_CubeFaceNormal.m_d3dxRearNormal (%f, %f, %f) \n",
		m_CubeFaceNormal.m_d3dxRearNormal.x,
		m_CubeFaceNormal.m_d3dxRearNormal.y,
		m_CubeFaceNormal.m_d3dxRearNormal.z);*/

	D3DXVec3TransformNormal(&m_CubeFaceNormal.m_d3dxBottomNormal, &m_CubeFaceNormal.m_d3dxBottomNormal, &mtxRotate);
	D3DXVec3TransformNormal(&m_CubeFaceNormal.m_d3dxTopNormal, &m_CubeFaceNormal.m_d3dxTopNormal, &mtxRotate);
	D3DXVec3TransformNormal(&m_CubeFaceNormal.m_d3dxLeftNormal, &m_CubeFaceNormal.m_d3dxLeftNormal, &mtxRotate);
	D3DXVec3TransformNormal(&m_CubeFaceNormal.m_d3dxRightNormal, &m_CubeFaceNormal.m_d3dxRightNormal, &mtxRotate);
	D3DXVec3TransformNormal(&m_CubeFaceNormal.m_d3dxFrontNormal, &m_CubeFaceNormal.m_d3dxFrontNormal, &mtxRotate);
	D3DXVec3TransformNormal(&m_CubeFaceNormal.m_d3dxRearNormal, &m_CubeFaceNormal.m_d3dxRearNormal, &mtxRotate);
	/*printf("m_CubeFaceNormal.m_d3dxRearNormal (%f, %f, %f) \n",
		m_CubeFaceNormal.m_d3dxRearNormal.x,
		m_CubeFaceNormal.m_d3dxRearNormal.y,
		m_CubeFaceNormal.m_d3dxRearNormal.z);*/

	D3DXVec3TransformNormal(&m_CubeSlopeFace.m_d3dxvUpNegative, &m_CubeSlopeFace.m_d3dxvUpNegative, &mtxRotate);
	D3DXVec3TransformNormal(&m_CubeSlopeFace.m_d3dxvUpPositive, &m_CubeSlopeFace.m_d3dxvUpPositive, &mtxRotate);
	D3DXVec3TransformNormal(&m_CubeSlopeFace.m_d3dxvDownNegative, &m_CubeSlopeFace.m_d3dxvDownNegative, &mtxRotate);
	D3DXVec3TransformNormal(&m_CubeSlopeFace.m_d3dxvDownPositive, &m_CubeSlopeFace.m_d3dxvDownPositive, &mtxRotate);

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedLightingVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CTexturedLightingVertex);
	m_pnVertexOffsets[0] = 0;

	SetRasterizerState(pd3dDevice);
}

CTexturedLightingUphillMesh::~CTexturedLightingUphillMesh()
{

}

void CTexturedLightingUphillMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
}

CTexturedLightingGridMesh::CTexturedLightingGridMesh(
	ID3D11Device *pd3dDevice, 
	float fStartX, float fStartZ, 
	float fIntervalX, float fIntervalZ,
	int nWidth, int nLength) : CLightingMesh(pd3dDevice)
{
	m_nVertices = nWidth * nLength; //6* (nWidth-1)*(nLength-1);
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	CTexturedLightingVertex * pVertices = new CTexturedLightingVertex[m_nVertices];

	m_iWidth = nWidth;
	m_iLength = nLength;
	m_fStartX = fStartX;
	m_fStartZ = fStartZ;
	m_fIntervalX = fIntervalX;
	m_fIntervalZ = fIntervalZ;

	int iVNum = 0;
	for (int i = 0; i < m_iLength; i++)
	{
		for (int j = 0; j < m_iWidth; j++)
		{
			pVertices[iVNum] = CTexturedLightingVertex(
				D3DXVECTOR3(m_fStartX + (float)j * m_fIntervalX, 5.0f, m_fStartZ - (float)i * m_fIntervalZ),
				D3DXVECTOR3(0.0f, 1.0f, 0.0f), 
				D3DXVECTOR2(j, i));
			//printf("u v : %f %f \n", (float)j / (float)(m_iWidth - 1), (float)i / (float)(m_iLength - 1));
		//	printf("Pos[%d] %f %f %f \n", iVNum, pVertices[iVNum].m_d3dxvPosition.x, pVertices[iVNum ].m_d3dxvPosition.y, pVertices[iVNum ].m_d3dxvPosition.z);
			iVNum++;
		}
	}

	m_nBuffers = 1;

	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	//격자 메쉬의 높이를 변화시키기 위하여 정점 버퍼를 CPU에서 사용할 수 있도록 생성한다.
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedLightingVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);



	delete[] pVertices;

	m_pnVertexStrides[0] = sizeof(CTexturedLightingVertex);
	m_pnVertexOffsets[0] = 0;

	m_nIndices = 2 * m_iWidth * (m_iLength - 1) + 2 * (m_iLength - 2);
	UINT *pIndices = new UINT[m_nIndices];

	//printf("pIndices \n");
	for (int i = 0; i < m_iLength - 1; i++)
	{
		int j = 0;
		for (; j < m_iWidth; j++)
		{
			pIndices[2 * j		+ m_iWidth * i * 2		+ i * 2] = m_iWidth + m_iWidth * i + j;
			pIndices[2 * j + 1	+ m_iWidth * i * 2		+ i * 2] = 0 + m_iWidth * i + j;
			//printf("%d %d ", 2 * j + m_iWidth * i * 2 + i * 2, 2 * j + 1 + m_iWidth * i * 2 + i * 2);
		}
		
		if (m_iLength - 2 == i) break;
		pIndices[2 * j + m_iWidth * i * 2 + i * 2] = pIndices[2 * (j - 1) + m_iWidth * i * 2 + i * 2 + 1];
		pIndices[2 * j + 1 + m_iWidth * i * 2 + i * 2] = m_iWidth + m_iWidth * (i + 1);
	//	printf("%d %d ", pIndices[2 * j + m_iWidth * i * 2 + i * 2], pIndices[2 * j + 1 + m_iWidth * i * 2 + i * 2]);
	}

	//printf("pIndices : %d개 \n", m_nIndices);
	for (int i = 0; i < m_nIndices; i++)
	{
	//	printf("%d ", pIndices[i]);
	}
	//printf("\n");
	//m_nIndices = 4;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	delete[] pIndices;

	SetRasterizerState(pd3dDevice);
}

CTexturedLightingGridMesh::~CTexturedLightingGridMesh()
{

}

void CTexturedLightingGridMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CTexturedLightingGridMesh::Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed)
{
	static float fAngleDistance = float(2.0f * D3DX_PI);
	static float fAngleDistance2 = float(2.0f * D3DX_PI);
	static float fUpdatingTime = 0.0f;
	static float fFlowCoefficient = 0.0f;
	static float fDynamicValue = 1.0f + (float)(rand() % 10000) / 10000.0f;
	static float fDynamicValue2 = 1.0f + (float)(rand() % 10000) / 10000.0f;

	fUpdatingTime += fTimeElapsed;

	//if (fUpdatingTime > 0.3f)
	//{
		
		//정점 버퍼를 매핑하여 정점 버퍼 포인터를 가져온다.
		D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
		pd3dDeviceContext->Map(m_ppd3dVertexBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		CTexturedLightingVertex *pVertices = (CTexturedLightingVertex *)d3dMappedResource.pData;


		fAngleDistance -= fDynamicValue * fTimeElapsed;
		if (fAngleDistance < 0.0f)
		{
			fAngleDistance += float(2.0f * D3DX_PI);
			fDynamicValue = 1.0f + (float)(rand() % 10000) / 10000.0f;
		}

		fAngleDistance2 -= fDynamicValue2 * fTimeElapsed;
		if (fAngleDistance2 < 0.0f)
		{
			fAngleDistance2 += float(2.0f * D3DX_PI);
			fDynamicValue2 = 1.0f + (float)(rand() % 10000) / 10000.0f;
		}
		fFlowCoefficient += 1.0f * fTimeElapsed;
		if (fFlowCoefficient > float(2.0f * D3DX_PI)) fFlowCoefficient -= float(2.0f * D3DX_PI);
		int iVNum = 0;

		for (int i = 0; i < m_iLength; i++)
		{
			for (int j = 0; j < m_iWidth; j++)
			{
				pVertices[iVNum++] = CTexturedLightingVertex(
					D3DXVECTOR3(m_fStartX + (float)j * m_fIntervalX, j % 2 == 0 && i % 2 == 0 ? -sin(fAngleDistance2+(i+j)) * 0.15f : sin(fAngleDistance + (i+j)) * 0.15f + 0.5f, m_fStartZ - (float)i * m_fIntervalZ),
					D3DXVECTOR3(0.0f, 1.0f, 0.0f),
					D3DXVECTOR2(j / 4.0f -  0.015f*cos(fFlowCoefficient), i / 4.0f - 0.015f * sin(fAngleDistance)));
				//printf("u v : %f %f \n", (float)j / (float)(m_iWidth - 1), (float)i / (float)(m_iLength - 1));
				//printf("Pos[%d] %f %f %f \n", iVNum, pVertices[iVNum].m_d3dxvPosition.x, pVertices[iVNum].m_d3dxvPosition.y, pVertices[iVNum].m_d3dxvPosition.z);
				//iVNum++;
			}
		}

		pd3dDeviceContext->Unmap(m_ppd3dVertexBuffers[0], 0);
	//	fUpdatingTime = 0.0f;
	//}
}


CMesh* CMeshResource::pWaveMesh = NULL;
CMesh* CMeshResource::pPirateMesh = NULL;
CMesh* CMeshResource::pSnowWomanMesh = NULL;
CMesh** CMeshResource::ppRegisteredVoxelMesh = NULL;
CMesh* CMeshResource::pStandardVoxelMesh = NULL;
CMesh* CMeshResource::pUphill000Mesh = NULL;
CMesh* CMeshResource::pUphill090Mesh = NULL;
CMesh* CMeshResource::pUphill180Mesh = NULL;
CMesh* CMeshResource::pUphill270Mesh = NULL;
CMesh* CMeshResource::pUI_VoxelPocketMesh = NULL;

CMeshResource::CMeshResource()
{

}

CMeshResource::~CMeshResource()
{

}

void CMeshResource::CreateMeshResource(ID3D11Device *pd3dDevice)
{
	pPirateMesh = new CTexturedLightingCubeMesh(pd3dDevice, 0.6f, 1.0f, 0.6f);
	pWaveMesh = new CTexturedLightingGridMesh(pd3dDevice, -160.0f, 160.0f, 3.2f,3.2f,100, 100);

	pStandardVoxelMesh = new CTexturedLightingCubeMesh(pd3dDevice, 1.0f, 1.0f, 1.0f);
	pUphill000Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_FRONT);		// 앞
	pUphill090Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_RIGHT);		// 우
	pUphill180Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_REAR);		// 뒤
	pUphill270Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_LEFT);		// 좌

	pUI_VoxelPocketMesh = new CTexturedRectMesh(pd3dDevice, 5, 1);
	
	ppRegisteredVoxelMesh = new CMesh*[7];
	ppRegisteredVoxelMesh[0] = pStandardVoxelMesh;
	ppRegisteredVoxelMesh[1] = pStandardVoxelMesh;
	ppRegisteredVoxelMesh[2] = pUphill000Mesh;
	ppRegisteredVoxelMesh[3] = pUphill090Mesh;
	ppRegisteredVoxelMesh[4] = pUphill180Mesh;
	ppRegisteredVoxelMesh[5] = pUphill270Mesh;
	ppRegisteredVoxelMesh[6] = pStandardVoxelMesh;
}