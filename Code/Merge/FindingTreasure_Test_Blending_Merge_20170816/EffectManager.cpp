#include "stdafx.h"
#include "EffectManager.h"


std::vector<CEffect*> CEffectManager::m_vecEffect;

CEffectManager::CEffectManager()
{
}


CEffectManager::~CEffectManager()
{
}

void CEffectManager::InitEffectManager(void)
{
	for (int i = 0; i < m_vecEffect.size(); i++)
	{
		if (m_vecEffect[i]) delete m_vecEffect[i];
	}
	m_vecEffect.clear();
}

void CEffectManager::RegisterEffectManager(CEffect *pEffect)
{
	pEffect->Init(CGameManager::GetInstance()->m_pGameFramework->m_pd3dDevice);
	m_vecEffect.push_back(pEffect);
}

void CEffectManager::UpdateEffectManager(float fTimeElapsed)
{
	for (int i = 0; i < m_vecEffect.size(); i++)
	{
		if (m_vecEffect[i])
		{
			m_vecEffect[i]->Update(fTimeElapsed);
			m_vecEffect[i]->m_fLifeTime -= fTimeElapsed;
			if (m_vecEffect[i]->m_fLifeTime < 0.0f)
			{
				m_vecEffect[i]->Destroy();
				m_vecEffect.erase(m_vecEffect.begin() + i);
				i--;
			}
		}
	}
}

void CEffectManager::RenderEffects(ID3D11DeviceContext* pd3dDeviceContext)
{
	for (int i = 0; i < m_vecEffect.size(); i++)
	{
		if (m_vecEffect[i])
		{
			m_vecEffect[i]->Render(pd3dDeviceContext);
		}
	}
}