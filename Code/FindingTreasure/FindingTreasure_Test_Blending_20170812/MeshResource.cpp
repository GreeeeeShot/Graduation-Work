#include "stdafx.h"
#include "VertexResource.h"
#include "ShaderResource.h"
#include "MeshResource.h"
#include "GameFramework.h"
//#include "FBXAnim.h"

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
		for (int i = 0; i < m_nBuffers; i++)  m_ppd3dVertexBuffers[i]->Release();
	}
	delete []m_ppd3dVertexBuffers;
	if (m_AABB.m_pRenderObject) delete m_AABB.m_pRenderObject;
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

void CMesh::InstRender(ID3D11DeviceContext *pd3dDeviceContext, ID3D11Buffer* pd3dbInstBuf, UINT iInstSize, UINT iInstNum, UINT iInstStartIdx)
{
	int nBuffers = m_nBuffers + 1;		// 하나의 인스턴스 버퍼가 추가됨.
	ID3D11Buffer** ppd3dVertexBuffers = new ID3D11Buffer*[nBuffers];
	UINT* pnVertexStrides = new UINT[nBuffers];
	UINT* pnVertexOffsets = new UINT[nBuffers];

	for (int i = 0; i < m_nBuffers; i++)
	{
		ppd3dVertexBuffers[i] = m_ppd3dVertexBuffers[i];
		pnVertexStrides[i] = m_pnVertexStrides[i];
		pnVertexOffsets[i] = m_pnVertexOffsets[i];
	}

	ppd3dVertexBuffers[m_nBuffers] = pd3dbInstBuf;
	pnVertexStrides[m_nBuffers] = iInstSize;
	pnVertexOffsets[m_nBuffers] = 0;
	
	pd3dDeviceContext->IASetVertexBuffers(0, nBuffers, ppd3dVertexBuffers, pnVertexStrides, pnVertexOffsets);
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	//객체들의 인스턴스들을 렌더링한다. 
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, iInstNum, m_nStartIndex, m_nBaseVertex, iInstStartIdx);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, iInstNum, 0, iInstStartIdx);

	delete[] ppd3dVertexBuffers;
	delete[]pnVertexStrides;
	delete[]pnVertexOffsets;
}

CDiffusedMesh::CDiffusedMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{

}

CDiffusedMesh::~CDiffusedMesh()
{

}

CDiffusedCubeMesh::CDiffusedCubeMesh(
	ID3D11Device *pd3dDevice,
	float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxvColor) : CDiffusedMesh(pd3dDevice)
{
	m_nVertices = 24;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	float fx = fWidth*0.5f, fy = fHeight * 0.5f, fz = fDepth*0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, fz);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, -fz);
	/*m_AABB.m_pRenderObject = new CGameObject();
	m_AABB.m_pRenderObject->SetShader(CShaderResource::pDiffusedShader);
	m_AABB.m_pRenderObject->SetMesh(this);*/

	CDiffusedVertex pVertices[24];

	// 세로 모서리
	//D3DXCOLOR d3dxcPurple = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);

	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(-fx, fy, -fz), d3dxvColor);
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, -fz), d3dxvColor);
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(fx, fy, -fz), d3dxvColor);
	pVertices[3] = CDiffusedVertex(D3DXVECTOR3(fx, -fy, -fz), d3dxvColor);
	pVertices[4] = CDiffusedVertex(D3DXVECTOR3(fx, fy, fz), d3dxvColor);
	pVertices[5] = CDiffusedVertex(D3DXVECTOR3(fx, -fy, fz), d3dxvColor);
	pVertices[6] = CDiffusedVertex(D3DXVECTOR3(-fx, fy, fz), d3dxvColor);
	pVertices[7] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, fz), d3dxvColor);

	// 가로 모서리
	pVertices[8] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, -fz), d3dxvColor);
	pVertices[9] = CDiffusedVertex(D3DXVECTOR3(fx, -fy, -fz), d3dxvColor);
	pVertices[10] = CDiffusedVertex(D3DXVECTOR3(-fx,fy, -fz), d3dxvColor);
	pVertices[11] = CDiffusedVertex(D3DXVECTOR3(fx, fy, -fz), d3dxvColor);
	pVertices[12] = CDiffusedVertex(D3DXVECTOR3(-fx, fy, fz), d3dxvColor);
	pVertices[13] = CDiffusedVertex(D3DXVECTOR3(fx, fy, fz), d3dxvColor);
	pVertices[14] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, fz), d3dxvColor);
	pVertices[15] = CDiffusedVertex(D3DXVECTOR3(fx, -fy, fz), d3dxvColor);

	// 깊이 모서리
	pVertices[16] = CDiffusedVertex(D3DXVECTOR3(-fx, fy, -fz), d3dxvColor);
	pVertices[17] = CDiffusedVertex(D3DXVECTOR3(-fx, fy, fz), d3dxvColor);
	pVertices[18] = CDiffusedVertex(D3DXVECTOR3(fx, fy , -fz), d3dxvColor);
	pVertices[19] = CDiffusedVertex(D3DXVECTOR3(fx, fy, fz), d3dxvColor);
	pVertices[20] = CDiffusedVertex(D3DXVECTOR3(fx, -fy, -fz), d3dxvColor);
	pVertices[21] = CDiffusedVertex(D3DXVECTOR3(fx, -fy, fz), d3dxvColor);
	pVertices[22] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, -fz), d3dxvColor);
	pVertices[23] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, fz), d3dxvColor);

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CDiffusedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CDiffusedVertex);
	m_pnVertexOffsets[0] = 0;

	SetRasterizerState(pd3dDevice);
}

