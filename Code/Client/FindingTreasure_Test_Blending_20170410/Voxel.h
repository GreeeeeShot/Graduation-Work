#pragma once
#include "TextureResource.h"
#include "MeshResource.h"
#include "Object.h"

#define DAMPING_WOOD_0						0.05f
#define DAMPING_GRASS_0						9.0f
#define DAMPING_UPHILL						9.0f
#define DAMPING_NULL						0.0f

#define REFLECTION_WOOD_0					0.5f
#define REFLECTION_GRASS_0					0.36f
#define REFLECTION_NULL						0.0f

#define SIDE_REFLECTION_WOOD_0				0.5f
#define SIDE_REFLECTION_GRASS_0				0.36f
#define SIDE_REFLECTION_NULL				0.0f

enum VOXEL_TYPE {
	VOXEL_TYPE_NOT,
	VOXEL_TYPE_GRASS_0, VOXEL_TYPE_WOOD_0,
	VOXEL_TYPE_UPHILL_0, VOXEL_TYPE_UPHILL_1, VOXEL_TYPE_UPHILL_2, VOXEL_TYPE_UPHILL_3,
	VOXEL_TYPE_BUSH_0, VOXEL_TYPE_SWAMP_0, VOXEL_TYPE_JUMP_0
};

class CVoxel : public CGameObject
{
public:
	CVoxel()
	{ 
		m_eVoxelType = VOXEL_TYPE_GRASS_0;
		m_bIsActive = true;
		m_bIsVisible = true;
		m_fFriction = 1.0f;
		m_fReflection = 1.0f;
		m_fSideReflection = 1.0f;
		m_bIsSlope = false;
		m_eVoxelDir = VOXEL_SLOPE_DIR_FRONT;
	};
	virtual ~CVoxel();

	VOXEL_TYPE m_eVoxelType;
	bool m_bIsVisible;			// ���ü� ����
	bool m_bIsActive;			// �浹 ����
	bool m_bIsSlope;
	float m_fFriction;
	float m_fReflection;
	float m_fSideReflection;
	VOXEL_SLOPE_DIR m_eVoxelDir;

	void SetVoxelType(VOXEL_TYPE eVoxelType) { m_eVoxelType = eVoxelType; }
	void SetVisible(bool bIsVisible) { m_bIsVisible = bIsVisible; }
	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }						// Active�� false�̸� �浹 �ڽ��� �������� �ʴ´�.
	void SetSlope(bool bIsSlope) { m_bIsSlope = bIsSlope; }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetReflection(float fReflection) { m_fReflection = fReflection; }
	void SetSideReflection(float fSideReflection) { m_fSideReflection; }
	void SetVoxelSlopeDir(VOXEL_SLOPE_DIR eVoxelSlopeDir) { m_eVoxelDir = eVoxelSlopeDir; }
	float GetHeightByPlayerPos(D3DXVECTOR3 d3dxvPlayerPos);
};

class CVoxelGrass0 : public CVoxel
{
public :
	CVoxelGrass0()
	{
		m_eVoxelType = VOXEL_TYPE_GRASS_0;
		m_bIsActive = true;
		m_bIsVisible = true;
		m_fFriction = 9.0f;
		m_fReflection = 0.2f;
		m_fSideReflection = 0.2f;
		m_bIsSlope = false;
		m_eVoxelDir = VOXEL_SLOPE_DIR_FRONT;
		SetMesh(CMeshResource::pStandardVoxelMesh);
		SetTexture(CTextureResource::pGrassTexture);
	}
	~CVoxelGrass0() {}
};

class CVoxelWood0 : public CVoxel
{
public:
	CVoxelWood0()
	{
		m_eVoxelType = VOXEL_TYPE_WOOD_0;
		m_bIsActive = true;
		m_bIsVisible = true;
		m_fFriction = 9.0f;
		m_fReflection = 0.2f;
		m_fSideReflection = 0.2f;
		m_bIsSlope = false;
		m_eVoxelDir = VOXEL_SLOPE_DIR_FRONT;
		SetMesh(CMeshResource::pStandardVoxelMesh);
		SetTexture(CTextureResource::pWoodTexture);
	}
	~CVoxelWood0() {}
};

