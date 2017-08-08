#pragma once

enum PIRATE_ANIM_TYPE { PIRATE_ANIM_TYPE_IDLE, PIRATE_ANIM_TYPE_WALK, PIRATE_ANIM_TYPE_ATTACK, PIRATE_ANIM_TYPE_DIG, PIRATE_ANIM_TYPE_HIT};

template <typename AnimType>				// typename : ����� �� �ִ� �ִϸ��̼� Ÿ���� ������.
class CFBXAnim
{
public:
	D3DXVECTOR3* m_pd3dxvVertexList;
	int m_iVertexNum;
	D3DXVECTOR3* m_pd3dxvNormalList;
	D3DXVECTOR2* m_pd3dxvUVList;
	FBXExporter* m_pSetFBXAnim;

public:
	CFBXAnim();
	~CFBXAnim();

	virtual void SetFBXAnimForType(AnimType) {}
};

class CPirateFBXAnim : public CFBXAnim<PIRATE_ANIM_TYPE>
{
private:
	FBXExporter* m_pFBXAnimIdle;
	FBXExporter* m_pFBXAnimWalk;
	FBXExporter* m_pFBXAnimAttack;
	FBXExporter* m_pFBXAnimDig;
	FBXExporter* m_pFBXAnimHit;

public:
	CPirateFBXAnim();
	~CPirateFBXAnim();

	void SetFBXAnimForType(PIRATE_ANIM_TYPE);
};

class CFBXAnimResource
{
public:
	CFBXAnimResource();
	~CFBXAnimResource();

	static CFBXAnim<PIRATE_ANIM_TYPE> *m_pPirateFBXAnim;

	static void CreateFBXAnimResource(void);
	static void DestoryFBXAnimResource(void);
};