CDiffusedCubeMesh::~CDiffusedCubeMesh()
{

}

CSphereMesh::CSphereMesh(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks, D3DXCOLOR d3dxColor) : CMesh(pd3dDevice)
{
	/*사각형의 개수는 (nSlices * nStacks)이고 사각형은 2개의 삼각형으로 구성되므로 삼각형 리스트일 때 정점의 개수는  ((nSlices * nStacks) * 3 * 2)개이다.*/
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex *pVertices = new CDiffusedVertex[m_nVertices];
	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		//180도를 nStacks 만큼 분할한다. 
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		//180도를 nStacks 만큼 분할하여 y 좌표와 원기둥의 반지름을 구한다.
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius*cosf(phi_j);
		y_jj = fRadius*cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			//360도를 nSlices 만큼 분할하고 <그림 3>과 같이 삼각형들의 리스트를 만든다.
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			//사각형의 첫 번째 삼각형의 정점을 만든다.
			pVertices[k++] = CDiffusedVertex(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i), d3dxColor);
			pVertices[k++] = CDiffusedVertex(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i), d3dxColor);
			pVertices[k++] = CDiffusedVertex(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii), d3dxColor);
			//사각형의 두 번째 삼각형의 정점을 만든다.
			pVertices[k++] = CDiffusedVertex(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i), d3dxColor);
			pVertices[k++] = CDiffusedVertex(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii), d3dxColor);
			pVertices[k++] = CDiffusedVertex(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii), d3dxColor);
		}
	}

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CDiffusedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CDiffusedVertex);
	m_pnVertexOffsets[0] = 0;

	delete[] pVertices;

	CreateRasterizerState(pd3dDevice);
}

CSphereMesh::~CSphereMesh()
{
}

void CSphereMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

CTexturedRectMesh::CTexturedRectMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, D3DXVECTOR2 d3dxvUV[4]) : CMesh(pd3dDevice)
{
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, 0.0f);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, 0.0f);
	/*m_AABB.m_pRenderObject = new CGameObject();
	m_AABB.m_pRenderObject->SetShader(CShaderResource::pDiffusedShader);
	m_AABB.m_pRenderObject->SetMesh(CMeshResource::pAABBMesh);*/

	m_d3dxvUV[0] = d3dxvUV[0];
	m_d3dxvUV[1] = d3dxvUV[1];
	m_d3dxvUV[2] = d3dxvUV[2];
	m_d3dxvUV[3] = d3dxvUV[3];

	CTexturedVertex pVertices[4];
	int i = 0;
	//직육면체의 한 면에 텍스쳐 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.

	pVertices[0] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, 0.0f), d3dxvUV[0]);
	pVertices[1] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, 0.0f), d3dxvUV[1]);
	pVertices[2] = CTexturedVertex(D3DXVECTOR3(+fx , -fy, 0.0f), d3dxvUV[2]);
	pVertices[3] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, 0.0f), d3dxvUV[3]);

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

void CTexturedRectMesh::Animate(ID3D11DeviceContext *pd3dDeviceContext, float fInterpRate)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_ppd3dVertexBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	CTexturedVertex *pVertices = (CTexturedVertex *)d3dMappedResource.pData;

	pVertices[0] = CTexturedVertex(D3DXVECTOR3(m_AABB.m_d3dxvMin.x, m_AABB.m_d3dxvMax.y, 0.0f), m_d3dxvUV[0]);
	pVertices[1] = CTexturedVertex(D3DXVECTOR3(m_AABB.m_d3dxvMin.x, m_AABB.m_d3dxvMin.y, 0.0f), m_d3dxvUV[1]);
	pVertices[2] = CTexturedVertex(D3DXVECTOR3(m_AABB.m_d3dxvMin.x * (1.0f - fInterpRate) + m_AABB.m_d3dxvMax.x * fInterpRate, m_AABB.m_d3dxvMax.y, 0.0f), m_d3dxvUV[2]);
	pVertices[3] = CTexturedVertex(D3DXVECTOR3(m_AABB.m_d3dxvMin.x * (1.0f - fInterpRate) + m_AABB.m_d3dxvMax.x * fInterpRate, m_AABB.m_d3dxvMin.y, 0.0f), m_d3dxvUV[3]);

	pd3dDeviceContext->Unmap(m_ppd3dVertexBuffers[0], 0);
}

