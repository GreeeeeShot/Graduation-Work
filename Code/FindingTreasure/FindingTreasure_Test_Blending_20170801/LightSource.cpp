#include "stdafx.h"
#include "LightSource.h"

CLightSource::CLightSource()
{
	m_pMesh = NULL;
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	m_pShader = NULL;
	D3DXMatrixIdentity(&m_d3dxmtxView);
	D3DXMatrixIdentity(&m_d3dxmtxProjection);
	m_d3dxvPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_CameraForCulling.m_bIsInnerCulling = false;
}

CLightSource::~CLightSource()
{

}

void CLightSource::SetMesh(CMesh *pMesh)
{
	m_pMesh = pMesh;
}

void CLightSource::SetShader(CShader *pShader)
{
	m_pShader = pShader;
}

void CLightSource::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pShader)
	{
		m_pShader->UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
		m_pShader->Render(pd3dDeviceContext);
	}
	if (m_pMesh) m_pMesh->Render(pd3dDeviceContext);
}

void CLightSource::CreateViewMatrix(D3DXVECTOR3 d3dxvPos, D3DXVECTOR3 d3dxvTarget, D3DXVECTOR3 d3dxvUp)
{
	m_d3dxmtxWorld._41 = d3dxvPos.x;
	m_d3dxmtxWorld._42 = d3dxvPos.y;
	m_d3dxmtxWorld._43 = d3dxvPos.z;

	m_d3dxvPos = d3dxvPos;
	m_d3dxvTarget = d3dxvTarget;
	m_d3dxvUp = d3dxvUp;
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &d3dxvPos, &d3dxvTarget, &d3dxvUp);
	CalculateFrustumPlanes();
}

void CLightSource::CreateProjectionMatrix(float fFovDegree, float fLightEmitW, float fLightEmitH, float fLightStart, float fLightEnd)
{
	if (fFovDegree != 0.0f)		// 원근투영을 적용한다.
	{
		D3DXMatrixPerspectiveFovLH(&m_d3dxmtxProjection, (float)D3DXToRadian(fFovDegree), fLightEmitW / fLightEmitH, fLightStart, fLightEnd);
	}
	else						// 직교투영을 적용한다.
	{
		D3DXMatrixOrthoLH(&m_d3dxmtxProjection, fLightEmitW, fLightEmitH, fLightStart, fLightEnd);
	}
	CalculateFrustumPlanes();
}

void CLightSource::CalculateFrustumPlanes(void)
{
	m_CameraForCulling.SetViewMatrix(&m_d3dxmtxView);
	m_CameraForCulling.SetProjectionMatrix(&m_d3dxmtxProjection);
	m_CameraForCulling.CalculateFrustumPlanes();
}

void CLightSource::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	m_d3dxmtxWorld._41 = d3dxvPosition.x;
	m_d3dxmtxWorld._42 = d3dxvPosition.y;
	m_d3dxmtxWorld._43 = d3dxvPosition.z;

	m_d3dxvPos = d3dxvPosition;
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &m_d3dxvPos, &m_d3dxvTarget, &m_d3dxvUp);
	CalculateFrustumPlanes();
}

D3DXVECTOR3 CLightSource::GetPosition(void)
{
	return m_d3dxvPos;
}

void CLightSource::SetTarget(D3DXVECTOR3 d3dxvTarget)
{
	m_d3dxvTarget = d3dxvTarget;
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &m_d3dxvPos, &m_d3dxvTarget, &m_d3dxvUp);
	CalculateFrustumPlanes();
}

D3DXVECTOR3 CLightSource::GetTarget(void)
{
	return m_d3dxvTarget;
}

void CLightSource::SetUp(D3DXVECTOR3 d3dxvUp)
{
	m_d3dxvUp = d3dxvUp;
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &m_d3dxvPos, &m_d3dxvTarget, &m_d3dxvUp);
	CalculateFrustumPlanes();
}

D3DXVECTOR3 CLightSource::GetUp(void)
{
	return m_d3dxvUp;
}