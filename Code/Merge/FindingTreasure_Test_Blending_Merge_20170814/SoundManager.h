#pragma once
//#pragma comment(lib, "fmod_vc.lib")
#include "inc/fmod.hpp"
using namespace FMOD;

#define SOUND_NUM 100

enum SOUND_TYPE {
	SOUND_TYPE_BACK_WAVE,
	SOUND_TYPE_BACK_FOREST,
	SOUND_TYPE_EFFECT_VOXEL_GRASS,
	SOUND_TYPE_EFFECT_VOXEL_ICE,
	SOUND_TYPE_EFFECT_VOXEL_SNOW,
	SOUND_TYPE_EFFECT_VOXEL_SWAMP,
	SOUND_TYPE_EFFECT_VOXEL_TREE,
	SOUND_TYPE_EFFECT_SPLASH,
	SOUND_TYPE_EFFECT_SWING,
	SOUND_TYPE_EFFECT_COLLISION,
	SOUND_TYPE_EFFECT_FALL,
	SOUND_TYPE_EFFECT_SHOUT,
	SOUND_TYPE_EFFECT_JUMP,
	SOUND_TYPE_EFFECT_PICKAX,
	SOUND_TYPE_EFFECT_SELECT_UI,
	SOUND_TYPE_EFFECT_SELECT_VOXEL
};

class CSoundManager
{
public:
	CSoundManager();
	~CSoundManager();

public:
	static System*	m_pFmod;
	static Channel* m_pArrCh[SOUND_NUM];
	static Sound* m_pArrSounds[SOUND_NUM];

public:
	static void CreateSoundResource(void);

	static void FMODSoundPlayAndRepeat(SOUND_TYPE eSndType, float fVolume = 0.5f, bool bIsOverlapped = true);				//
																															/*static void FMODSoundPause(SOUND_TYPE eSndType);
																															static void FMODSoundResume(SOUND_TYPE eSndType);*/
	static void FMODSoundStop(SOUND_TYPE eSndType);
};