CTexturedSpritePieceMesh::CTexturedSpritePieceMesh(ID3D11Device *pd3dDevice, float fPieceW, float fPieceH, int nPieceNum, int iExtractionIdx) : CMesh(pd3dDevice)
{
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fPieceW*0.5f, fy = fPieceH*0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, 0.0f);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, 0.0f);
	//m_AABB.m_pRenderObject = new CGameObject();
	//m_AABB.m_pRenderObject->SetShader(CShaderResource::pDiffusedShader);
	//m_AABB.m_pRenderObject->SetMesh(CMeshResource::pAABBMesh);

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
	if (m_pd3dNormalBuffer) for (int j = 0; j < 100; j++) m_pd3dNormalBuffer->Release();
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
	//m_AABB.m_pRenderObject = new CGameObject();
	////printf("pDiffused : %x\n", CShaderResource::pDiffusedShader);
	//m_AABB.m_pRenderObject->SetShader(CShaderResource::pDiffusedShader);
	//m_AABB.m_pRenderObject->SetMesh(CMeshResource::pAABBMesh);

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
		
		if (r % 2 == 0)
		{
			p[0] = D3DXVECTOR3(-fx, +fy, -fz);
			p[1] = D3DXVECTOR3(+fx, +fy, -fz);
			p[2] = D3DXVECTOR3(-fx, -fy, -fz);

			p[3] = D3DXVECTOR3(+fx, -fy, -fz);
			p[4] = D3DXVECTOR3(-fx, -fy, -fz);
			p[5] = D3DXVECTOR3(+fx, +fy, -fz);
		}
		else
		{
			p[0] = D3DXVECTOR3(-fz, +fy, -fx);
			p[1] = D3DXVECTOR3(+fz, +fy, -fx);
			p[2] = D3DXVECTOR3(-fz, -fy, -fx);

			p[3] = D3DXVECTOR3(+fz, -fy, -fx);
			p[4] = D3DXVECTOR3(-fz, -fy, -fx);
			p[5] = D3DXVECTOR3(+fz, +fy, -fx);
		}

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
	d3dRasterizerDesc.DepthClipEnable = true;
	d3dRasterizerDesc.DepthBias = 10000;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 1.0f;
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
	/*m_AABB.m_pRenderObject = new CGameObject();
	m_AABB.m_pRenderObject->SetShader(CShaderResource::pDiffusedShader);
	m_AABB.m_pRenderObject->SetMesh(CMeshResource::pAABBMesh);*/

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

	CNormalMappingVertex * pVertices = new CNormalMappingVertex[m_nVertices];

	m_iWidth = nWidth;
	m_iLength = nLength;
	m_fStartX = fStartX;
	m_fStartZ = fStartZ;
	m_fIntervalX = fIntervalX;
	m_fIntervalZ = fIntervalZ;

	m_pfHeightMap = new float[m_iWidth * m_iLength];
	ZeroMemory(m_pfHeightMap, sizeof(float) * m_iWidth * m_iLength);

	int iVNum = 0;
	for (int i = 0; i < m_iLength; i++)
	{
		for (int j = 0; j < m_iWidth; j++)
		{
			pVertices[iVNum] = CNormalMappingVertex(
				D3DXVECTOR3(m_fStartX + (float)j * m_fIntervalX, 5.0f, m_fStartZ - (float)i * m_fIntervalZ),
				D3DXVECTOR3(0.0f, 1.0f, 0.0f), 
				D3DXVECTOR3(j, i, 0.0f),
				D3DXVECTOR3(1.0f, 0.0f, 0.0f));
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
	d3dBufferDesc.ByteWidth = sizeof(CNormalMappingVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);



	delete[] pVertices;

	m_pnVertexStrides[0] = sizeof(CNormalMappingVertex);
	m_pnVertexOffsets[0] = 0;

	m_nIndices = 2 * m_iWidth * (m_iLength - 1) + 2 * (m_iLength - 2);
	UINT *pIndices = new UINT[m_nIndices];

	for (int i = 0; i < m_iLength - 1; i++)
	{
		int j = 0;
		for (; j < m_iWidth; j++)
		{
			pIndices[2 * j		+ m_iWidth * i * 2		+ i * 2] = m_iWidth + m_iWidth * i + j;
			pIndices[2 * j + 1	+ m_iWidth * i * 2		+ i * 2] = 0 + m_iWidth * i + j;
		}
		
		if (m_iLength - 2 == i) break;
		pIndices[2 * j		+ m_iWidth * i * 2		+ i * 2] = pIndices[2 * (j - 1) + m_iWidth * i * 2 + i * 2 + 1];
		pIndices[2 * j + 1	+ m_iWidth * i * 2		+ i * 2] = m_iWidth + m_iWidth * (i + 1);
	}

	//printf("pIndices : %d개 \n", m_nIndices);
	//for (int i = 0; i < m_nIndices; i++)
	//{
	////	printf("%d ", pIndices[i]);
	//}
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
	if (m_pfHeightMap) delete[]m_pfHeightMap;
}

void CTexturedLightingGridMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthBias = 10000;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 1.0f;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

float CTexturedLightingGridMesh::GetHeight(float fWorldX, float fWorldZ)
{
	float fGridMapW = m_fIntervalX * (m_iWidth - 1);				// 그리드맵의 X 길이
	float fGridMapH = m_fIntervalZ * (m_iLength - 1);				// 그리드맵의 Z 길이 ; 이때 배열 좌표계와 D3D 좌표계의 Z 축은 서로 반대임에 유의!

	float fU = (fWorldX - m_fStartX) / fGridMapW;			// 정규화된 높이맵 배열의 U축에 대한 기준 idx값
	float fV = (m_fStartZ - fWorldZ) / fGridMapH;			// 정규화된 높이맵 배열의 V축에 대한 기준 idx값

	if (fU < 0.0f || fU > 1.0f) return -1.0f;				// 범위를 벗어난 값이면 -1.0f를 반환하고 메서드 종료.
	if (fV < 0.0f || fV > 1.0f) return -1.0f;				// 동일

	fU *= (float)(m_iWidth - 1);									// 높이맵의 U축에 대한 기준 idx값(소수 O)을 얻는다.
	fV *= (float)(m_iLength - 1);									// 높이맵의 V축에 대한 기준 idx값(소수 O)을 얻는다.

	int iArrUIndex = floor(fU);								// 높이맵의 U축에 대한 기준 idx값(소수 X)을 얻는다.
	int iArrVIndex = floor(fV);								// 높이맵의 V축에 대한 기준 idx값(소수 X)을 얻는다.

	fU -= (float)iArrUIndex;											// 하나의 사각형 내 U축에 대한 보간 값
	fV -= (float)iArrVIndex;											// 하나의 사각형 내 V축에 대한 보간 값

	float fHeight[3];

	if (fU > fV)			// 사각형 내 오른쪽 삼각형일 때
	{
		fHeight[0] = m_pfHeightMap[m_iWidth * iArrVIndex + (iArrUIndex + 1)];
		fHeight[1] = m_pfHeightMap[m_iWidth * iArrVIndex + iArrUIndex];					// U
		fHeight[2] = m_pfHeightMap[m_iWidth * (iArrVIndex + 1) + (iArrUIndex + 1)];		// V
		
		fU = 1.0f - fU;
	}
	else					// 사각형 내 왼쪽 삼각형일 때
	{
		fHeight[0] = m_pfHeightMap[m_iWidth * (iArrVIndex + 1) + iArrUIndex];
		fHeight[1] = m_pfHeightMap[m_iWidth * iArrVIndex + iArrUIndex];					//U
		fHeight[2] = m_pfHeightMap[m_iWidth * (iArrVIndex + 1) + (iArrUIndex + 1)];		//V

		fV = 1.0f - fV;
	}

	// return (fHeight[0] * (1.0f - fU) + fHeight[1] * fU) + (fHeight[0] * (1.0f - fV) + fHeight[2] * fV);
	return fHeight[0] + fU * (fHeight[1] - fHeight[0]) + fV * (fHeight[2] - fHeight[0]);
}

void CTexturedLightingGridMesh::Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed)
{
	static float fDynamicRadian = float(2.0f * D3DX_PI);
	static float fDynamicRadian2 = float(2.0f * D3DX_PI);
	static float fUpdatingTime = 0.0f;
	static float fFlowCoefficient = 0.0f;
	static float fDynamicValue = 0.3f + (float)(rand() % 10000) / 12000.0f;
	static float fDynamicValue2 = 0.3f + (float)(rand() % 10000) / 12000.0f;

	fUpdatingTime += fTimeElapsed;

	//if (fUpdatingTime > 0.3f)
	//{
		
		//정점 버퍼를 매핑하여 정점 버퍼 포인터를 가져온다.
		D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
		pd3dDeviceContext->Map(m_ppd3dVertexBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		CNormalMappingVertex *pVertices = (CNormalMappingVertex *)d3dMappedResource.pData;


		fDynamicRadian -= fDynamicValue * fTimeElapsed;
		if (fDynamicRadian < 0.0f)
		{
			fDynamicRadian += float(2.0f * D3DX_PI);
			fDynamicValue = 1.0f + (float)(rand() % 10000) / 10000.0f;
		}

		fDynamicRadian2 -= fDynamicValue2 * fTimeElapsed;
		if (fDynamicRadian2 < 0.0f)
		{
			fDynamicRadian2 += float(2.0f * D3DX_PI);
			fDynamicValue2 = 1.0f + (float)(rand() % 10000) / 10000.0f;
		}
		fFlowCoefficient += 1.0f * fTimeElapsed;
		if (fFlowCoefficient > float(2.0f * D3DX_PI)) fFlowCoefficient -= float(2.0f * D3DX_PI);
		int iVNum = 0;

		float fDerI;
		float fDerJ;
		D3DXVECTOR3 fNorm;

		for (int i = 0; i < m_iLength; i++)
		{
			for (int j = 0; j < m_iWidth; j++)
			{
				fDerI = j % 2 == 0 && i % 2 == 0 ? -cos(fDynamicRadian2 + (i + j)) * 0.25f : cos(fDynamicRadian + (i + j)) * 0.25f;
				fDerJ = j % 2 == 0 && i % 2 == 0 ? -cos(fDynamicRadian2 + (i + j)) * 0.25f : cos(fDynamicRadian + (i + j)) * 0.25f;
				fNorm = D3DXVECTOR3(-fDerI, 1, -fDerJ);

				D3DXVec3Normalize(&fNorm, &fNorm);
				
				pVertices[iVNum++] = CNormalMappingVertex(
					D3DXVECTOR3(m_fStartX + (float)j * m_fIntervalX, 
						m_pfHeightMap[m_iWidth * i + j] = (j % 2 == 0 && i % 2 == 0 ? -sin(fDynamicRadian2+(i+j)) * 0.25f : sin(fDynamicRadian + (i+j)) * 0.25f + 0.5f), 
						m_fStartZ - (float)i * m_fIntervalZ),
					fNorm,
					D3DXVECTOR3(j / 4.0f -  0.010f*cos(fFlowCoefficient), i / 4.0f - 0.010f * sin(fDynamicRadian), fUpdatingTime),
					D3DXVECTOR3(1.0f, 0.0f, 0.0f));
			}
		}

		pd3dDeviceContext->Unmap(m_ppd3dVertexBuffers[0], 0);
	//	fUpdatingTime = 0.0f;
	//}
}

CTexturedLightingPirateMesh::CTexturedLightingPirateMesh(ID3D11Device *pd3dDevice) : CLightingMesh(pd3dDevice)
{
	float fx = 0.2f, fy = 0.45f, fz = 0.2f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, fz);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, -fz);
	/*m_AABB.m_pRenderObject = new CGameObject();
	m_AABB.m_pRenderObject->SetShader(CShaderResource::pDiffusedShader);
	CMesh* pAABBMesh = new CDiffusedCubeMesh(pd3dDevice, 0.4f, 0.9f, 0.4f, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
	m_AABB.m_pRenderObject->SetMesh(pAABBMesh);*/

	m_pFBXAnim = CFBXAnimResource::m_pPirateFBXAnim;//new CPirateFBXAnim();
	
	D3DXVECTOR3* vec = NULL;
	D3DXVECTOR3* nor = NULL;
	D3DXVECTOR2* uv = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vec = m_pFBXAnim->m_pd3dxvVertexList;
	nor = m_pFBXAnim->m_pd3dxvNormalList;
	uv = m_pFBXAnim->m_pd3dxvUVList;

	m_nVertices = m_pFBXAnim->m_iVertexNum;

	m_pFBXAnim->SetFBXAnimForType(PIRATE_ANIM_TYPE_IDLE);

	CTexturedLightingVertex* pVertices = new CTexturedLightingVertex[m_nVertices];

	for (int i = 0; i < m_nVertices; ++i)
	{
		pVertices[i] = CTexturedLightingVertex(D3DXVECTOR3(vec[i].x*0.01, vec[i].y*0.01, vec[i].z*0.01 - 0.2f), nor[i], uv[i]);
	}

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

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

	m_pnVertexStrides[0] = sizeof(CTexturedLightingVertex);
	m_pnVertexOffsets[0] = 0;

	SetRasterizerState(pd3dDevice);
}

