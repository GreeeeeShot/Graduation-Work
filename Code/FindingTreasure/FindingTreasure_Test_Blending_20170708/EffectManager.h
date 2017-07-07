#pragma once

#include "EffectResource.h"

class CEffectManager
{
public:
	static std::vector<CEffect*> m_vecEffect;
	CEffectManager();
	~CEffectManager();

	static void InitEffectManager(void);
	static void RegisterEffectManager(CEffect *pEffect);
	static void UpdateEffectManager(float fTimeElapsed);
	static void RenderEffects(ID3D11DeviceContext*pd3dDeviceContext);
};

