#pragma once
#include "MeshResource.h"			// 오브젝트는 메쉬를 (비)균등 비례, 회전, 이동을 갱신한다.
#include "Object.h"
#include "Shader.h"			// 오브젝트에 적용할 셰이더 기능을 추상화한다.

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
	CTexture *m_pTexture;
	CMaterial *m_pMaterial;
		
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	void SetTexture(CTexture *pTexture);
	void SetMaterial(CMaterial *pMaterial);

	//객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();

	void SetLook(D3DXVECTOR3 d3dxvLook) { m_d3dxmtxWorld._31 = d3dxvLook.x, m_d3dxmtxWorld._32 = d3dxvLook.y, m_d3dxmtxWorld._33 = d3dxvLook.z; }
	void SetUp(D3DXVECTOR3 d3dxvUp) { m_d3dxmtxWorld._21 = d3dxvUp.x, m_d3dxmtxWorld._22 = d3dxvUp.y, m_d3dxmtxWorld._23 = d3dxvUp.z; }
	void SetRight(D3DXVECTOR3 d3dxvRight) { m_d3dxmtxWorld._11 = d3dxvRight.x, m_d3dxmtxWorld._12 = d3dxvRight.y, m_d3dxmtxWorld._13 = d3dxvRight.z; }

	D3DXVECTOR3 GetLook(void) { return D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33); }
	D3DXVECTOR3 GetUp(void) { return D3DXVECTOR3(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23); }
	D3DXVECTOR3 GetRight(void) { return D3DXVECTOR3(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13); }

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};