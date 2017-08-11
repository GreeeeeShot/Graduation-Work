#pragma once
#include "TextureResource.h"
#include "MeshResource.h"
#include "Object.h"

class CSprite
{
public:
	CTexture* m_pSpriteTexture;
	CGameObject** m_ppSpritePieces;
	D3DXVECTOR3 m_d3dxvVelocity;
	CShader* m_pShader;
	int m_nSpritePiecesNum;

	CSprite();
	~CSprite();

	void SetSpriteTexture(CTexture* pSpriteTexture);
	void SetShader(CShader* pShader);
	void BuildSpritePieces(ID3D11Device *pd3dDevice, float fPieceW, float fPieceH, int nPieceNum);
	void SetPosition(D3DXVECTOR3 d3dxvPos);
	void Moving(D3DXVECTOR3 d3dxvShift);
	void MakeBillBoard(D3DXVECTOR3 d3dxvEyePosition, D3DXVECTOR3 d3dxvLookAt, D3DXVECTOR3 d3dxvUp);
	CGameObject* GetSpritePieceByIndex(int iExtractionIdx);
	void Animate(float fTimeElapsed);
};

