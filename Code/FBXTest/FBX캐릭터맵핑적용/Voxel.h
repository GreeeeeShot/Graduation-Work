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
		m_bIsSlope = false;
		m_eVoxelDir = VOXEL_SLOPE_DIR_FRONT;
	};
	virtual ~CVoxel();

	bool m_bIsActive;
	bool m_bIsSlope;
	float m_fFriction;
	float m_fReflection;
	VOXEL_SLOPE_DIR m_eVoxelDir;

	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	void SetSlope(bool bIsSlope) { m_bIsSlope = bIsSlope; }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetReflection(float fReflection) { m_fReflection = fReflection; }
	void SetVoxelSlopeDir(VOXEL_SLOPE_DIR eVoxelSlopeDir) { m_eVoxelDir = eVoxelSlopeDir; }
	float GetHeightByPlayerPos(D3DXVECTOR3 d3dxvPlayerPos);
};