/// 2024. 06. 02 김상준
/// SFX 관리를 위해 만든 Manager

#pragma once
#include "YunutyEngine.h"
#include "PlayableComponent.h"
#include "SoundSystem.h"
#include "Storable.h"

#include <vector>
#include <string>
#include <map>

namespace FMOD
{
	class SoundGroup;
}

class SFXManager
	: public SingletonClass<SFXManager>, public application::PlayableComponent, public application::Storable
{
	friend class application::editor::MapFileManager;
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
	static void SetSFXGroupPriority(unsigned long long groupIndex, int priority);
	static int GetSFXGroupPriority(unsigned long long groupIndex);
	static void SetSFXGroupPriorityFlag(unsigned long long groupIndex, bool flag);
	static bool GetSFXGroupPriorityFlag(unsigned long long groupIndex);
	static void SetSFXGroupPriorityFadeRatio(unsigned long long groupIndex, float ratio);
	static float GetSFXGroupPriorityFadeRatio(unsigned long long groupIndex);
	static void SetSFXGroupPriorityFadeOutTime(unsigned long long groupIndex, float time);
	static float GetSFXGroupPriorityFadeOutTime(unsigned long long groupIndex);
	static void SetSFXGroupPriorityFadeInTime(unsigned long long groupIndex, float time);
	static float GetSFXGroupPriorityFadeInTime(unsigned long long groupIndex);

	static void SetSFXRandomFlag(string soundPath, bool flag);
	static bool GetSFXRandomFlag(string soundPath);
	static void SetSFXRandomSize(string soundPath, int size);
	static int GetSFXRandomSize(string soundPath);
	static void AddSFXRandomResorce(string soundPath, string additionalPath);
	static void ChangeSFXRandomResorce(string soundPath, int index, string changePath);
	static std::vector<string>& GetSFXRandomResorces(string soundPath);

	virtual void OnGameStart() override;
	virtual void OnGameStop() override;

	static std::vector<std::string>& GetSoundGroupNames();

	void Clear();

	virtual bool PreEncoding(json& data) const override;
	virtual bool PostEncoding(json& data) const override;
	virtual bool PreDecoding(const json& data) override;
	virtual bool PostDecoding(const json& data) override;

private:
	/// default 를 포함한 Group 개수입니다.
	int initSoundGroupCount = 31;
	int defaultSGPriority = 128;
	bool defaultSGFlag = false;
	float defaultSGFadeRatio = 0.5f;
	float defaultSGFadeOutTime = 1.0f;
	float defaultSGFadeInTime = 1.0f;
	
	std::vector<std::string> soundGroupNames = std::vector<std::string>();
	std::unordered_map<std::string, bool> soundRandomFlagMap = std::unordered_map<std::string, bool>();
	std::unordered_map<std::string, std::vector<std::string>> soundRandomResourceMap = std::unordered_map<std::string, std::vector<std::string>>();
};
