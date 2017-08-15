#include "stdafx.h"
#include "SoundManager.h"


CSoundManager::CSoundManager()
{
}


CSoundManager::~CSoundManager()
{
}

FMOD::System*	CSoundManager::m_pFmod;
FMOD::Channel* CSoundManager::m_pArrCh[SOUND_NUM];
FMOD::Sound* CSoundManager::m_pArrSounds[SOUND_NUM];

void CSoundManager::CreateSoundResource(void)
{
	FMOD::System_Create(&m_pFmod);
	m_pFmod->init(SOUND_NUM, FMOD_INIT_NORMAL, NULL);

	m_pFmod->createSound("Sound/SOUND_back_wave.mp3", FMOD_LOOP_NORMAL, NULL, &m_pArrSounds[SOUND_TYPE_BACK_WAVE]);
	m_pFmod->createSound("Sound/SOUND_back_forest.mp3", FMOD_LOOP_NORMAL, NULL, &m_pArrSounds[SOUND_TYPE_BACK_FOREST]);

	m_pFmod->createSound("Sound/SOUND_voxel_grass.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_VOXEL_GRASS]);
	m_pFmod->createSound("Sound/SOUND_voxel_ice.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_VOXEL_ICE]);
	m_pFmod->createSound("Sound/SOUND_voxel_snow.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_VOXEL_SNOW]);
	m_pFmod->createSound("Sound/SOUND_voxel_swamp.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_VOXEL_SWAMP]);
	m_pFmod->createSound("Sound/SOUND_voxel_tree.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_VOXEL_TREE]);
	m_pFmod->createSound("Sound/SOUND_splash.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_SPLASH]);
	m_pFmod->createSound("Sound/SOUND_swing1.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_SWING]);
	m_pFmod->createSound("Sound/SOUND_col2.wav", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_COLLISION]);
	m_pFmod->createSound("Sound/SOUND_fall.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_FALL]);
	m_pFmod->createSound("Sound/SOUND_shout.wav", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_SHOUT]);
	m_pFmod->createSound("Sound/SOUND_jump.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_JUMP]);
	m_pFmod->createSound("Sound/SOUND_pickax.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_PICKAX]);
	m_pFmod->createSound("Sound/SOUND_select_ui.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_SELECT_UI]);
	m_pFmod->createSound("Sound/SOUND_select_voxel.mp3", FMOD_DEFAULT, NULL, &m_pArrSounds[SOUND_TYPE_EFFECT_SELECT_VOXEL]);
}

void CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE eSndType, float fVolume, bool bIsOverlapped)				//
{
	m_pFmod->update();
	if (bIsOverlapped) m_pFmod->playSound(m_pArrSounds[eSndType], 0, false, &m_pArrCh[eSndType]);
	else
	{
		bool bIsPlaying;
		m_pArrCh[eSndType]->isPlaying(&bIsPlaying);
		if (!bIsPlaying) m_pFmod->playSound(m_pArrSounds[eSndType], 0, false, &m_pArrCh[eSndType]);
	}
	m_pArrCh[eSndType]->setVolume(fVolume);
}

void CSoundManager::FMODSoundStop(SOUND_TYPE eSndType)
{
	m_pArrCh[eSndType]->stop();
}