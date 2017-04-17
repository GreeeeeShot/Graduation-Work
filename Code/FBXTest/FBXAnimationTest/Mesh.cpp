#include "stdafx.h"
#include "Mesh.h"

CFbxLoadData::CFbxLoadData()
{
	m_pFbxDX11 = new FBX_LOADER::CFBXLoader;
	strcpy_s(m_filename, "make a hole-pir.FBX");
	if (FAILED(m_pFbxDX11->LoadFBX(m_filename, FBX_LOADER::CFBXLoader::eAXIS_OPENGL)))
	{
		MessageBox(NULL,
			L"FBX Error", L"Error", MB_OK);
	}
	getNodeData();
	//GetControlPoint();
}

CFbxLoadData::~CFbxLoadData()
{
}
void CFbxLoadData::GetControlPoint()
{
	int nNode = m_pFbxDX11->GetNodesCount();
	FBX_LOADER::FBX_MESH_NODE* meshNode = new FBX_LOADER::FBX_MESH_NODE[nNode];

	//ReadVertexCacheData();

	FbxNode* node = &(m_pFbxDX11->GetRootNode());
	FbxMesh* mesh = node->GetMesh();

	std::vector<FbxVector4> vertex;
	int nVertice = m_pFbxDX11->GetnVex();


	m_posvec = new D3DXVECTOR3[nVertice];

	for (int i = 0; i < nVertice; ++i)
	{
		m_posvec[i] = m_pFbxDX11->GetControlPoint(i);
	}

	vertex.clear();
	/*nVertice = 0;
	
	for (int i = 0; i < nNode; ++i)
	{
		for (int j = 0; j < meshNode[i].m_texcoordArray.size(); ++j)
		{
			FbxVector4 vec;
			vec.mData[0] = meshNode[i].m_texcoordArray[j].mData[0];
			vec.mData[1] = 1.0 - meshNode[i].m_texcoordArray[j].mData[1];
			vertex.push_back(vec);
			nVertice++;
		}
	}
	m_tex = new D3DXVECTOR2[nVertice];

	for (int i = 0; i < nVertice; ++i)
	{
		m_tex[i].x = vertex[i].mData[0];
		m_tex[i].y = vertex[i].mData[1];
	}
	*/
	m_nVertices = nVertice;
}
void CFbxLoadData::AniTimePerData()
{
	/*
	int nNode = m_pFbxDX11->GetNodesCount();
	FbxNode* node = &(m_pFbxDX11->GetRootNode());
	int nMesh;
	for (int i = 0; i < nNode; ++i)
	{
		FbxNode* node1 = node->GetChild(i);

		FbxMesh* mesh = node->GetMesh();
		if (mesh != NULL)
		{
			nMesh = mesh->GetControlPointsCount();
		}
	}
	*/
//	FbxVector4* vector = new FbxVector4[5000];

	m_pFbxDX11->AnimationTimePerData();

	//vector;
}

