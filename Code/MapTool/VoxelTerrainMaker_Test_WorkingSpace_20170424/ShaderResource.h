#pragma once
#include "Shader.h"

class CShaderResource
{
public:
	CShaderResource();
	~CShaderResource();

	static CShader* pTexturedLightingShader;
	static CShader* pDiffusedShader;

	static void CreateShaderResource(ID3D11Device *pd3dDevice);
};

