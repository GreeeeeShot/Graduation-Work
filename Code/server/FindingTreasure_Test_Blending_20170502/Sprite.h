#pragma once
#include "TextureResource.h"
#include "MeshResource.h"
#include "Object.h"

class CSprite
{
public:
	CTexture* m_pSpriteTexture;
	CGameObject** m_ppSpritePieces;
	int m_nSpritePiecesNum;

	CSprite();
	~CSprite();

	void SetSpriteTexture(CTexture* pSpriteTexture);
	void BuildSpritePieces(ID3D11Device *pd3dDevice, float fPieceW, float fPieceH, int nPieceNum);
	CGameObject* GetSpritePieceByIndex(int iExtractionIdx);
	void Animate(float fTimeElapsed);
};

