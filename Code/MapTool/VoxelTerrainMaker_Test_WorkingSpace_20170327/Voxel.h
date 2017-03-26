#pragma once
#include "TextureResource.h"
#include "Object.h"

class CVoxel : public CGameObject
{
public:
	CVoxel(bool bIsActive = true) : m_bIsActive(bIsActive) 
	{ 
		m_fDamping = 1.0f;
		m_fReflection = 1.0f;
	};
	virtual ~CVoxel();

	bool m_bIsActive;
	float m_fDamping;
	float m_fReflection;

	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	void SetfDamping(float fDamping) { m_fDamping = fDamping; }
	void SetReflection(float fReflection) { m_fReflection = fReflection; }
};