CTexturedLightingPirateMesh::~CTexturedLightingPirateMesh()
{
}

void CTexturedLightingPirateMesh::Animate(ID3D11DeviceContext *pd3dDeviceContext, float t)
{
	//정점 버퍼를 매핑하여 정점 버퍼 포인터를 가져온다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_ppd3dVertexBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	CTexturedLightingVertex* Vertices = (CTexturedLightingVertex *)d3dMappedResource.pData;

	D3DXVECTOR3* vec = NULL;
	D3DXVECTOR3* nor = NULL;
	D3DXVECTOR2* uv = NULL;

	nor = m_pFBXAnim->m_pd3dxvNormalList;
	uv = m_pFBXAnim->m_pd3dxvUVList;
	m_pFBXAnim->m_pSetFBXAnim->AnimationToVertex(&vec, (int)t);
	
	//if (CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_action == IDLE)
	//{
	//	

	//	m_AniIdle->AnimationToVertex(&vec, (int)t);
	//}
	//else if (CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_action == WALK)
	//{
	//	/*nor = m_AniWalk->CopyNormalVertex();
	//	uv = m_AniWalk->CopyUVVertex();*/

	//	m_AniWalk->AnimationToVertex(&vec, (int)t);
	//}
	//else if (CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_action == ATK)
	//{
	//	/*nor = m_AniAtk->CopyNormalVertex();
	//	uv = m_AniAtk->CopyUVVertex();*/

	//	m_AniAtk->AnimationToVertex(&vec, (int)t);

	//	if (t > m_AniAtk->GetAnimationnum()-1)
	//		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_action = IDLE;

	//}
	//else if (CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_action == MAKEAHOLE)
	//{
	//	/*nor = m_AniMakeAHole->CopyNormalVertex();
	//	uv = m_AniMakeAHole->CopyUVVertex();*/

	//	m_AniMakeAHole->AnimationToVertex(&vec, (int)t);
	//}
	//else if (CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_action == HIT)
	//{
	///*	nor = m_AniHit->CopyNormalVertex();
	//	uv = m_AniHit->CopyUVVertex();*/

	//	m_AniHit->AnimationToVertex(&vec, (int)t);
	//}


	for (int i = 0; i < m_nVertices; ++i)
	{
		Vertices[i] = CTexturedLightingVertex(D3DXVECTOR3(vec[i].x*0.01, vec[i].y*0.01, vec[i].z*0.01 - 0.2f), nor[i], uv[i]);
	}

	delete[] vec;
	/*delete[] nor;
	delete[] uv;*/

	pd3dDeviceContext->Unmap(m_ppd3dVertexBuffers[0], 0);
}

