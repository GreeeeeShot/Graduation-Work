#pragma once
#include "Mesh.h"			// 오브젝트는 메쉬를 (비)균등 비례, 회전, 이동을 갱신한다.
#include "Object.h"
#include "Shader.h"			// 오브젝트에 적용할 셰이더 기능을 추상화한다. 

struct MATERIAL
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular; //(r,g,b,a=power)
	D3DXCOLOR m_d3dxcEmissive;
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	
	MATERIAL m_Material;
};

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
	CMaterial *m_pMaterial;
		
	virtual void SetMesh(CMesh *pMesh);
	void SetMaterial(CMaterial *pMaterial);
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