void CFbxLoadData::getNodeData()
{
	int nNode = m_pFbxDX11->GetNodesCount();
	FBX_LOADER::FBX_MESH_NODE* meshNode = new FBX_LOADER::FBX_MESH_NODE[nNode];

	//ReadVertexCacheData();

	FbxNode* node = &(m_pFbxDX11->GetRootNode());
	FbxMesh* mesh = node->GetMesh();

	for (int i = 0; i < nNode; ++i)
	{
		FBX_LOADER::FBX_MESH_NODE currentNode = m_pFbxDX11->GetNode(static_cast<unsigned int>(i));
		meshNode[i].name = currentNode.name;
		meshNode[i].parentName = currentNode.parentName;
		meshNode[i].m_positionArray = currentNode.m_positionArray;
		meshNode[i].m_normalArray = currentNode.m_normalArray;
		meshNode[i].indexArray = currentNode.indexArray;
		meshNode[i].m_texcoordArray = currentNode.m_texcoordArray;
		meshNode[i].m_materialArray = currentNode.m_materialArray;
		meshNode[i].elements = currentNode.elements;
		memcpy(meshNode[i].mat4x4, currentNode.mat4x4, sizeof(currentNode.mat4x4));
	}

	std::vector<FbxVector4> vertex;
	int nVertice = 0;

	for (int i = 0; i < nNode; ++i)
	{
		for (int j = 0; j < meshNode[i].m_positionArray.size(); ++j)
		{
			FbxVector4 vec;
			vec.mData[0] = meshNode[i].m_positionArray[j].mData[0];
			vec.mData[1] = meshNode[i].m_positionArray[j].mData[1];
			vec.mData[2] = meshNode[i].m_positionArray[j].mData[2];
			vertex.push_back(vec);
			nVertice++;
		}
	}

	m_posvec = new D3DXVECTOR3[nVertice];

	for (int i = 0; i < nVertice; ++i)
	{
		m_posvec[i].x = vertex[i].mData[0];
		m_posvec[i].y = vertex[i].mData[1];
		m_posvec[i].z = vertex[i].mData[2];
	}

	vertex.clear();
	nVertice = 0;

	for (int i = 0; i < nNode; ++i)
	{
		for (int j = 0; j < meshNode[i].m_normalArray.size(); ++j)
		{
			FbxVector4 vec;
			vec.mData[0] = meshNode[i].m_normalArray[j].mData[0];
			vec.mData[1] = meshNode[i].m_normalArray[j].mData[1];
			vec.mData[2] = meshNode[i].m_normalArray[j].mData[2];
			vertex.push_back(vec);
			nVertice++;
		}
	}

	m_norvec = new D3DXVECTOR3[nVertice];

	for (int i = 0; i < nVertice; ++i)
	{
		m_norvec[i].x = vertex[i].mData[0];
		m_norvec[i].y = vertex[i].mData[1];
		m_norvec[i].z = vertex[i].mData[2];
	}

	nVertice = 0;
	std::vector<unsigned int> v;

	for (int i = 0; i < nNode; ++i)
	{
		for (int j = 0; j < meshNode[i].indexArray.size(); ++j)
		{
			v.push_back(meshNode[i].indexArray[j]);
			nVertice++;
		}
	}

	m_index = new UINT[nVertice];

	for (int i = 0; i < nVertice; ++i)
	{
		m_index[i] = v[i];
	}

	vertex.clear();
	nVertice = 0;

	for (int i = 0; i < nNode; ++i)
	{
		for (int j = 0; j < meshNode[i].m_texcoordArray.size(); ++j)
		{
			FbxVector4 vec;
			vec.mData[0] = meshNode[i].m_texcoordArray[j].mData[0];
			vec.mData[1] = 1.0-meshNode[i].m_texcoordArray[j].mData[1];
			vertex.push_back(vec);
			nVertice++;
		}
	}
	m_tex = new D3DXVECTOR2[nVertice];

	for (int i = 0; i < nVertice; ++i)
	{
		m_tex[i].x = vertex[i].mData[0];
		m_tex[i].y = vertex[i].mData[1];
	}

	m_nVertices = nVertice;

	delete[] meshNode;
}

CTexture::CTexture(int nTextures)
{
	m_nReferences = 0;
	m_nTextures = nTextures;
	m_ppd3dsrvTextures = new ID3D11ShaderResourceView*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) m_ppd3dsrvTextures[i] = NULL;
	m_ppd3dSamplerStates = new ID3D11SamplerState*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) m_ppd3dSamplerStates[i] = NULL;
}

CTexture::~CTexture()
{
	if (m_ppd3dsrvTextures) delete[] m_ppd3dsrvTextures;
	if (m_ppd3dSamplerStates) delete[] m_ppd3dSamplerStates;
}

void CTexture::AddRef()
{
	m_nReferences++;
}

void CTexture::Release()
{
	if (m_nReferences > 0) m_nReferences--;
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppd3dsrvTextures[i]) m_ppd3dsrvTextures[i]->Release();
		if (m_ppd3dSamplerStates[i]) m_ppd3dSamplerStates[i]->Release();
	}
	if (m_nReferences == 0) delete this;
}