void CTexturedLightingPirateMesh::SetFBXAnimForType(int eAnimType)
{
	if(m_pFBXAnim) m_pFBXAnim->SetFBXAnimForType((PIRATE_ANIM_TYPE)eAnimType);
}

void CTexturedLightingPirateMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CTexturedLightingPirateMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CLightingMesh::Render(pd3dDeviceContext);
}

CTexturedLightingCowgirlMesh::CTexturedLightingCowgirlMesh(ID3D11Device *pd3dDevice) : CLightingMesh(pd3dDevice)
{
	float fx = 0.2f, fy = 0.45f, fz = 0.2f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, fz);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, -fz);
	m_AABB.m_pRenderObject = new CGameObject();
	m_AABB.m_pRenderObject->SetShader(CShaderResource::pDiffusedShader);
	CMesh* pAABBMesh = new CDiffusedCubeMesh(pd3dDevice, 0.4f, 0.9f, 0.4f, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
	m_AABB.m_pRenderObject->SetMesh(pAABBMesh);

	m_pFBXAnim = CFBXAnimResource::m_pCowgirlFBXAnim;//new CPirateFBXAnim();

	D3DXVECTOR3* vec = NULL;
	D3DXVECTOR3* nor = NULL;
	D3DXVECTOR2* uv = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vec = m_pFBXAnim->m_pd3dxvVertexList;
	nor = m_pFBXAnim->m_pd3dxvNormalList;
	uv = m_pFBXAnim->m_pd3dxvUVList;

	m_nVertices = m_pFBXAnim->m_iVertexNum;

	m_pFBXAnim->SetFBXAnimForType(COWGIRL_ANIM_TYPE_IDLE);

	CTexturedLightingVertex* pVertices = new CTexturedLightingVertex[m_nVertices];

	for (int i = 0; i < m_nVertices; ++i)
	{
		pVertices[i] = CTexturedLightingVertex(D3DXVECTOR3(vec[i].x*0.01, vec[i].y*0.01, vec[i].z*0.01 - 0.2f), nor[i], uv[i]);
	}

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

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

	m_pnVertexStrides[0] = sizeof(CTexturedLightingVertex);
	m_pnVertexOffsets[0] = 0;

	SetRasterizerState(pd3dDevice);
}

