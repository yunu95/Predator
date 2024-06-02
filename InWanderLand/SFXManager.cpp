#include "SFXManager.h"

void SFXManager::PlaySoundfile(string soundPath)
{
	SetSFXVolume(SingleInstance().localSFXVolume);
	SoundSystem::PlaySoundfile(soundPath).SetVolume(SingleInstance().localSFXVolume);
}

void SFXManager::PlaySoundfile3D(string soundPath, Vector3d worldPosition)
{
	SetSFXVolume(SingleInstance().localSFXVolume);
	SoundSystem::PlaySoundfile3D(soundPath, worldPosition).SetVolume(SingleInstance().localSFXVolume);
}

void SFXManager::SetSFXVolume(float volume)
{
	SingleInstance().localSFXVolume = volume;
	assert(SingleInstance().ratio != 0);
	SoundSystem::SetSFXVolume(volume / SingleInstance().ratio);
}

float SFXManager::GetSFXVolume()
{
	return SingleInstance().localSFXVolume;
}
