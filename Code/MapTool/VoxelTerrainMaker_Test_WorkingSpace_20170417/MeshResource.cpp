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
	//���� �����͸� �����ϱ� ���� ���� ���� �������̽� �����͸� �����Ѵ�.
	m_ppd3dVertexBuffers = NULL;
	m_nVertices = 0;

	m_pd3dxvPositions = NULL;
	//������ ��ġ ���͸� �����ϱ� ���� ���ۿ� ���� �������̽� �������̴�. 
	m_pd3dPositionBuffer = NULL;

	//������ �����ϱ� ���� �ʿ��� ������ �����̴�. 
	m_nBuffers = 0;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ������ ����Ʈ ���� ��Ÿ���� �迭�̴�. 
	m_pnVertexStrides = NULL;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ���� ��ġ(����Ʈ ��)�� ��Ÿ���� �迭�̴�. 
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

void CMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
}

void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	// if (m_pd3dVertexBuffer) pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer, &m_nStride, &m_nOffset);
	pd3dDeviceContext->IASetVertexBuffers(0, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	//�ε��� ���۰� ������ �ε��� ���۸� ����̽� ���ؽ�Ʈ�� �����Ѵ�.
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	//�����Ͷ����� ���¸� ����̽� ���ؽ�Ʈ�� �����Ѵ�.
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);
	/*�ε��� ���۰� ������ �ε��� ���۸� ����Ͽ� �޽��� �������ϰ� ������ ���� ���۸��� ����Ͽ� �޽��� �������Ѵ�.*/
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, 0);
}

CDiffusedMesh::CDiffusedMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{

}

CDiffusedMesh::~CDiffusedMesh()
{

}

CDiffusedCoordSystemMesh::CDiffusedCoordSystemMesh(
	ID3D11Device *pd3dDevice,
	float AxisXLength, float AxisYLength, float AxisZLength,
	D3DXCOLOR AxisXColor,
	D3DXCOLOR AxisYColor,
	D3DXCOLOR AxisZColor) : CDiffusedMesh(pd3dDevice)
{
	m_nVertices = 6;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(AxisXLength, AxisYLength, AxisZLength);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	CDiffusedVertex pVertices[6];

	// X
	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.0f, 0.0f), AxisXColor);
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(AxisXLength, 0.0f, 0.0f), AxisXColor);
	// Y
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.0f, 0.0f), AxisYColor);
	pVertices[3] = CDiffusedVertex(D3DXVECTOR3(0.0f, AxisYLength, 0.0f), AxisYColor);
	// Z
	pVertices[4] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.0f, 0.0f), AxisZColor);
	pVertices[5] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.0f, AxisZLength), AxisZColor);

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

CDiffusedCoordSystemMesh::~CDiffusedCoordSystemMesh()
{

}

CDiffusedWorkingSpaceMesh::CDiffusedWorkingSpaceMesh(
	ID3D11Device *pd3dDevice, 
	float fWidth, float fHeight, float fDepth) : CDiffusedMesh(pd3dDevice)
{
	m_nVertices = 24;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	float fx = fWidth*0.5f, fz = fDepth*0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, fHeight, fz);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, 0.0f, -fz);

	CDiffusedVertex pVertices[24];

	// ���� �𼭸�
	D3DXCOLOR d3dxcPurple = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);

	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(-fx, fHeight, -fz), d3dxcPurple);
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(-fx, 0.0f, -fz), d3dxcPurple);
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(fx, fHeight, -fz), d3dxcPurple);
	pVertices[3] = CDiffusedVertex(D3DXVECTOR3(fx, 0.0f, -fz), d3dxcPurple);
	pVertices[4] = CDiffusedVertex(D3DXVECTOR3(fx, fHeight, fz), d3dxcPurple);
	pVertices[5] = CDiffusedVertex(D3DXVECTOR3(fx, 0.0f, fz), d3dxcPurple);
	pVertices[6] = CDiffusedVertex(D3DXVECTOR3(-fx, fHeight, fz), d3dxcPurple);
	pVertices[7] = CDiffusedVertex(D3DXVECTOR3(-fx, 0.0f, fz), d3dxcPurple);

	// ���� �𼭸�
	pVertices[8] = CDiffusedVertex(D3DXVECTOR3(-fx, 0.0f, -fz), d3dxcPurple);
	pVertices[9] = CDiffusedVertex(D3DXVECTOR3(fx, 0.0f, -fz), d3dxcPurple);
	pVertices[10] = CDiffusedVertex(D3DXVECTOR3(-fx, fHeight, -fz), d3dxcPurple);
	pVertices[11] = CDiffusedVertex(D3DXVECTOR3(fx, fHeight, -fz), d3dxcPurple);
	pVertices[12] = CDiffusedVertex(D3DXVECTOR3(-fx, fHeight, fz), d3dxcPurple);
	pVertices[13] = CDiffusedVertex(D3DXVECTOR3(fx, fHeight, fz), d3dxcPurple);
	pVertices[14] = CDiffusedVertex(D3DXVECTOR3(-fx, 0.0f, fz), d3dxcPurple);
	pVertices[15] = CDiffusedVertex(D3DXVECTOR3(fx, 0.0f, fz), d3dxcPurple);

	// ���� �𼭸�
	pVertices[16] = CDiffusedVertex(D3DXVECTOR3(-fx, fHeight, -fz), d3dxcPurple);
	pVertices[17] = CDiffusedVertex(D3DXVECTOR3(-fx, fHeight, fz), d3dxcPurple);
	pVertices[18] = CDiffusedVertex(D3DXVECTOR3(fx, fHeight, -fz), d3dxcPurple);
	pVertices[19] = CDiffusedVertex(D3DXVECTOR3(fx, fHeight, fz), d3dxcPurple);
	pVertices[20] = CDiffusedVertex(D3DXVECTOR3(fx, 0.0f, -fz), d3dxcPurple);
	pVertices[21] = CDiffusedVertex(D3DXVECTOR3(fx, 0.0f, fz), d3dxcPurple);
	pVertices[22] = CDiffusedVertex(D3DXVECTOR3(-fx, 0.0f, -fz), d3dxcPurple);
	pVertices[23] = CDiffusedVertex(D3DXVECTOR3(-fx, 0.0f, fz), d3dxcPurple);

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

