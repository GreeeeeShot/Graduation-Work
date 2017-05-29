#include "stdafx.h"
#include "FBXAnim.h"

template <typename AnimType>
CFBXAnim<AnimType>::CFBXAnim()
{
	m_pd3dxvVertexList = NULL;
	m_iVertexNum = 0;
	m_pd3dxvNormalList = NULL;
	m_pd3dxvUVList = NULL;
	m_pSetFBXAnim = NULL;
}

template <typename AnimType>
CFBXAnim<AnimType>::~CFBXAnim()
{
	if (m_pd3dxvVertexList) delete[]m_pd3dxvVertexList;
	if (m_pd3dxvNormalList) delete[]m_pd3dxvNormalList;
	if (m_pd3dxvUVList) delete[]m_pd3dxvUVList;
}

CPirateFBXAnim::CPirateFBXAnim()
{
	m_pFBXAnimIdle = new FBXExporter();
	m_pFBXAnimIdle->Initialize();
	m_pFBXAnimIdle->LoadScene("P_standing.FBX");
	m_pFBXAnimIdle->ImportFBX();

	m_pFBXAnimWalk = new FBXExporter();
	m_pFBXAnimWalk->Initialize();
	m_pFBXAnimWalk->LoadScene("P_Walk.FBX");
	m_pFBXAnimWalk->ImportFBX();

	m_pFBXAnimAttack = new FBXExporter();
	m_pFBXAnimAttack->Initialize();
	m_pFBXAnimAttack->LoadScene("P_standing.FBX");
	m_pFBXAnimAttack->ImportFBX();

	m_pFBXAnimDig = new FBXExporter();
	m_pFBXAnimDig->Initialize();
	m_pFBXAnimDig->LoadScene("P_standing.FBX");
	m_pFBXAnimDig->ImportFBX();

	m_pFBXAnimHit = new FBXExporter();
	m_pFBXAnimHit->Initialize();
	m_pFBXAnimHit->LoadScene("P_standing.FBX");
	m_pFBXAnimHit->ImportFBX();

	m_pSetFBXAnim = m_pFBXAnimIdle;
	m_pd3dxvVertexList = m_pFBXAnimIdle->CopyMeshVertex(&m_iVertexNum);
	m_pd3dxvNormalList = m_pFBXAnimIdle->CopyNormalVertex();
	m_pd3dxvUVList = m_pFBXAnimIdle->CopyUVVertex();
}

CPirateFBXAnim::~CPirateFBXAnim()
{

}

void CPirateFBXAnim::SetFBXAnimForType(PIRATE_ANIM_TYPE eAnimType)
{
	switch (eAnimType)
	{
	case PIRATE_ANIM_TYPE_IDLE: 
		m_pSetFBXAnim = m_pFBXAnimIdle;
		break;
	case PIRATE_ANIM_TYPE_WALK:
		m_pSetFBXAnim = m_pFBXAnimWalk;
		break;
	case PIRATE_ANIM_TYPE_ATTACK:
		m_pSetFBXAnim = m_pFBXAnimAttack;
		break;
	case PIRATE_ANIM_TYPE_DIG:
		m_pSetFBXAnim = m_pFBXAnimDig;
		break;
	case PIRATE_ANIM_TYPE_HIT:
		m_pSetFBXAnim = m_pFBXAnimHit;
		break;
	}
}

CFBXAnim<PIRATE_ANIM_TYPE>* CFBXAnimResource::m_pPirateFBXAnim = NULL;

void CFBXAnimResource::CreateFBXAnimResource(void)
{
	m_pPirateFBXAnim = new CPirateFBXAnim();
}