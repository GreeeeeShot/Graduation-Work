#include "stdafx.h"
#include "BlendingResource.h"

//CBlending::CBlending(ID3D11Device *pd3dDevice)
//{
//
//}
//
//CBlending::~CBlending()
//{
//
//}
//
CBlendingResource::CBlendingResource()
{
}


CBlendingResource::~CBlendingResource()
{
}

ID3D11BlendState* CBlendingResource::pTransparentBlending = NULL;

void CBlendingResource::CreateBlendingResource(ID3D11Device *pd3dDevice)
{
	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;

	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;//D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;// D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	transparentDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;

	pd3dDevice->CreateBlendState(&transparentDesc, &pTransparentBlending);
}
