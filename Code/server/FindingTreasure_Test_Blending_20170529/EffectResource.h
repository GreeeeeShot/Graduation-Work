#pragma once

#define COLLISION_PARTICLE_NUM 50
#define COLLISION_PARTICLE_EFFECT_LIFE_TIME 1.5f
#define COLLISION_PARTICLE_EFFECT_GRAVITY -15.0f
#define COLLISION_PARTICLE_EFFECT_AIR_RESISTANCE_COEFFICIENT 8.0f

class CEffect
{
public:
	CPlayer *m_pPlayer;
	D3DXVECTOR3 m_d3dxvOccurPos;
	float m_fLifeTime;

	CEffect(CPlayer *pPlayer, D3DXVECTOR3 d3dxvOccurPos) : m_pPlayer(pPlayer), m_d3dxvOccurPos(d3dxvOccurPos) { m_fLifeTime = 0.0f; }
	~CEffect() {};

	virtual void Init(ID3D11Device *pd3dDevice) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Render(ID3D11DeviceContext*pd3dDeviceContext) {}
	virtual void Destroy(void) {}
};

class CCollisionParticleEffect : public CEffect
{
public:
	CSprite *m_pParticleSprite[COLLISION_PARTICLE_NUM];
	float m_fGravitationalAcceleration;
	float m_fAirResistanceCoefficient;
	
	CCollisionParticleEffect(CPlayer *pPlayer, D3DXVECTOR3 d3dxvOccurPos) : CEffect(pPlayer, d3dxvOccurPos) 
	{
		m_fGravitationalAcceleration = COLLISION_PARTICLE_EFFECT_GRAVITY;
		m_fAirResistanceCoefficient = COLLISION_PARTICLE_EFFECT_AIR_RESISTANCE_COEFFICIENT;
		m_fLifeTime = COLLISION_PARTICLE_EFFECT_LIFE_TIME;
	}
	~CCollisionParticleEffect() {}

	void Init(ID3D11Device *pd3dDevice);
	void Update(float fTimeElapsed);
	void Render(ID3D11DeviceContext*pd3dDeviceContext);
	void Destroy(void);
};

class CEffectResource
{
public:
	CEffectResource();
	~CEffectResource();
};

