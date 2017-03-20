#pragma once
#include "Mesh.h"			// ������Ʈ�� �޽��� (��)�յ� ���, ȸ��, �̵��� �����Ѵ�.
#include "Object.h"
#include "Shader.h"			// ������Ʈ�� ������ ���̴� ����� �߻�ȭ�Ѵ�. 

class CGameObject
{
private:
	int m_nReferences;

public:
	CGameObject();
	virtual ~CGameObject();
	
	void AddRef();
	void Release();

	CMesh *m_pMesh;
	D3DXMATRIX m_d3dxmtxWorld;
	CShader *m_pShader;
	
		
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);

	//��ü�� ��ġ�� �����Ѵ�.
	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CVoxelObject : public CGameObject
{
private:
	bool m_bIsVisible;

public:
	CVoxelObject(bool bIsVisible = true) : m_bIsVisible(bIsVisible) {};
	~CVoxelObject();

	void SetVisible(bool bIsVisible) { m_bIsVisible = bIsVisible; }
};