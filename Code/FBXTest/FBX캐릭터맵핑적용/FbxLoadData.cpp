#include "stdafx.h"
#include "FbxLoadData.h"

FbxLoadData::FbxLoadData(char* filename)
{
	m_pFbxDX11 = new FBX_LOADER::CFBXLoader;
	strcpy_s(m_filename, filename);
	if (FAILED(m_pFbxDX11->LoadFBX(m_filename, FBX_LOADER::CFBXLoader::eAXIS_OPENGL)))
	{
		MessageBox(NULL,
			L"FBX Error", L"Error", MB_OK);
	}
	getNodeData();
}

void FbxLoadData::getNodeData()
{
	int nNode = m_pFbxDX11->GetNodesCount();
	FBX_LOADER::FBX_MESH_NODE* meshNode = new FBX_LOADER::FBX_MESH_NODE[nNode];
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
		m_posvec[i].x = vertex[i].mData[0]*0.005;
		m_posvec[i].y = vertex[i].mData[1]*0.005;
		m_posvec[i].z = vertex[i].mData[2]*0.005;
	}

	vertex.clear();
	nVertice = 0;

	for (int i = 0; i < nNode; ++i)
	{
		for (int j = 0; j < meshNode[i].m_normalArray.size() / 3; ++j)
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
			vec.mData[1] = meshNode[i].m_texcoordArray[j].mData[1];
			vertex.push_back(vec);
			nVertice++;
		}
	}
	m_tex = new D3DXVECTOR2[nVertice];

	for (int i = 0; i < nVertice; ++i)
	{
		m_tex[i].x = vertex[i].mData[0];
		m_tex[i].y = 1.0 - vertex[i].mData[1];
	}

	m_nVertices = nVertice;

	delete[] meshNode;
}

FbxLoadData::~FbxLoadData()
{
}

FbxCharacLoad::FbxCharacLoad(char* filename) : FbxLoadData(filename)
{

}

void FbxCharacLoad::getNodeData()
{
	int nNode = m_pFbxDX11->GetNodesCount();
	FBX_LOADER::FBX_MESH_NODE* meshNode = new FBX_LOADER::FBX_MESH_NODE[nNode];
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
			vec.mData[0] = meshNode[i].m_positionArray[j].mData[0] * 0.05;
			vec.mData[1] = meshNode[i].m_positionArray[j].mData[1] * 0.05;
			vec.mData[2] = meshNode[i].m_positionArray[j].mData[2] * 0.05;
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
		for (int j = 0; j < meshNode[i].m_normalArray.size() / 3; ++j)
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
			vec.mData[1] = meshNode[i].m_texcoordArray[j].mData[1];
			vertex.push_back(vec);
			nVertice++;
		}
	}
	m_tex = new D3DXVECTOR2[nVertice];

	for (int i = 0; i < nVertice; ++i)
	{
		m_tex[i].x = vertex[i].mData[0];
		m_tex[i].y = 1.0 - vertex[i].mData[1];
	}

	m_nVertices = nVertice;

	delete[] meshNode;
}

FbxSkyDomLoad::FbxSkyDomLoad(char* filename) : FbxLoadData(filename)
{

}

void FbxSkyDomLoad::getNodeData()
{
	int nNode = m_pFbxDX11->GetNodesCount();
	FBX_LOADER::FBX_MESH_NODE* meshNode = new FBX_LOADER::FBX_MESH_NODE[nNode];
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
			vec.mData[0] = meshNode[i].m_positionArray[j].mData[0] * 0.05;
			vec.mData[1] = meshNode[i].m_positionArray[j].mData[1] * 0.05;
			vec.mData[2] = meshNode[i].m_positionArray[j].mData[2] * 0.05;
			vertex.push_back(vec);
			nVertice++;
		}
	}

	m_posvec = new D3DXVECTOR3[nVertice];

	for (int i = 0; i < nVertice/3; ++i)
	{
		m_posvec[i*3].x = vertex[i*3+2].mData[0];
		m_posvec[i*3].y = vertex[i*3+2].mData[1];
		m_posvec[i*3].z = vertex[i*3+2].mData[2];
		m_posvec[i*3+1].x = vertex[i*3+1].mData[0];
		m_posvec[i*3+1].y = vertex[i*3+1].mData[1];
		m_posvec[i*3+1].z = vertex[i*3+1].mData[2];
		m_posvec[i*3+2].x = vertex[i*3].mData[0];
		m_posvec[i*3+2].y = vertex[i*3].mData[1];
		m_posvec[i*3+2].z = vertex[i*3].mData[2];
	}

	vertex.clear();
	nVertice = 0;

	for (int i = 0; i < nNode; ++i)
	{
		for (int j = 0; j < meshNode[i].m_normalArray.size()/3; ++j)
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
			vec.mData[1] = meshNode[i].m_texcoordArray[j].mData[1];
			vertex.push_back(vec);
			nVertice++;
		}
	}
	m_tex = new D3DXVECTOR2[nVertice];

	for (int i = 0; i < nVertice; ++i)
	{
		m_tex[i].x = vertex[i].mData[0];
		m_tex[i].y = 1.0 - vertex[i].mData[1];
	}

	m_nVertices = nVertice;

	delete[] meshNode;
}