#pragma once
#include "Mesh.h"			// 오브젝트는 메쉬를 (비)균등 비례, 회전, 이동을 갱신한다.
#include "Shader.h"			// 오브젝트에 적용할 셰이더 기능을 추상화한다. 

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

private:
	int m_nReferences;

public:
	void AddRef();
	void Release();

	D3DXMATRIX m_d3dxmtxWorld;
	CMesh *m_pMesh;
	CShader *m_pShader;

	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);

	//객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

