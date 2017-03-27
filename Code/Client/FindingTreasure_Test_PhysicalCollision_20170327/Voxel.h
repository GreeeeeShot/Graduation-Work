#pragma once
#include "TextureResource.h"
#include "Object.h"

class CVoxel : public CGameObject
{
public:
	CVoxel(bool bIsActive = true) : m_bIsActive(bIsActive) 
	{ 
		m_fFriction = 1.0f;
		m_fReflection = 1.0f;
	};
	virtual ~CVoxel();

	bool m_bIsActive;
	float m_fFriction;
	float m_fReflection;

	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetReflection(float fReflection) { m_fReflection = fReflection; }
};