CTexturedLightingCowgirlMesh::~CTexturedLightingCowgirlMesh()
{
}

void CTexturedLightingCowgirlMesh::Animate(ID3D11DeviceContext *pd3dDeviceContext, float t)
{
	//정점 버퍼를 매핑하여 정점 버퍼 포인터를 가져온다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_ppd3dVertexBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	CTexturedLightingVertex* Vertices = (CTexturedLightingVertex *)d3dMappedResource.pData;

	D3DXVECTOR3* vec = NULL;
	D3DXVECTOR3* nor = NULL;
	D3DXVECTOR2* uv = NULL;

	nor = m_pFBXAnim->m_pd3dxvNormalList;
	uv = m_pFBXAnim->m_pd3dxvUVList;
	m_pFBXAnim->m_pSetFBXAnim->AnimationToVertex(&vec, (int)t);


	for (int i = 0; i < m_nVertices; ++i)
	{
		Vertices[i] = CTexturedLightingVertex(D3DXVECTOR3(vec[i].x*0.001, vec[i].z*0.003 , -vec[i].y*0.001 ), nor[i], uv[i]);
	}

	delete[] vec;

	pd3dDeviceContext->Unmap(m_ppd3dVertexBuffers[0], 0);
}

void CTexturedLightingCowgirlMesh::SetFBXAnimForType(int eAnimType)
{
	if (m_pFBXAnim) m_pFBXAnim->SetFBXAnimForType((COWGIRL_ANIM_TYPE)eAnimType);
}

void CTexturedLightingCowgirlMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CTexturedLightingCowgirlMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CLightingMesh::Render(pd3dDeviceContext);
}


SkydomMesh::SkydomMesh(ID3D11Device *pd3dDevice) : CLightingMesh(pd3dDevice)
{
	Skydom = new FBXExporter();
	Skydom->Initialize();
	Skydom->LoadScene("Skydom.FBX");
	Skydom->ImportFBX();

	D3DXVECTOR3* vec = NULL;
	D3DXVECTOR3* nor = NULL;
	D3DXVECTOR2* uv = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vec = Skydom->CopyMeshVertex(&m_nVertices);
	nor = Skydom->CopyNormalVertex();
	uv = Skydom->CopyUVVertex();

	CTexturedVertex* pVertices = new CTexturedVertex[m_nVertices];
	
	D3DXVECTOR3 d3dxVertex;
	D3DXMATRIX mtxRotate;

	printf("m_nVertices : %d", m_nVertices);
	for (int i = 0; i < m_nVertices; ++i)
	{
		d3dxVertex = D3DXVECTOR3(vec[i].x*10.f, vec[i].y*10.f, vec[i].z*10.f);
		D3DXMatrixRotationX(&mtxRotate, D3DXToRadian(90.0f));
		D3DXVec3TransformNormal(&d3dxVertex, &d3dxVertex, &mtxRotate);
		pVertices[i] = CTexturedVertex(d3dxVertex, uv[i]);
	}

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CTexturedVertex);
	m_pnVertexOffsets[0] = 0;

	delete[] vec;
	delete[] nor;
	delete[] uv;

	SetRasterizerState(pd3dDevice);
}

SkydomMesh::~SkydomMesh()
{
	if (Skydom) delete Skydom;
}

void SkydomMesh::Animate(ID3D11DeviceContext *pd3dDeviceContext, float t)
{
	
}

void SkydomMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_FRONT;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void SkydomMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CLightingMesh::Render(pd3dDeviceContext);
}

ShipMesh::ShipMesh(ID3D11Device *pd3dDevice) : CLightingMesh(pd3dDevice)
{
	float fx = 1.8f, fy = 1.8f, fz = 4.0f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, fz);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, -fz);
	m_AABB.m_pRenderObject = new CGameObject();
	m_AABB.m_pRenderObject->SetShader(CShaderResource::pDiffusedShader);
	CMesh* pAABBMesh = new CDiffusedCubeMesh(pd3dDevice, 1.8f, 1.f, 2.0f, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
	m_AABB.m_pRenderObject->SetMesh(pAABBMesh);

	Ship = new FBXExporter();
	Ship->Initialize();
	Ship->LoadScene("boatfbx.FBX");
	Ship->ImportFBX();

	D3DXVECTOR3* vec = NULL;
	D3DXVECTOR3* nor = NULL;
	D3DXVECTOR2* uv = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vec = Ship->CopyMeshVertex(&m_nVertices);
	nor = Ship->CopyNormalVertex();
	uv = Ship->CopyUVVertex();

	CTexturedVertex* pVertices = new CTexturedVertex[m_nVertices];

	D3DXVECTOR3 d3dxVertex;
	D3DXMATRIX mtxRotate;

	printf("m_nVertices : %d", m_nVertices);
	for (int i = 0; i < m_nVertices; ++i)
	{
		d3dxVertex = D3DXVECTOR3(vec[i].x*0.075f, vec[i].y*0.075f, vec[i].z*0.075f);
		D3DXMatrixRotationX(&mtxRotate, D3DXToRadian(-90.0f));
		D3DXVec3TransformNormal(&d3dxVertex, &d3dxVertex, &mtxRotate);
		pVertices[i] = CTexturedVertex(d3dxVertex, uv[i]);
	}

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CTexturedVertex);
	m_pnVertexOffsets[0] = 0;

	delete[] vec;
	delete[] nor;
	delete[] uv;

	SetRasterizerState(pd3dDevice);
}

