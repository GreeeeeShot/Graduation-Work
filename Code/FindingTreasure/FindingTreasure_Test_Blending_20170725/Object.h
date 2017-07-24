#pragma once
#include "MeshResource.h"			// ������Ʈ�� �޽��� (��)�յ� ���, ȸ��, �̵��� �����Ѵ�.
#include "Object.h"
//#include "Shader.h"			// ������Ʈ�� ������ ���̴� ����� �߻�ȭ�Ѵ�.
#include "ShaderResource.h"

class CCamera;

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

	D3DXVECTOR3 m_Vel;
	D3DXVECTOR3 m_LastPos;
	bool move;
		
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	void SetTexture(CTexture *pTexture);
	virtual void SetNormalMap(CTexture *pTexture) {}
	void SetMaterial(CMaterial *pMaterial);

	//��ü�� ��ġ�� �����Ѵ�.
	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();

	void SetLook(D3DXVECTOR3 d3dxvLook) { m_d3dxmtxWorld._31 = d3dxvLook.x, m_d3dxmtxWorld._32 = d3dxvLook.y, m_d3dxmtxWorld._33 = d3dxvLook.z; }
	void SetUp(D3DXVECTOR3 d3dxvUp) { m_d3dxmtxWorld._21 = d3dxvUp.x, m_d3dxmtxWorld._22 = d3dxvUp.y, m_d3dxmtxWorld._23 = d3dxvUp.z; }
	void SetRight(D3DXVECTOR3 d3dxvRight) { m_d3dxmtxWorld._11 = d3dxvRight.x, m_d3dxmtxWorld._12 = d3dxvRight.y, m_d3dxmtxWorld._13 = d3dxvRight.z; }

	D3DXVECTOR3 GetLook(void) { return D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33); }
	D3DXVECTOR3 GetUp(void) { return D3DXVECTOR3(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23); }
	D3DXVECTOR3 GetRight(void) { return D3DXVECTOR3(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13); }

	virtual float GetHeightByPlayerPos(D3DXVECTOR3 d3dxvPlayerPos) { return 0.0f; }

	virtual void Animate(float fTimeElapsed);
	//��ü�� ī�޶��� ����ü ���ο� �ִ� ���� �Ǵ��Ѵ�. 
	bool IsVisible(CCamera *pCamera = NULL);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CWaveObject : public CGameObject
{
public:
	CWaveObject();
	~CWaveObject();

	CTexture *m_pNormalMap;

	void SetNormalMap(CTexture *pTexture);
	void Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed);
	void Render(ID3D11DeviceContext *pd3dDeviceContext);
	//void Render(ID3D11DeviceContext *pd3dDeviceContext);
};