void CTexture::SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState)
{
	if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->Release();
	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->Release();
	m_ppd3dsrvTextures[nIndex] = pd3dsrvTexture;
	m_ppd3dSamplerStates[nIndex] = pd3dSamplerState;
	if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->AddRef();
	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->AddRef();
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

	m_pTexture = NULL;

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
	if (m_pTexture) m_pTexture->Release();
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

void CMesh::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
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

//CCubeMesh::CCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice)
//{
//	m_nVertices = 8;
//	//m_nStride = sizeof(CDiffusedVertex);
//	//m_nOffset = 0;
//	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
//
//	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������.
//	CDiffusedVertex pVertices[8];
//	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(-fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(+fx, +fy, -fz), RANDOM_COLOR);
//	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(+fx, +fy, +fz), RANDOM_COLOR);
//	pVertices[3] = CDiffusedVertex(D3DXVECTOR3(-fx, +fy, +fz), RANDOM_COLOR);
//	pVertices[4] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, -fz), RANDOM_COLOR);
//	pVertices[5] = CDiffusedVertex(D3DXVECTOR3(+fx, -fy, -fz), RANDOM_COLOR);
//	pVertices[6] = CDiffusedVertex(D3DXVECTOR3(+fx, -fy, +fz), RANDOM_COLOR);
//	pVertices[7] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, +fz), RANDOM_COLOR);
//
//	D3D11_BUFFER_DESC d3dBufferDesc;
//	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
//	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	d3dBufferDesc.ByteWidth = sizeof(CDiffusedVertex) * m_nVertices;
//	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	d3dBufferDesc.CPUAccessFlags = 0;
//	D3D11_SUBRESOURCE_DATA d3dBufferData;
//	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
//	d3dBufferData.pSysMem = pVertices;
//	//pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);
//
//	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���̹Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� �ε����� ������.*/
//	m_nIndices = 36;
//
//	UINT pIndices[36];
//	//�� �ո�(Front) �簢���� ���� �ﰢ��
//	pIndices[0] = 3; pIndices[1] = 1; pIndices[2] = 0;
//	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
//	pIndices[3] = 2; pIndices[4] = 1; pIndices[5] = 3;
//	//�� ����(Top) �簢���� ���� �ﰢ��
//	pIndices[6] = 0; pIndices[7] = 5; pIndices[8] = 4;
//	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
//	pIndices[9] = 1; pIndices[10] = 5; pIndices[11] = 0;
//	//�� �޸�(Back) �簢���� ���� �ﰢ��
//	pIndices[12] = 3; pIndices[13] = 4; pIndices[14] = 7;
//	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
//	pIndices[15] = 0; pIndices[16] = 4; pIndices[17] = 3;
//	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
//	pIndices[18] = 1; pIndices[19] = 6; pIndices[20] = 5;
//	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
//	pIndices[21] = 2; pIndices[22] = 6; pIndices[23] = 1;
//	//�� ����(Left) �簢���� ���� �ﰢ��
//	pIndices[24] = 2; pIndices[25] = 7; pIndices[26] = 6;
//	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
//	pIndices[27] = 3; pIndices[28] = 7; pIndices[29] = 2;
//	//�� ����(Right) �簢���� ���� �ﰢ��
//	pIndices[30] = 6; pIndices[31] = 4; pIndices[32] = 5;
//	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
//	pIndices[33] = 7; pIndices[34] = 4; pIndices[35] = 6;
//
//	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
//	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
//	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	d3dBufferDesc.CPUAccessFlags = 0;
//	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
//	d3dBufferData.pSysMem = pIndices;
//	//�ε��� ���۸� �����Ѵ�.
//	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);
//
//	CreateRasterizerState(pd3dDevice);
//}

CCubeMesh::~CCubeMesh()
{
}

void CCubeMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
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

CLightingCubeMesh::CLightingCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CLightingMesh(pd3dDevice)
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	// ��ġ ���� ����
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[6] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[7] = D3DXVECTOR3(-fx, -fy, +fz);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���̹Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� �ε����� ������.*/
	m_nIndices = 36;

	UINT pIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	pIndices[0] = 3; pIndices[1] = 1; pIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	pIndices[3] = 2; pIndices[4] = 1; pIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	pIndices[6] = 0; pIndices[7] = 5; pIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pIndices[9] = 1; pIndices[10] = 5; pIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	pIndices[12] = 3; pIndices[13] = 4; pIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	pIndices[15] = 0; pIndices[16] = 4; pIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pIndices[18] = 1; pIndices[19] = 6; pIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pIndices[21] = 2; pIndices[22] = 6; pIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	pIndices[24] = 2; pIndices[25] = 7; pIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	pIndices[27] = 3; pIndices[28] = 7; pIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	pIndices[30] = 6; pIndices[31] = 4; pIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	pIndices[33] = 7; pIndices[34] = 4; pIndices[35] = 6;

	// ���� ���� ���� ����
	D3DXVECTOR3 pd3dxvNormals[8];
	CalculateVertexNormal(pd3dxvNormals, m_pd3dxvPositions, pIndices);
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	// ���� ���� ����
	m_nBuffers = 2;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[2];
	m_ppd3dVertexBuffers[0] = m_pd3dPositionBuffer;
	m_ppd3dVertexBuffers[1] = m_pd3dNormalBuffer;

	m_pnVertexStrides = new UINT[2];
	m_pnVertexStrides[0] = sizeof(D3DXVECTOR3);
	m_pnVertexStrides[1] = sizeof(D3DXVECTOR3);
	
	m_pnVertexOffsets = new UINT[2];
	m_pnVertexOffsets[0] = 0;
	m_pnVertexOffsets[1] = 0;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	//�ε��� ���۸� �����Ѵ�.
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	// ���� ���� ����.

	CreateRasterizerState(pd3dDevice);
}

