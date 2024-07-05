/// 2024. 06. 02 김상준
/// SFX 관리를 위해 만든 Manager

#pragma once
#include "YunutyEngine.h"
#include "PlayableComponent.h"
#include "SoundSystem.h"

#include <vector>
#include <string>

namespace FMOD
{
	class SoundGroup;
}

class SFXManager
	: public SingletonClass<SFXManager>, public application::PlayableComponent
{
public:
	SFXManager();
	static void PlaySoundfile(string soundPath);
	static void PlaySoundfile3D(string soundPath, Vector3d worldPosition);
	static void SetSFXVolume(float volume);
	static float GetSFXVolume();

	static const unordered_set<string>& GetLoadedSFXList();
	static void SetSFXPriority(string soundPath, int priority);
	static int GetSFXPriority(string soundPath);
	static void SettingSFXGroup(string soundPath, unsigned long long groupIndex);
	static unsigned long long GetSFXGroupIndex(string soundPath);
	static void SetSFXGroupVolume(unsigned long long groupIndex, float volume);
	static float GetSFXGroupVolume(unsigned long long groupIndex);
	static void SetSFXGroupMaxAudible(unsigned long long groupIndex, int audible);
	static int GetSFXGroupMaxAudible(unsigned long long groupIndex);
	static void SetSFXGroupMaxAudibleBehavior(unsigned long long groupIndex, SoundSystem::SOUNDGROUP_BEHAVIOR behavior);
	static SoundSystem::SOUNDGROUP_BEHAVIOR GetSFXGroupMaxAudibleBehavior(unsigned long long groupIndex);

	virtual void OnGameStart() override;
	virtual void OnGameStop() override;

	static std::vector<std::string>& GetSoundGroupNames();

private:
	float localSFXVolume = 1.0f;
	float ratio = 1.f;

	/// default 를 포함한 Group 개수입니다.
	int initSoundGroupCount = 11;
	
	std::vector<std::string> soundGroupNames = std::vector<std::string>();
};
