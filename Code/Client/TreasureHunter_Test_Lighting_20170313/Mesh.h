#pragma once
/*������ ������ ��������(Random) �����ϱ� ���� ����Ѵ�. �� ������ ������ ����(Random Number)�� �����Ͽ� �����Ѵ�.*/
#define RANDOM_COLOR D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)

// �޽��� �����ϴ� ������ ���� Ŭ�����̴�.
class CVertex
{
	//������ ��ġ ����(3���� ����)�� �����ϱ� ���� ��� ������ �����Ѵ�. 
	D3DXVECTOR3 m_d3dxvPosition;

public:
	//CVertex Ŭ������ �����ڿ� �Ҹ��ڸ� ������ ���� �����Ѵ�. 
	CVertex() { m_d3dxvPosition = D3DXVECTOR3(0, 0, 0); }
	CVertex(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
	D3DXVECTOR3 GetPositiion(void) { return m_d3dxvPosition; }
	~CVertex() { }
};

class CDiffusedVertex		// ������ ������ �����̴�.
{
	D3DXVECTOR3 m_d3dxvPosition;
	//������ ������ ��Ÿ���� ��� ����(D3DXCOLOR ����ü)�� �����Ѵ�. 
	D3DXCOLOR m_d3dxcDiffuse;	//������ ���� ������ �߰��Ǿ���.
public:
	//�����ڿ� �Ҹ��ڸ� �����Ѵ�.
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }
};

class CMesh
{
public:
	//CMesh Ŭ������ �����ڿ� �Ҹ��ڸ� �����Ѵ�. 
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

	//CMesh Ŭ���� ��ü�� ����(Reference)�� ���õ� ��� ������ �Լ��� �����Ѵ�.
private:
	int m_nReferences;

public:
	void AddRef();
	void Release();

protected:
	//������ ��ҵ��� ��Ÿ���� ���۵��� �Է������⿡ �����ϱ� ���� �����̴�. 
	ID3D11Buffer **m_ppd3dVertexBuffers;

	int m_nVertices; 
	D3DXVECTOR3 * m_pd3dxvPositions;
	//������ ��ġ ���͸� �����ϱ� ���� ���ۿ� ���� �������̽� �������̴�. 
	ID3D11Buffer *m_pd3dPositionBuffer;

	//������ �����ϱ� ���� �ʿ��� ������ �����̴�. 
	int m_nBuffers;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ������ ����Ʈ ���� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexStrides;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ���� ��ġ(����Ʈ ��)�� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexOffsets;

	/*�ε��� ����(�ε����� �迭)�� ���� �������̽� �������̴�. �ε��� ���۴� ���� ����(�迭)�� ���� �ε����� ������.*/
	ID3D11Buffer *m_pd3dIndexBuffer;
	//�ε��� ���۰� �����ϴ� �ε����� �����̴�. 
	UINT m_nIndices;
	//�ε��� ���ۿ��� �޽��� ǥ���ϱ� ���� ���Ǵ� ���� �ε����̴�. 
	UINT m_nStartIndex;
	//�� �ε����� ������ �ε����̴�. 
	int m_nBaseVertex;

	//���� �����Ͱ� � ������Ƽ�긦 ǥ���ϰ� �ִ� ���� ��Ÿ���� ��� ������ �����Ѵ�.
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	/*�����Ͷ����� ���� ��ü�� ���� �������̽� �����͸� �����Ѵ�. �����Ͷ����� ���� ��ü�� ��CGameObject" Ŭ������ ����� �߰��Ͽ��� �ȴ�. �����Ͷ����� ���� ��ü�� �޽��� ����� ���� ���� ��ü�� ����� ���� �ǹ̻��� ���̸� �����غ��⸦ �ٶ���.*/
	ID3D11RasterizerState *m_pd3dRasterizerState;

	//���� �����͸� �������ϴ� ��� �Լ��� �����Ѵ�.
public:
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D11Device *pd3dDevice);
	virtual ~CTriangleMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CCubeMesh : public CMesh
{
public:
	//������ü�� ����, ����, ������ ũ�⸦ �����Ͽ� ������ü �޽��� �����Ѵ�.
	CCubeMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CLightingMesh : public CMesh
{
public:
	CLightingMesh(ID3D11Device *pd3dDevice);
	virtual ~CLightingMesh();

protected:
	//������ ������ ����ϱ� ���Ͽ� �������Ͱ� �ʿ��ϴ�.
	ID3D11Buffer *m_pd3dNormalBuffer;	

public:
	void CalculateVertexNormal(D3DXVECTOR3 *pd3dxvNormals, D3DXVECTOR3 *pd3dxvVertices, UINT *piIndices);
};

class CLightingCubeMesh : public CLightingMesh
{
public:
	CLightingCubeMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CLightingCubeMesh();

	//������ �����ϱ� ���� ��� �����̴�.
	ID3D11Buffer	 *m_pd3dcbMaterial;

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};