CLightingCubeMesh::~CLightingCubeMesh()
{

}

void CLightingCubeMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CLightingCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

CLightingCharacterMesh::CLightingCharacterMesh(ID3D11Device *pd3dDevice,CFbxLoadData* data) : CLightingMesh(pd3dDevice)
{
	

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	m_nVertices = data->m_nVertices;
	
	UINT* pIndices = new UINT[m_nVertices];
	

	// ���� ���� ���� ����
	D3DXVECTOR3* pd3dxvNormals = new D3DXVECTOR3[m_nVertices];

	// ��ġ ���� ����
	

	memcpy(m_pd3dxvPositions, data->m_posvec, sizeof(D3DXVECTOR3)*m_nVertices);
	memcpy(pd3dxvNormals, data->m_norvec, sizeof(D3DXVECTOR3)*m_nVertices);
	memcpy(pIndices, data->m_index, sizeof(UINT)*m_nVertices);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	// ���� ���� ����
	m_nBuffers = 2;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[2];
	m_ppd3dVertexBuffers[0] = m_pd3dPositionBuffer;
	m_ppd3dVertexBuffers[1] = m_pd3dNormalBuffer;

	m_pnVertexStrides = new UINT[2];
	m_pnVertexStrides[0] = sizeof(D3DXVECTOR3);
	m_pnVertexStrides[1] = sizeof(D3DXVECTOR3);

	m_pnVertexOffsets = new UINT[2];
	m_pnVertexOffsets[0] = 0;
	m_pnVertexOffsets[1] = 0;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	//�ε��� ���۸� �����Ѵ�.
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	// ���� ���� ����.

	CreateRasterizerState(pd3dDevice);
}

CLightingCharacterMesh::~CLightingCharacterMesh()
{

}

void CLightingCharacterMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CLightingCharacterMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

