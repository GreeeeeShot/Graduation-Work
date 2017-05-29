#include "stdafx.h"
#include "Sprite.h"
#include "ShaderResource.h"


CSprite::CSprite()
{
	m_pSpriteTexture = NULL;
	m_ppSpritePieces = NULL;
	m_nSpritePiecesNum = NULL;
	m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
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
		//m_ppSpritePieces[i]->SetPosition(0.0f, -2.5f, +3.99f);
	}
}

void  CSprite::SetPosition(D3DXVECTOR3 d3dxvPos)
{
	for (int i = 0; i < m_nSpritePiecesNum; i++)
	{
		m_ppSpritePieces[i]->SetPosition(d3dxvPos);
	}
}

void CSprite::Moving(D3DXVECTOR3 d3dxvShift)
{
	for (int i = 0; i < m_nSpritePiecesNum; i++)
	{
		m_ppSpritePieces[i]->SetPosition(m_ppSpritePieces[i]->GetPosition() + d3dxvShift);
	}
}

void CSprite::MakeBillBoard(D3DXVECTOR3 d3dxvEyePosition, D3DXVECTOR3 d3dxvLookAt, D3DXVECTOR3 d3dxvUp)
{
	D3DXVECTOR3 d3dxvLook;
	D3DXVECTOR3 d3dxvRight;

	d3dxvLook = d3dxvLookAt - d3dxvEyePosition;
	D3DXVec3Normalize(&d3dxvLook, &d3dxvLook);

	D3DXVec3Cross(&d3dxvRight, &d3dxvUp, &d3dxvLook);
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	
	D3DXVec3Cross(&d3dxvUp, &d3dxvLook, &d3dxvRight);
	
	for (int i = 0; i < m_nSpritePiecesNum; i++)
	{
		m_ppSpritePieces[i]->SetLook(d3dxvLook);
		m_ppSpritePieces[i]->SetRight(d3dxvRight);
		m_ppSpritePieces[i]->SetUp(d3dxvUp);
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