ShipMesh::~ShipMesh()
{
}

void ShipMesh::Animate(ID3D11DeviceContext *pd3dDeviceContext, float t)
{

}

void ShipMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void ShipMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CLightingMesh::Render(pd3dDeviceContext);
}

TreasureChestMesh::TreasureChestMesh(ID3D11Device *pd3dDevice) : CLightingMesh(pd3dDevice)
{
	float fx = 0.5f, fy = 0.5f, fz = 0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fy, fz);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, -fy, -fz);
	m_AABB.m_pRenderObject = new CGameObject();
	m_AABB.m_pRenderObject->SetShader(CShaderResource::pDiffusedShader);
	CMesh* pAABBMesh = new CDiffusedCubeMesh(pd3dDevice, 1.f, 1.f, 1.f, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
	m_AABB.m_pRenderObject->SetMesh(pAABBMesh);

	TreasureChest = new FBXExporter();
	TreasureChest->Initialize();
	TreasureChest->LoadScene("treasureboxfbx.FBX");
	TreasureChest->ImportFBX();

	D3DXVECTOR3* vec = NULL;
	D3DXVECTOR3* nor = NULL;
	D3DXVECTOR2* uv = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vec = TreasureChest->CopyMeshVertex(&m_nVertices);
	nor = TreasureChest->CopyNormalVertex();
	uv = TreasureChest->CopyUVVertex();

	CTexturedVertex* pVertices = new CTexturedVertex[m_nVertices];

	D3DXVECTOR3 d3dxVertex;
	D3DXMATRIX mtxRotate;

	printf("m_nVertices : %d", m_nVertices);
	for (int i = 0; i < m_nVertices; ++i)
	{
		d3dxVertex = D3DXVECTOR3(vec[i].x*0.05, vec[i].y*0.1, vec[i].z*0.1);
		D3DXMatrixRotationX(&mtxRotate, D3DXToRadian(0.0f));
		D3DXVec3TransformNormal(&d3dxVertex, &d3dxVertex, &mtxRotate);
		pVertices[i] = CTexturedVertex(d3dxVertex, uv[i]);
	}

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CTexturedVertex);
	m_pnVertexOffsets[0] = 0;

	delete[] vec;
	delete[] nor;
	delete[] uv;

	SetRasterizerState(pd3dDevice);
}

TreasureChestMesh::~TreasureChestMesh()
{
}

void TreasureChestMesh::Animate(ID3D11DeviceContext *pd3dDeviceContext, float t)
{

}

void TreasureChestMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void TreasureChestMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CLightingMesh::Render(pd3dDeviceContext);
}

CMesh* CMeshResource::pSkyMesh = NULL;
CMesh* CMeshResource::pAABBMesh = NULL;
CMesh* CMeshResource::pShipMesh = NULL;
CMesh* CMeshResource::pTreasureChestMesh = NULL;
CMesh* CMeshResource::pTexturedRectMesh = NULL;
CMesh* CMeshResource::pWaveMesh = NULL;
CMesh* CMeshResource::pStandardVoxelMesh = NULL;
CMesh* CMeshResource::pUphill000Mesh = NULL;
CMesh* CMeshResource::pUphill090Mesh = NULL;
CMesh* CMeshResource::pUphill180Mesh = NULL;
CMesh* CMeshResource::pUphill270Mesh = NULL;
CMesh* CMeshResource::pUI_VoxelPocketMesh = NULL;
CMesh* CMeshResource::pUI_VoxelActivatedMesh = NULL;
CMesh* CMeshResource::pUI_RespawningTextMesh = NULL;
CMesh* CMeshResource::pUI_RespawnigGaugeMesh = NULL;
CMesh* CMeshResource::pUI_StaminaMesh = NULL;
CMesh* CMeshResource::pUI_StaminaGaugeMesh = NULL;
CMesh* CMeshResource::pEffect_ParticleMesh = NULL;

CMeshResource::CMeshResource()
{

}

CMeshResource::~CMeshResource()
{

}