//CTexturedCubeMesh::CTexturedCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice)
//{
//	m_nVertices = 36;
//	m_nStride = sizeof(CTexturedVertex);
//	m_nOffset = 0;
//	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
//
//	CTexturedVertex pVertices[36];
//	int i = 0;
//	//������ü�� �� �鿡 �ϳ���(�ٸ�) �ؽ��� �̹��� ��ü�� ���εǵ��� �ؽ��� ��ǥ�� �����Ѵ�.
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR2(1.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR2(1.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR2(1.0f, 1.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR2(0.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR2(1.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR2(1.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR2(1.0f, 1.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR2(0.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR2(1.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR2(1.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR2(1.0f, 1.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR2(0.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR2(1.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR2(1.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR2(1.0f, 1.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR2(0.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR2(1.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR2(1.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR2(1.0f, 1.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR2(0.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR2(1.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR2(1.0f, 1.0f));
//
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR2(0.0f, 0.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR2(1.0f, 1.0f));
//	pVertices[i++] = CTexturedVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR2(0.0f, 1.0f));
//
//	D3D11_BUFFER_DESC d3dBufferDesc;
//	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
//	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
//	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	d3dBufferDesc.CPUAccessFlags = 0;
//	D3D11_SUBRESOURCE_DATA d3dBufferData;
//	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
//	d3dBufferData.pSysMem = pVertices;
//	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);
//
//	//������ü�� �� �鿡 �ٸ� �ؽ��ĸ� �����Ϸ��� �ε����� ����� �� �����Ƿ� �ε��� ���۴� �������� �ʴ´�.
//
//	SetRasterizerState(pd3dDevice);
//}

CTexturedCubeMesh::~CTexturedCubeMesh()
{
}

void CTexturedCubeMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CTexturedCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

CTexturedLightingCubeMesh::CTexturedLightingCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CLightingMesh(pd3dDevice)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	CTexturedNormalVertex pVertices[36];
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

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(TOP_TEX_COORD_U, TOP_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, TOP_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(TOP_TEX_COORD_U, TOP_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(TOP_TEX_COORD_U, TOP_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, SIDE_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, MAX_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(BOTTOM_TEX_COORD_U, MAX_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(BOTTOM_TEX_COORD_U, BOTTOM_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, MAX_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(BOTTOM_TEX_COORD_U, BOTTOM_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, BOTTOM_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, HALF_TEX_COORD_V));

	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedNormalVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CTexturedNormalVertex);
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

void CTexturedLightingCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CLightingMesh::Render(pd3dDeviceContext);
}

CTexturedLightingCharacterMesh::CTexturedLightingCharacterMesh(ID3D11Device *pd3dDevice, CFbxLoadData* data) : CLightingMesh(pd3dDevice)
{

	m_nVertices = data->m_nVertices;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	
	CTexturedNormalVertex* pVertices = new CTexturedNormalVertex[m_nVertices];

	for (int i = 0; i < m_nVertices; ++i)
	{
		pVertices[i] = CTexturedNormalVertex(data->m_posvec[i], data->m_norvec[i], data->m_tex[i]);
	}
	/*
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(TOP_TEX_COORD_U, TOP_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, TOP_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(TOP_TEX_COORD_U, TOP_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(TOP_TEX_COORD_U, TOP_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(MAX_TEX_COORD_U, SIDE_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, MAX_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(BOTTOM_TEX_COORD_U, MAX_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(BOTTOM_TEX_COORD_U, BOTTOM_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, MAX_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(BOTTOM_TEX_COORD_U, BOTTOM_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(HALF_TEX_COORD_U, BOTTOM_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));

	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, SIDE_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(MAX_TEX_COORD_U, HALF_TEX_COORD_V));
	pVertices[i++] = CTexturedNormalVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(SIDE_TEX_COORD_U, HALF_TEX_COORD_V));
	*/
	m_nBuffers = 1;
	m_ppd3dVertexBuffers = new ID3D11Buffer*[1];
	m_pnVertexStrides = new UINT[1];
	m_pnVertexOffsets = new UINT[1];

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CTexturedNormalVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_ppd3dVertexBuffers[0]);

	m_pnVertexStrides[0] = sizeof(CTexturedNormalVertex);
	m_pnVertexOffsets[0] = 0;

	SetRasterizerState(pd3dDevice);
}

CTexturedLightingCharacterMesh::~CTexturedLightingCharacterMesh()
{
}

void CTexturedLightingCharacterMesh::SetRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CTexturedLightingCharacterMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CLightingMesh::Render(pd3dDeviceContext);
}