CDiffusedWorkingSpaceMesh::~CDiffusedWorkingSpaceMesh()
{

}

CDiffusedWorkingBoardMesh::CDiffusedWorkingBoardMesh(
	ID3D11Device *pd3dDevice,
	int iWidthCount, float fWidthIntervalUnit, 
	int iDepthCount, float fDepthIntervalUnit) : CDiffusedMesh(pd3dDevice)
{
	m_nVertices = (iWidthCount + 1) * 2 + (iDepthCount + 1) * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	float fx = iWidthCount * fWidthIntervalUnit * 0.5f, fz = iDepthCount * fDepthIntervalUnit * 0.5f;
	m_AABB.m_d3dxvMax = D3DXVECTOR3(fx, 0.0f, fz);
	m_AABB.m_d3dxvMin = D3DXVECTOR3(-fx, 0.0f, -fz);

	CDiffusedVertex *pVertices = new CDiffusedVertex[m_nVertices];

	D3DXCOLOR d3dxcBlue = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

	// ���� �� ����
	int iCount = 0;

	for (int i = 0; i < iDepthCount + 1; i++)
	{
		pVertices[iCount * 2] = CDiffusedVertex(D3DXVECTOR3(fx, 0.0f, -fz + i * fDepthIntervalUnit), d3dxcBlue);
		pVertices[iCount * 2 + 1] = CDiffusedVertex(D3DXVECTOR3(-fx, 0.0f, -fz + i * fDepthIntervalUnit), d3dxcBlue);
		iCount++;
	}

	// ���� �� ����
	for (int i = 0; i < iWidthCount + 1; i++)
	{
		pVertices[iCount * 2] = CDiffusedVertex(D3DXVECTOR3(-fx + i * fWidthIntervalUnit, 0.0f, -fz), d3dxcBlue);
		pVertices[iCount * 2 + 1] = CDiffusedVertex(D3DXVECTOR3(-fx + i * fWidthIntervalUnit, 0.0f, fz), d3dxcBlue);
		iCount++;
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

	delete[]pVertices;

	SetRasterizerState(pd3dDevice);
}
CDiffusedWorkingBoardMesh::~CDiffusedWorkingBoardMesh()
{

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
		// i��° �ﰢ���� ���ε�
		int idx0 = piIndices[i * 3 + 0];
		int idx1 = piIndices[i * 3 + 1];
		int idx2 = piIndices[i * 3 + 2];

		// i��° �ﰢ���� ������
		D3DXVECTOR3 v0 = pd3dxvVertices[idx0];
		D3DXVECTOR3 v1 = pd3dxvVertices[idx1];
		D3DXVECTOR3 v2 = pd3dxvVertices[idx2];

		// �� ������ ����Ѵ�.
		D3DXVECTOR3 e0 = v1 - v0;
		D3DXVECTOR3 e1 = v2 - v0;
		D3DXVECTOR3 Normal;
		D3DXVec3Cross(&Normal, &e0, &e1);

		// �� �ﰢ���� �� ������ �ٸ� �ﰢ���鵵
		// �����ϹǷ�, �� ������ �� ������ ������ �����Ѵ�.
		pd3dxvNormals[idx0] += Normal;
		pd3dxvNormals[idx1] += Normal;
		pd3dxvNormals[idx2] += Normal;
	}

	// ���� �� ���� v�� normal ������ �� ������ �����ϴ� ���
	// �ﰢ���� �� ������ ������ ����̴�. �̸� ����ȭ�ϸ�
	// ��������� �� �������� ����� �ȴ�.
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
	//������ü�� �� �鿡 �ؽ��� ��ü�� ���εǵ��� �ؽ��� ��ǥ�� �����Ѵ�.

#define TEXTURE_WIDTH		384.0f
#define TEXTURE_HEIGHT		128.0f


	// �� ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f));

	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f / 3.0f, 0.0f));

	// �Ʒ� ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(2.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(2.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));

	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(2.0f / 3.0f, 0.0f));

	// �� ��

	D3DXVECTOR3 p[6];
	D3DXVECTOR2 t[6];
	D3DXVECTOR3 d3dxvNormal;
	D3DXMATRIX d3dxmtxRotation;

	t[0] = D3DXVECTOR2(1.0f / 3.0f, 0.0f);
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
	//������ü�� �� �鿡 �ؽ��� ��ü�� ���εǵ��� �ؽ��� ��ǥ�� �����Ѵ�.

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


	// �� ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, -fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(0.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(1.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(0.0f, 1.0f));

	// �� ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, +fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(0.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxFrontNormal, D3DXVECTOR2(1.0f / 3.0f, 0.0f));

	// ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), m_CubeFaceNormal.m_d3dxLeftNormal, D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, +fz), m_CubeFaceNormal.m_d3dxLeftNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxLeftNormal, D3DXVECTOR2(2.0f / 3.0f, 0.0f));

	// ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), m_CubeFaceNormal.m_d3dxRightNormal, D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, -fz), m_CubeFaceNormal.m_d3dxRightNormal, D3DXVECTOR2(1.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxRightNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));

	// �� ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(1.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(1.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));

	// �� ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, +fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(2.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, +fy, -fz), m_CubeFaceNormal.m_d3dxRearNormal, D3DXVECTOR2(1.0f / 3.0f, 0.0f));

	// �Ʒ� ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(2.0f / 3.0f, 0.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, -fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(1.0f, 0.0f));

	// �Ʒ� ��
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(+fx, -fy, +fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(1.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, +fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(2.0f / 3.0f, 1.0f));
	pVertices[i++] = CTexturedLightingVertex(D3DXVECTOR3(-fx, -fy, -fz), m_CubeFaceNormal.m_d3dxBottomNormal, D3DXVECTOR2(2.0f / 3.0f, 0.0f));

	D3DXMATRIX mtxRotate;

	D3DXMatrixRotationAxis(&mtxRotate, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)D3DXToRadian(90.0f * (float)eVoxelDir));

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