void CMeshResource::CreateMeshResource(ID3D11Device *pd3dDevice)
{
	pSkyMesh = new SkydomMesh(pd3dDevice);
	pAABBMesh = new CDiffusedCubeMesh(pd3dDevice, 1.05f, 1.05f, 1.05f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pShipMesh = new ShipMesh(pd3dDevice);
	//pShipMesh = new CTexturedLightingCubeMesh(pd3dDevice, 4.0f, 3.0f, 6.0f);
	pTreasureChestMesh = new TreasureChestMesh(pd3dDevice);
	pWaveMesh = new CTexturedLightingGridMesh(pd3dDevice, -200.0f, 200.0f, 8.0f, 8.0f, 50, 50);

	pStandardVoxelMesh = new CTexturedLightingCubeMesh(pd3dDevice, 1.0f, 1.0f, 1.0f);
	pUphill000Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_FRONT);		// 앞
	pUphill090Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_RIGHT);		// 우
	pUphill180Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_REAR);		// 뒤
	pUphill270Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_LEFT);		// 좌

	D3DXVECTOR2 d3dxvUV[4];

	d3dxvUV[0]= D3DXVECTOR2(0.0f, 1.0f);
	d3dxvUV[1] = D3DXVECTOR2(0.0f, 0.0f);
	d3dxvUV[2] = D3DXVECTOR2(1.0f, 1.0f);
	d3dxvUV[3] = D3DXVECTOR2(1.0f, 0.0f);
	pTexturedRectMesh = new CTexturedRectMesh(pd3dDevice, 2.0f, 2.0f, d3dxvUV);
	pUI_VoxelPocketMesh = new CTexturedRectMesh(pd3dDevice, 640.0f, 128.0f, d3dxvUV);
	pEffect_ParticleMesh = new CTexturedRectMesh(pd3dDevice, 24.0f, 0.5f, d3dxvUV);
	pUI_VoxelActivatedMesh = new CTexturedRectMesh(pd3dDevice, 128.0f, 128.0f, d3dxvUV);

	d3dxvUV[0] = D3DXVECTOR2(0.0f, 6.0f/7.0f);
	d3dxvUV[1] = D3DXVECTOR2(0.0f, 0.0f);
	d3dxvUV[2] = D3DXVECTOR2(1.0f, 6.0f / 7.0f);
	d3dxvUV[3] = D3DXVECTOR2(1.0f, 0.0f);
	pUI_RespawningTextMesh = new CTexturedRectMesh(pd3dDevice, 500.0f * 1.6f, 60.0f * 1.6f, d3dxvUV);

	d3dxvUV[0] = D3DXVECTOR2(0.0f, 1.0f);
	d3dxvUV[1] = D3DXVECTOR2(0.0f, 6.0f / 7.0f);
	d3dxvUV[2] = D3DXVECTOR2(1.0f, 1.0f);
	d3dxvUV[3] = D3DXVECTOR2(1.0f, 6.0f / 7.0f);
	pUI_RespawnigGaugeMesh = new CTexturedRectMesh(pd3dDevice, 480.0f * 1.6f, 10.0f * 1.6f, d3dxvUV);

	d3dxvUV[0] = D3DXVECTOR2(0.0f, 2.0f/3.0f);
	d3dxvUV[1] = D3DXVECTOR2(0.0f, 0.0f);
	d3dxvUV[2] = D3DXVECTOR2(1.0f, 2.0f / 3.0f);
	d3dxvUV[3] = D3DXVECTOR2(1.0f, 0.0f);
	pUI_StaminaMesh = new CTexturedRectMesh(pd3dDevice, 300.0f * 1.6f, 20.0f * 1.6f, d3dxvUV);

	d3dxvUV[0] = D3DXVECTOR2(0.0f, 1.0f);
	d3dxvUV[1] = D3DXVECTOR2(0.0f, 2.0f / 3.0f);
	d3dxvUV[2] = D3DXVECTOR2(1.0f, 1.0f);
	d3dxvUV[3] = D3DXVECTOR2(1.0f, 2.0f / 3.0f);
	pUI_StaminaGaugeMesh = new CTexturedRectMesh(pd3dDevice, 280.0f * 1.6f, 10.0f * 1.6f, d3dxvUV);
}

void CMeshResource::DestroyMeshResource(void)
{
	if (pSkyMesh)
	{
		delete pSkyMesh;
		pSkyMesh = NULL;
	}
	if (pAABBMesh)
	{
		delete pAABBMesh;
		pAABBMesh = NULL;
	}
	if (pShipMesh)
	{
		delete pShipMesh;
		pShipMesh = NULL;
	}
	if (pTreasureChestMesh)
	{
		delete pTreasureChestMesh;
		pTreasureChestMesh = NULL;
	}
	if (pTexturedRectMesh)
	{
		delete pTexturedRectMesh;
		pTexturedRectMesh = NULL;
	}
	if (pWaveMesh)
	{
		delete pWaveMesh;
		pWaveMesh = NULL;
	}
	if (pStandardVoxelMesh)
	{
		delete pStandardVoxelMesh;
		pStandardVoxelMesh = NULL;
	}
	if (pUphill000Mesh)
	{
		delete pUphill000Mesh;
		pUphill000Mesh = NULL;
	}
	if (pUphill090Mesh)
	{
		delete pUphill090Mesh;
		pUphill090Mesh = NULL;
	}
	if (pUphill180Mesh)
	{
		delete pUphill180Mesh;
		pUphill180Mesh = NULL;
	}
	if (pUphill270Mesh)
	{
		delete  pUphill270Mesh;
		pUphill270Mesh = NULL;
	}
	if (pUI_VoxelPocketMesh)
	{
		delete pUI_VoxelPocketMesh;
		pUI_VoxelPocketMesh = NULL;
	}
	if (pUI_VoxelActivatedMesh)
	{
		delete pUI_VoxelActivatedMesh;
		pUI_VoxelActivatedMesh = NULL;
	}
	if (pUI_RespawningTextMesh)
	{
		delete pUI_RespawningTextMesh;
		pUI_RespawningTextMesh = NULL;
	}
	if (pUI_RespawnigGaugeMesh)
	{
		delete pUI_RespawnigGaugeMesh;
		pUI_RespawnigGaugeMesh = NULL;
	}
	if (pUI_StaminaMesh)
	{
		delete pUI_StaminaMesh;
		pUI_StaminaMesh = NULL;
	}
	if (pUI_StaminaGaugeMesh)
	{
		delete pUI_StaminaGaugeMesh;
		pUI_StaminaGaugeMesh = NULL;
	}
	if (pEffect_ParticleMesh)
	{
		delete pEffect_ParticleMesh;
		pEffect_ParticleMesh = NULL;
	}
}