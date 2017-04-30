#include "stdafx.h"
#include "ShaderResource.h"


CShaderResource::CShaderResource()
{
}


CShaderResource::~CShaderResource()
{
}

CShader* CShaderResource::pTexturedLightingShader = NULL;
CShader* CShaderResource::pDiffusedShader = NULL;

void CShaderResource::CreateShaderResource(ID3D11Device *pd3dDevice)
{
	pTexturedLightingShader = new CTexturedLightingShader();
	pTexturedLightingShader->CreateShader(pd3dDevice);

	pDiffusedShader = new CShader();
	pDiffusedShader->CreateShader(pd3dDevice);
}