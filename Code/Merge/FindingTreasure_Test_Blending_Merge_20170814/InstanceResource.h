#pragma once
class CInstanceResource
{
public:
	CInstanceResource();
	~CInstanceResource();
};


class CTexturedLightingInstance
{
public:
	float m_fTexType;
	D3DXMATRIX m_d3dxmtxWorld;
	D3DXCOLOR m_d3dxvMtrlDiffuse;
	D3DXCOLOR m_d3dxvMtrlAmbient;
	D3DXCOLOR m_d3dxvMtrlSpecular;
	D3DXCOLOR m_d3dxvMtrlEmissive;

	CTexturedLightingInstance(
		D3DXMATRIX d3dxmtxWorld,
		float fTexType,
		D3DXCOLOR d3dxvMtrlDiffuse,
		D3DXCOLOR d3dxvMtrlAmbient,
		D3DXCOLOR d3dxvMtrlSpecular,
		D3DXCOLOR d3dxvMtrlEmissive)
	{
		m_fTexType = m_fTexType;
		m_d3dxmtxWorld = d3dxmtxWorld;
		m_d3dxvMtrlDiffuse = d3dxvMtrlDiffuse;
		m_d3dxvMtrlAmbient = d3dxvMtrlAmbient;
		m_d3dxvMtrlSpecular = d3dxvMtrlSpecular;
		m_d3dxvMtrlEmissive = d3dxvMtrlEmissive;
	}
	~CTexturedLightingInstance() { }
};