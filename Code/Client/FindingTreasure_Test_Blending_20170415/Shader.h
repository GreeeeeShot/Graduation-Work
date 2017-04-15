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

	//쉐이더 소스 코드 파일에서 정점-쉐이더를 생성하는 함수이다. 
	void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);
	//쉐이더 소스 코드 파일에서 픽셀-쉐이더를 생성하는 함수이다. 
	void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	//컴파일된 이펙트 파일에서 정점 쉐이더와 픽셀 쉐이더를 생성하는 함수이다.
	void CreateVertexShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements);
	void CreatePixelShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName);

	//쉐이더를 생성하기 위해 호출하는 가상함수이다.
	virtual void CreateShader(ID3D11Device *pd3dDevice);

	//쉐이더 클래스의 상수 버퍼를 생성하고 반환하는 멤버 함수를 선언한다. 
	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void ReleaseShaderVariables();
	//쉐이더 클래스의 상수 버퍼를 갱신하는 멤버 함수를 선언한다.
	virtual void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld = NULL);
	virtual void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial = NULL);
	virtual void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CTexture *pTexture = NULL);

	//렌더링하기 위해 호출하는 가상함수이다.
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

public:
	//정점-쉐이더 인터페이스 포인터와 입력-레이아웃 인터페이스 포인터를 선언한다.
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	//픽셀-쉐이더 인터페이스 포인터를 선언한다.
	ID3D11PixelShader *m_pd3dPixelShader;

	//월드 변환 행렬을 위한 상수 버퍼에 대한 인터페이스 포인터를 선언한다.
	ID3D11Buffer *m_pd3dcbWorldMatrix;

};

class CLightingShader : public CShader
{
public:
	CLightingShader();
	virtual ~CLightingShader();

	//재질을 설정하기 위한 상수 버퍼이다.
	ID3D11Buffer *m_pd3dcbMaterial;

	void CreateShader(ID3D11Device *pd3dDevice);				// 입력 배치 객체 및 정점 및 픽셀 셰이더 생성

	void CreateShaderVariables(ID3D11Device *pd3dDevice);				// 셰이더가 사용할 자원을 상수 버퍼로 설정
	void ReleaseShaderVariables();
	//재질을 쉐이더 변수에 설정(연결)하기 위한 함수이다.
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

	//재질을 설정하기 위한 상수 버퍼이다.
	ID3D11Buffer *m_pd3dcbMaterial;

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	//virtual void BuildObjects(ID3D11Device *pd3dDevice);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);				// 셰이더가 사용할 자원을 상수 버퍼로 설정
	void ReleaseShaderVariables();
	//재질을 쉐이더 변수에 설정(연결)하기 위한 함수이다.
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial);
};

class CTexturedNotLightingShader : public CShader
{
public:
	CTexturedNotLightingShader();
	~CTexturedNotLightingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	//virtual void BuildObjects(ID3D11Device *pd3dDevice);

	//void CreateShaderVariables(ID3D11Device *pd3dDevice);				// 셰이더가 사용할 자원을 상수 버퍼로 설정
	//void ReleaseShaderVariables();
	////재질을 쉐이더 변수에 설정(연결)하기 위한 함수이다.
	//void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial);
};
