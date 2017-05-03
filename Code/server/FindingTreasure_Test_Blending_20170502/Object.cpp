#include "stdafx.h"
#include "Object.h"
#include "Camera.h"

CMaterial::CMaterial()
{
	m_nReferences = 0;
}

CMaterial::~CMaterial()
{
}

CGameObject::CGameObject()
{
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	m_pMesh = NULL;
	m_pShader = NULL;
	m_pTexture = NULL;
	m_pMaterial = NULL;

	m_nReferences = 0;
	m_Vel = D3DXVECTOR3(0.0,0.0,0.0);
	m_LastPos = D3DXVECTOR3(0.0, 0.0, 0.0);
	move = false;
}

CGameObject::~CGameObject()
{
	//if (m_pMesh) m_pMesh->Release();
	//if (m_pShader) m_pShader->Release();
	//if (m_pTexture) m_pTexture->Release();
	//if (m_pMaterial) m_pMaterial->Release();
}

void CGameObject::AddRef()
{
	m_nReferences++;
}

void CGameObject::Release()
{
	if (m_nReferences > 0) m_nReferences--;
	if (m_nReferences <= 0) delete this;
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	//if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	//if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader *pShader)
{
	//if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	//if (m_pShader) m_pShader->AddRef();
}

void CGameObject::SetTexture(CTexture *pTexture)
{
	//if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	//if (m_pTexture) m_pTexture->AddRef();
}

void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	//if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	//if (m_pMaterial) m_pMaterial->AddRef();
}


void CGameObject::SetPosition(float x, float y, float z)
{
	m_d3dxmtxWorld._41 = x;
	m_d3dxmtxWorld._42 = y;
	m_d3dxmtxWorld._43 = z;
}

void CGameObject::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	m_d3dxmtxWorld._41 = d3dxvPosition.x;
	m_d3dxmtxWorld._42 = d3dxvPosition.y;
	m_d3dxmtxWorld._43 = d3dxvPosition.z;
}

D3DXVECTOR3 CGameObject::GetPosition()
{
	return(D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43));
}

void CGameObject::Animate(float fTimeElapsed)
{
}

bool CGameObject::IsVisible(CCamera *pCamera)
{
	bool bIsVisible = true;
	AABB bcBoundingCube;
	if (m_pMesh)
	{
		bcBoundingCube = m_pMesh->m_AABB;
		/*객체의 메쉬의 바운딩 박스(모델 좌표계)를 객체의 월드 변환 행렬로 변환하고 새로운 바운딩 박스를 계산한다.*/
		//bcBoundingCube.Update(&m_d3dxmtxWorld);
	}
	//바운딩 박스(월드 좌표계)가 카메라의 절두체에 포함되는 가를 검사한다. 
	if (pCamera) bIsVisible = pCamera->IsInFrustum(&CPhysicalCollision::MoveAABB(bcBoundingCube, GetPosition()));
	return(bIsVisible);
}

void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pShader)
	{
		m_pShader->UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
		if(m_pMaterial) m_pShader->UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);
		if(m_pTexture) m_pShader->UpdateShaderVariables(pd3dDeviceContext, m_pTexture);
		m_pShader->Render(pd3dDeviceContext);
	}
	if (m_pMesh) m_pMesh->Render(pd3dDeviceContext);
}

CWaveObject::CWaveObject()
{

}
CWaveObject::~CWaveObject()
{

}

void CWaveObject::Animate(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed)
{
	if (m_pMesh) m_pMesh->Animate(pd3dDeviceContext, fTimeElapsed);
}