#pragma once
#include "TextureResource.h"
#include "Object.h"

class CVoxel : public CGameObject
{
public:
	CVoxel(bool bIsActive = true) : m_bIsActive(bIsActive) {};
	virtual ~CVoxel();

	bool m_bIsActive;
	float m_fFrictional;

	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	void SetFrictional(float fFrictional) { m_fFrictional = fFrictional; }
};