class CVoxelUphill0 : public CVoxel
{
public:
	CVoxelUphill0()
	{
		m_eVoxelType = VOXEL_TYPE_UPHILL_0;
		m_bIsActive = true;
		m_bIsVisible = true;
		m_fFriction = 9.0f;
		m_fReflection = 0.2f;
		m_fSideReflection = 0.2f;
		m_bIsSlope = true;
		m_eVoxelDir = VOXEL_SLOPE_DIR_FRONT;
		SetMesh(CMeshResource::pUphill000Mesh);
		SetTexture(CTextureResource::pGrassTexture);
	}
	~CVoxelUphill0() {}
};

class CVoxelUphill1 : public CVoxel
{
public:
	CVoxelUphill1()
	{
		m_eVoxelType = VOXEL_TYPE_UPHILL_1;
		m_bIsActive = true;
		m_bIsVisible = true;
		m_fFriction = 9.0f;
		m_fReflection = 0.2f;
		m_fSideReflection = 0.2f;
		m_bIsSlope = true;
		m_eVoxelDir = VOXEL_SLOPE_DIR_RIGHT;
		SetMesh(CMeshResource::pUphill090Mesh);
		SetTexture(CTextureResource::pGrassTexture);
	}
	~CVoxelUphill1() {}
};

class CVoxelUphill2 : public CVoxel
{
public:
	CVoxelUphill2()
	{
		m_eVoxelType = VOXEL_TYPE_UPHILL_2;
		m_bIsActive = true;
		m_bIsVisible = true;
		m_fFriction = 9.0f;
		m_fReflection = 0.2f;
		m_fSideReflection = 0.2f;
		m_bIsSlope = true;
		m_eVoxelDir = VOXEL_SLOPE_DIR_REAR;
		SetMesh(CMeshResource::pUphill180Mesh);
		SetTexture(CTextureResource::pGrassTexture);
	}
	~CVoxelUphill2() {}
};

class CVoxelUphill3 : public CVoxel
{
public:
	CVoxelUphill3()
	{
		m_eVoxelType = VOXEL_TYPE_UPHILL_3;
		m_bIsActive = true;
		m_bIsVisible = true;
		m_fFriction = 9.0f;
		m_fReflection = 0.2f;
		m_fSideReflection = 0.2f;
		m_bIsSlope = true;
		m_eVoxelDir = VOXEL_SLOPE_DIR_LEFT;
		SetMesh(CMeshResource::pUphill270Mesh);
		SetTexture(CTextureResource::pGrassTexture);
	}
	~CVoxelUphill3() {}
};

class CVoxelBush0 : public CVoxel
{
public:
	CVoxelBush0()
	{
		m_eVoxelType = VOXEL_TYPE_BUSH_0;
		m_bIsActive = false;
		m_bIsVisible = true;
		m_fFriction = 1.0f;
		m_fReflection = 1.0f;
		m_fSideReflection = 1.0f;
		m_bIsSlope = false;
		m_eVoxelDir = VOXEL_SLOPE_DIR_FRONT;
		SetMesh(CMeshResource::pStandardVoxelMesh);
		SetTexture(CTextureResource::pBushTexture);
	}
	~CVoxelBush0() {}
};

class CVoxelSwamp0 : public CVoxel
{
public:
	CVoxelSwamp0()
	{
		m_eVoxelType = VOXEL_TYPE_SWAMP_0;
		m_bIsActive = true;
		m_bIsVisible = true;
		m_fFriction = 12.5f;
		m_fReflection = 0.0f;
		m_fSideReflection = 0.0f;
		m_bIsSlope = false;
		m_eVoxelDir = VOXEL_SLOPE_DIR_FRONT;
		SetMesh(CMeshResource::pStandardVoxelMesh);
		SetTexture(CTextureResource::pSwampTexture);
	}
	~CVoxelSwamp0() {}
};


class CVoxelJump0 : public CVoxel
{
public:
	CVoxelJump0()
	{
		m_eVoxelType = VOXEL_TYPE_JUMP_0;
		m_bIsActive = true;
		m_bIsVisible = true;
		m_fFriction = 9.0f;
		m_fReflection = 1.5f;
		m_fSideReflection = 0.2f;
		m_bIsSlope = false;
		m_eVoxelDir = VOXEL_SLOPE_DIR_FRONT;
		SetMesh(CMeshResource::pStandardVoxelMesh);
		SetTexture(CTextureResource::pJumpTexture);
	}
	~CVoxelJump0() {}
};