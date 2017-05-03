#include "stdafx.h"
#include "Sprite.h"
#include "ShaderResource.h"


CSprite::CSprite()
{
	m_pSpriteTexture = NULL;
	m_ppSpritePieces = NULL;
	m_nSpritePiecesNum = NULL;
}


CSprite::~CSprite()
{
	if (m_ppSpritePieces)
	{
		for (int i = 0; i < m_nSpritePiecesNum; i++)
		{
			delete m_ppSpritePieces[i]->m_pMesh;
			delete m_ppSpritePieces[i];
		}
	}
	delete[]m_ppSpritePieces;
}

void CSprite::SetSpriteTexture(CTexture* pSpriteTexture)
{
	m_pSpriteTexture = pSpriteTexture;
}

void CSprite::BuildSpritePieces(ID3D11Device *pd3dDevice, float fPieceW, float fPieceH, int nPieceNum)
{
	m_nSpritePiecesNum = nPieceNum;
	m_ppSpritePieces = new CGameObject*[nPieceNum];
	
	for (int i = 0; i < nPieceNum; i++)
	{
		m_ppSpritePieces[i] = new CGameObject();
		m_ppSpritePieces[i]->SetTexture(m_pSpriteTexture);
		m_ppSpritePieces[i]->SetShader(CShaderResource::pTexturedNotLightingShader);
		m_ppSpritePieces[i]->SetMesh(new CTexturedSpritePieceMesh(pd3dDevice, fPieceW, fPieceH, nPieceNum, i));
		m_ppSpritePieces[i]->SetPosition(0.0f, -2.5f, +3.99f);
	}
}

CGameObject* CSprite::GetSpritePieceByIndex(int iExtractionIdx)
{
	if (iExtractionIdx >= m_nSpritePiecesNum) return NULL;
	return m_ppSpritePieces[iExtractionIdx];
}

void CSprite::Animate(float fTimeElapsed)
{

}