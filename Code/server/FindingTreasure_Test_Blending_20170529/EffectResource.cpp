#include "stdafx.h"
#include "EffectResource.h"

void CCollisionParticleEffect::Init(ID3D11Device *pd3dDevice)
{
	D3DXVECTOR3 d3dxvEffectNormal;
	D3DXVECTOR3 d3dxvRotateAxis;
	D3DXMATRIX mtxRotate;
	
	float fVelocityLengthXZ;
	float fDot;
	float fRotateRadian;

	D3DXVec3Normalize(&d3dxvEffectNormal, &(m_pPlayer->GetPosition() - m_d3dxvOccurPos));
	D3DXVec3Cross(&d3dxvRotateAxis, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &d3dxvEffectNormal);
	fDot = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), &d3dxvEffectNormal);
	if (fDot >= 1.0f) fDot = 1.0f;
	else if (fDot <= -1.0f) fDot = -1.0f;
	fRotateRadian = acosf(fDot);

	D3DXMatrixRotationAxis(&mtxRotate, &d3dxvRotateAxis, fRotateRadian);

	for (int i = 0; i < COLLISION_PARTICLE_NUM; i++)
	{
		m_pParticleSprite[i] = new CSprite();
		m_pParticleSprite[i]->SetSpriteTexture(CTextureResource::pEffect_ParticleBlue0);
		m_pParticleSprite[i]->BuildSpritePieces(pd3dDevice, 0.3f, 0.3f, 5);
		m_pParticleSprite[i]->SetPosition(m_d3dxvOccurPos);
		m_pParticleSprite[i]->MakeBillBoard(m_d3dxvOccurPos, m_pPlayer->m_CameraOperator.m_Camera.GetPosition(), D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		fVelocityLengthXZ = rand() % (14 - 5 + 1) + 2;
		m_pParticleSprite[i]->m_d3dxvVelocity =
			D3DXVECTOR3(
				fVelocityLengthXZ * cos(D3DXToRadian(360.0f / (float)COLLISION_PARTICLE_NUM * i)), 
				rand() % (6 - (-2) + 1) + (-2), 
				fVelocityLengthXZ * sin(D3DXToRadian(360.0f / (float)COLLISION_PARTICLE_NUM * i)));
		D3DXVec3TransformNormal(&m_pParticleSprite[i]->m_d3dxvVelocity, &m_pParticleSprite[i]->m_d3dxvVelocity, &mtxRotate);
	}
}

void CCollisionParticleEffect::Update(float fTimeElapsed)
{
	D3DXVECTOR3 d3dxvPostV;
	D3DXVECTOR3 d3dxvShift;
	D3DXVECTOR3 d3dxvG = D3DXVECTOR3(0.0f, m_fGravitationalAcceleration, 0.0f);
	D3DXVECTOR2 d3dxvVelocityXZ;
	float fLengthVelocityXZ;
	float fDeceleration;

	//printf("G : %f\n", m_fGravitationalAcceleration);

	for (int i = 0; i < COLLISION_PARTICLE_NUM; i++)
	{
		if (m_pParticleSprite[i])
		{
			d3dxvPostV = m_pParticleSprite[i]->m_d3dxvVelocity + d3dxvG * fTimeElapsed;
			

			d3dxvVelocityXZ = -D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
			fLengthVelocityXZ = D3DXVec2Length(&d3dxvVelocityXZ);
			D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
			fDeceleration = COLLISION_PARTICLE_EFFECT_AIR_RESISTANCE_COEFFICIENT * fTimeElapsed;
			if (fDeceleration > fLengthVelocityXZ) fDeceleration = fLengthVelocityXZ;
			d3dxvVelocityXZ = d3dxvVelocityXZ * fDeceleration;
			d3dxvPostV.x += d3dxvVelocityXZ.x;
			d3dxvPostV.z += d3dxvVelocityXZ.y;

			d3dxvShift = ((m_pParticleSprite[i]->m_d3dxvVelocity + d3dxvPostV)  * fTimeElapsed) / 2.0f;

			m_pParticleSprite[i]->Moving(d3dxvShift);
			m_pParticleSprite[i]->m_d3dxvVelocity = d3dxvPostV;
		}
	}
}

void CCollisionParticleEffect::Render(ID3D11DeviceContext*pd3dDeviceContext)
{
	float fInterp;
	int iPiecesNum;

	for (int i = 0; i < COLLISION_PARTICLE_NUM; i++)
	{
		if (m_pParticleSprite[i])
		{
			fInterp = m_fLifeTime / COLLISION_PARTICLE_EFFECT_LIFE_TIME;
			iPiecesNum = m_pParticleSprite[i]->m_nSpritePiecesNum;
			m_pParticleSprite[i]->GetSpritePieceByIndex(0.0f * fInterp + (iPiecesNum - 1) * (1.0f - fInterp))->Render(pd3dDeviceContext);
		}
	}
}

void CCollisionParticleEffect::Destroy(void)
{
	for (int i = 0; i < COLLISION_PARTICLE_NUM; i++)
	{
		if (m_pParticleSprite[i]) delete m_pParticleSprite[i];
	}
}

CEffectResource::CEffectResource()
{
}


CEffectResource::~CEffectResource()
{
}
