#include "stdafx.h"
#include "LightResource.h"

CMaterialResource::CMaterialResource()
{

}

CMaterialResource::~CMaterialResource()
{

}

CMaterial *CMaterialResource::pStandardMaterial = NULL;
CMaterial *CMaterialResource::pHalfTransMaterial = NULL;

void CMaterialResource::CreateMaterialResource(ID3D11Device *pd3dDevice)
{
	pStandardMaterial = new CMaterial();
	pStandardMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pStandardMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 8.0f);
	pStandardMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	pHalfTransMaterial = new CMaterial();
	pHalfTransMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.6f);
	pHalfTransMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pHalfTransMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 8.0f);
	pHalfTransMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
}