CMesh* CMeshResource::pPirateMesh = NULL;
CMesh* CMeshResource::pSnowWomanMesh = NULL;

CMesh** CMeshResource::ppRegisteredVoxelMesh = NULL;
CMesh* CMeshResource::pStandardVoxelMesh = NULL;
CMesh* CMeshResource::pUphill000Mesh = NULL;
CMesh* CMeshResource::pUphill090Mesh = NULL;
CMesh* CMeshResource::pUphill180Mesh = NULL;
CMesh* CMeshResource::pUphill270Mesh = NULL;

CMesh* CMeshResource::pLocalCoordMesh = NULL;
CMesh* CMeshResource::pWorldCoordMesh = NULL;

CMeshResource::CMeshResource()
{

}

CMeshResource::~CMeshResource()
{

}

void CMeshResource::CreateMeshResource(ID3D11Device *pd3dDevice)
{
	pPirateMesh = new CTexturedLightingCubeMesh(pd3dDevice, 0.5f, 1.0f, 0.5f);

	pPirateMesh = new CTexturedLightingCubeMesh(pd3dDevice, 0.6f, 1.0f, 0.6f);

	pStandardVoxelMesh = new CTexturedLightingCubeMesh(pd3dDevice, 1.0f, 1.0f, 1.0f);
	pUphill000Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_FRONT);		// ��
	pUphill090Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_RIGHT);		// ��
	pUphill180Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_REAR);		// ��
	pUphill270Mesh = new CTexturedLightingUphillMesh(pd3dDevice, 1.0f, 1.0f, 1.0f, VOXEL_SLOPE_DIR_LEFT);		// ��

	ppRegisteredVoxelMesh = new CMesh*[7];
	ppRegisteredVoxelMesh[0] = pStandardVoxelMesh;
	ppRegisteredVoxelMesh[1] = pStandardVoxelMesh;
	ppRegisteredVoxelMesh[2] = pUphill000Mesh;
	ppRegisteredVoxelMesh[3] = pUphill090Mesh;
	ppRegisteredVoxelMesh[4] = pUphill180Mesh;
	ppRegisteredVoxelMesh[5] = pUphill270Mesh;
	ppRegisteredVoxelMesh[6] = pStandardVoxelMesh;

	pLocalCoordMesh = new CDiffusedCoordSystemMesh(pd3dDevice);
	pWorldCoordMesh = new CDiffusedCoordSystemMesh(pd3dDevice);
}