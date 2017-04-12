#pragma once

struct MATERIAL;
class CTexture;

struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX m_d3dxmtxWorld;
};

class CShader
{
private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CShader();
	virtual ~CShader();

	//���̴� �ҽ� �ڵ� ���Ͽ��� ����-���̴��� �����ϴ� �Լ��̴�. 
	void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);
	//���̴� �ҽ� �ڵ� ���Ͽ��� �ȼ�-���̴��� �����ϴ� �Լ��̴�. 
	void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	//�����ϵ� ����Ʈ ���Ͽ��� ���� ���̴��� �ȼ� ���̴��� �����ϴ� �Լ��̴�.
	void CreateVertexShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements);
	void CreatePixelShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName);

	//���̴��� �����ϱ� ���� ȣ���ϴ� �����Լ��̴�.
	virtual void CreateShader(ID3D11Device *pd3dDevice);

	//���̴� Ŭ������ ��� ���۸� �����ϰ� ��ȯ�ϴ� ��� �Լ��� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	//���̴� Ŭ������ ��� ���۸� �����ϴ� ��� �Լ��� �����Ѵ�.
	virtual void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld = NULL);
	virtual void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial = NULL);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture = NULL);

	//�������ϱ� ���� ȣ���ϴ� �����Լ��̴�.
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

public:
	//����-���̴� �������̽� �����Ϳ� �Է�-���̾ƿ� �������̽� �����͸� �����Ѵ�.
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	//�ȼ�-���̴� �������̽� �����͸� �����Ѵ�.
	ID3D11PixelShader *m_pd3dPixelShader;

	//���� ��ȯ ����� ���� ��� ���ۿ� ���� �������̽� �����͸� �����Ѵ�.
	ID3D11Buffer *m_pd3dcbWorldMatrix;

};

class CLightingShader : public CShader
{
public:
	CLightingShader();
	virtual ~CLightingShader();

	//������ �����ϱ� ���� ��� �����̴�.
	ID3D11Buffer *m_pd3dcbMaterial;

	void CreateShader(ID3D11Device *pd3dDevice);				// �Է� ��ġ ��ü �� ���� �� �ȼ� ���̴� ����

	void CreateShaderVariables(ID3D11Device *pd3dDevice);				// ���̴��� ����� �ڿ��� ��� ���۷� ����
	void ReleaseShaderVariables();
	//������ ���̴� ������ ����(����)�ϱ� ���� �Լ��̴�.
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial);
};

class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	~CTexturedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	// virtual void BuildObjects(ID3D11Device *pd3dDevice);
};

class CTexturedLightingShader : public CShader
{
public:
	CTexturedLightingShader();
	~CTexturedLightingShader();

	//������ �����ϱ� ���� ��� �����̴�.
	ID3D11Buffer *m_pd3dcbMaterial;

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	//virtual void BuildObjects(ID3D11Device *pd3dDevice);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);				// ���̴��� ����� �ڿ��� ��� ���۷� ����
	void ReleaseShaderVariables();
	//������ ���̴� ������ ����(����)�ϱ� ���� �Լ��̴�.
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial);
};

class CTexturedNotLightingShader : public CShader
{
public:
	CTexturedNotLightingShader();
	~CTexturedNotLightingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	//virtual void BuildObjects(ID3D11Device *pd3dDevice);

	//void CreateShaderVariables(ID3D11Device *pd3dDevice);				// ���̴��� ����� �ڿ��� ��� ���۷� ����
	//void ReleaseShaderVariables();
	////������ ���̴� ������ ����(����)�ϱ� ���� �Լ��̴�.
	//void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial);
};
