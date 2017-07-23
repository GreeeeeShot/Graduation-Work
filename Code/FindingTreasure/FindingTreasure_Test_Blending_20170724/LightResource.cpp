#include "stdafx.h"
#include "LightResource.h"

CMaterialResource::CMaterialResource()
{

}

CMaterialResource::~CMaterialResource()
{

}

CMaterial *CMaterialResource::pStandardMaterial = NULL;
CMaterial *CMaterialResource::pWaveMaterial = NULL;
CMaterial *CMaterialResource::pHalfTransMaterial = NULL;
CMaterial *CMaterialResource::pHalfMoreTransMaterial = NULL;
CMaterial *CMaterialResource::pHalfLessTransMaterial = NULL;

void CMaterialResource::CreateMaterialResource(ID3D11Device *pd3dDevice)
{
	pStandardMaterial = new CMaterial();
	pStandardMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.6f, 0.6f, 0.7f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.5f, 0.5f, 0.6f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	pWaveMaterial = new CMaterial();
	pWaveMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.8f, 0.4f, 0.2f, 0.43f);
	pWaveMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.35f, 0.35f, 0.24f, 1.0f);
	pWaveMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.9f, 0.6f, 0.4f, 3.0f);
	pWaveMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	pHalfTransMaterial = new CMaterial();
	pHalfTransMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pHalfTransMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pHalfTransMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 8.0f);
	pHalfTransMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	pHalfMoreTransMaterial = new CMaterial();
	pHalfMoreTransMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.33f);
	pHalfMoreTransMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pHalfMoreTransMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 8.0f);
	pHalfMoreTransMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	pHalfLessTransMaterial = new CMaterial();
	pHalfLessTransMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.66f);
	pHalfLessTransMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pHalfLessTransMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 8.0f);
	pHalfLessTransMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
}