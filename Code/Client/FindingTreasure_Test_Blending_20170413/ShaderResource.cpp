#include "stdafx.h"
#include "ShaderResource.h"


CShaderResource::CShaderResource()
{
}


CShaderResource::~CShaderResource()
{
}

CShader* CShaderResource::pTexturedLightingShader = NULL;
CShader* CShaderResource::pTexturedNotLightingShader = NULL;

void CShaderResource::CreateShaderResource(ID3D11Device *pd3dDevice)
{
	pTexturedLightingShader = new CTexturedLightingShader();
	pTexturedLightingShader->CreateShader(pd3dDevice);
	pTexturedNotLightingShader = new CTexturedNotLightingShader();
	pTexturedNotLightingShader->CreateShader(pd3dDevice);
}