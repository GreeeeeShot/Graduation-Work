#pragma once

#include "FBXLoader.h"

class FbxLoadData
{
protected:
	FBX_LOADER::CFBXLoader* m_pFbxDX11;
	FBX_LOADER::FBX_MESH_NODE* m_meshnode;
	char m_filename[256];
public:
	D3DXVECTOR3* m_posvec;
	D3DXVECTOR3* m_norvec;
	D3DXVECTOR3* m_matvec;
	D3DXVECTOR2* m_tex;
	UINT* m_index;
	int m_nVertices;

	FbxLoadData(char* filename);
	~FbxLoadData();
	virtual void getNodeData();
};


class FbxCharacLoad :public FbxLoadData
{
	FbxCharacLoad(char* filename);
	void getNodeData();
};

class FbxSkyDomLoad :public FbxLoadData
{
	FbxSkyDomLoad(char* filename);
	void getNodeData();
};