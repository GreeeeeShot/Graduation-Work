#pragma once
//
//class CBlending
//{
//public:
//	
//	//CMesh 클래스의 생성자와 소멸자를 선언한다. 
//	CBlending(ID3D11Device *pd3dDevice);
//	virtual ~CBlending();
//};
//
//class CTransparentBlending : public CBlending
//{
//public:
//	CTransparentBlending(ID3D11Device *pd3dDevice);
//	virtual ~CTransparentBlending();
//
//
//};

class CBlendingResource
{
public:
	CBlendingResource();
	~CBlendingResource();

	static ID3D11BlendState *pTransparentBlending;
	static ID3D11BlendState *pAddBlending;

	static void CreateBlendingResource(ID3D11Device *pd3dDevice);
};

