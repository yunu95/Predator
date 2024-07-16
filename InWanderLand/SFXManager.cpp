#include "SFXManager.h"

SFXManager::SFXManager()
{
	soundGroupNames.push_back("Default");
	SetSFXGroupPriority(0, defaultSGPriority);
	SetSFXGroupPriorityFlag(0, defaultSGFlag);
	SetSFXGroupPriorityFadeRatio(0, defaultSGFadeRatio);
	SetSFXGroupPriorityFadeOutTime(0, defaultSGFadeOutTime);
	SetSFXGroupPriorityFadeInTime(0, defaultSGFadeInTime);

	for (int i = 1; i < initSoundGroupCount; i++)
	{
		soundGroupNames.push_back("None");
		if (!SoundSystem::GetSoundGroup(i))
		{
			SoundSystem::CreateSoundGroup(i);
			SetSFXGroupPriority(i, defaultSGPriority);
			SetSFXGroupPriorityFlag(i, defaultSGFlag);
			SetSFXGroupPriorityFadeRatio(i, defaultSGFadeRatio);
			SetSFXGroupPriorityFadeOutTime(i, defaultSGFadeOutTime);
			SetSFXGroupPriorityFadeInTime(i, defaultSGFadeInTime);
		}
	}

	for (auto each : SoundSystem::GetLoadedSoundsList())
	{
		soundRandomFlagMap[each] = false;
		soundRandomResourceMap[each].reserve(4);
	}
}

void SFXManager::PlaySoundfile(string soundPath)
{
	auto& sfxManager = SingleInstance();
	SetSFXVolume(sfxManager.localSFXVolume);
	if (sfxManager.soundRandomFlagMap[soundPath] && GetSFXRandomSize(soundPath) != 0)
	{
		auto randomIdx = math::Random::GetRandomInt(0, GetSFXRandomSize(soundPath));
		if (randomIdx != GetSFXRandomSize(soundPath))
		{
			auto& finalStr = sfxManager.soundRandomResourceMap[soundPath][randomIdx];
			if (!finalStr.empty())
			{
				soundPath = finalStr;
			}
		}
	}
	SoundSystem::PlaySoundfile(soundPath).SetVolume(sfxManager.localSFXVolume);
}

void SFXManager::PlaySoundfile3D(string soundPath, Vector3d worldPosition)
{
	auto& sfxManager = SingleInstance();
	SetSFXVolume(sfxManager.localSFXVolume);
	if (sfxManager.soundRandomFlagMap[soundPath] && GetSFXRandomSize(soundPath) != 0)
	{
		auto randomIdx = math::Random::GetRandomInt(0, GetSFXRandomSize(soundPath));
		if (randomIdx != GetSFXRandomSize(soundPath))
		{
			auto& finalStr = sfxManager.soundRandomResourceMap[soundPath][randomIdx];
			if (!finalStr.empty())
			{
				soundPath = finalStr;
			}
		}
	}
	SoundSystem::PlaySoundfile3D(soundPath, worldPosition).SetVolume(sfxManager.localSFXVolume);
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

const unordered_set<string>& SFXManager::GetLoadedSFXList()
{
	return SoundSystem::GetLoadedSoundsList();
}

void SFXManager::SetSFXPriority(string soundPath, int priority)
{
	SoundSystem::SetSoundPriority(soundPath, priority);
	SoundSystem::Set3DSoundPriority(soundPath, priority);
}

int SFXManager::GetSFXPriority(string soundPath)
{
	return SoundSystem::GetSoundPriority(soundPath);
}

void SFXManager::SettingSFXGroup(string soundPath, unsigned long long groupIndex)
{
	SoundSystem::SettingSoundGroupOfSound(soundPath, groupIndex);
	SoundSystem::SettingSoundGroupOf3DSound(soundPath, groupIndex);
}

unsigned long long SFXManager::GetSFXGroupIndex(string soundPath)
{
	return SoundSystem::GetSoundGroupIndexOfSound(soundPath);
}

void SFXManager::SetSFXGroupVolume(unsigned long long groupIndex, float volume)
{
	SoundSystem::SetSoundGroupVolume(groupIndex, volume);
}

float SFXManager::GetSFXGroupVolume(unsigned long long groupIndex)
{
	return SoundSystem::GetSoundGroupVolume(groupIndex);
}

void SFXManager::SetSFXGroupMaxAudible(unsigned long long groupIndex, int audible)
{
	SoundSystem::SetSoundGroupMaxAudible(groupIndex, audible);
}

int SFXManager::GetSFXGroupMaxAudible(unsigned long long groupIndex)
{
	return SoundSystem::GetSoundGroupMaxAudible(groupIndex);
}

void SFXManager::SetSFXGroupMaxAudibleBehavior(unsigned long long groupIndex, SoundSystem::SOUNDGROUP_BEHAVIOR behavior)
{
	SoundSystem::SetSoundGroupMaxAudibleBehavior(groupIndex, behavior);
}

SoundSystem::SOUNDGROUP_BEHAVIOR SFXManager::GetSFXGroupMaxAudibleBehavior(unsigned long long groupIndex)
{
	return SoundSystem::GetSoundGroupMaxAudibleBehavior(groupIndex);
}

void SFXManager::SetSFXGroupPriority(unsigned long long groupIndex, int priority)
{
	SoundSystem::SetSoundGroupPriority(groupIndex, priority);
}

int SFXManager::GetSFXGroupPriority(unsigned long long groupIndex)
{
	return SoundSystem::GetSoundGroupPriority(groupIndex);
}

void SFXManager::SetSFXGroupPriorityFlag(unsigned long long groupIndex, bool flag)
{
	SoundSystem::SetSoundGroupPriorityFlag(groupIndex, flag);
}

bool SFXManager::GetSFXGroupPriorityFlag(unsigned long long groupIndex)
{
	return SoundSystem::GetSoundGroupPriorityFlag(groupIndex);
}

void SFXManager::SetSFXGroupPriorityFadeRatio(unsigned long long groupIndex, float ratio)
{
	SoundSystem::SetSoundGroupPriorityFadeRatio(groupIndex, ratio);
}

float SFXManager::GetSFXGroupPriorityFadeRatio(unsigned long long groupIndex)
{
	return SoundSystem::GetSoundGroupPriorityFadeRatio(groupIndex);
}

void SFXManager::SetSFXGroupPriorityFadeOutTime(unsigned long long groupIndex, float time)
{
	SoundSystem::SetSoundGroupPriorityFadeOutTime(groupIndex, time);
}

float SFXManager::GetSFXGroupPriorityFadeOutTime(unsigned long long groupIndex)
{
	return SoundSystem::GetSoundGroupPriorityFadeOutTime(groupIndex);
}

void SFXManager::SetSFXGroupPriorityFadeInTime(unsigned long long groupIndex, float time)
{
	SoundSystem::SetSoundGroupPriorityFadeInTime(groupIndex, time);
}

float SFXManager::GetSFXGroupPriorityFadeInTime(unsigned long long groupIndex)
{
	return SoundSystem::GetSoundGroupPriorityFadeInTime(groupIndex);
}

void SFXManager::SetSFXRandomFlag(string soundPath, bool flag)
{
	SingleInstance().soundRandomFlagMap[soundPath] = flag;
}

bool SFXManager::GetSFXRandomFlag(string soundPath)
{
	return SingleInstance().soundRandomFlagMap[soundPath];
}

void SFXManager::SetSFXRandomSize(string soundPath, int size)
{
	if (SingleInstance().soundRandomResourceMap[soundPath].size() == size || size < 0)
	{
		return;
	}

	SingleInstance().soundRandomResourceMap[soundPath].resize(size);
}

int SFXManager::GetSFXRandomSize(string soundPath)
{
	return SingleInstance().soundRandomResourceMap[soundPath].size();
}

void SFXManager::AddSFXRandomResorce(string soundPath, string additionalPath)
{
	SingleInstance().soundRandomResourceMap[soundPath].push_back(additionalPath);
}

void SFXManager::ChangeSFXRandomResorce(string soundPath, int index, string changePath)
{
	if (index < 0 || index >= GetSFXRandomSize(soundPath))
	{
		return;
	}

	SingleInstance().soundRandomResourceMap[soundPath][index] = changePath;
}

std::vector<string>& SFXManager::GetSFXRandomResorces(string soundPath)
{
	return SingleInstance().soundRandomResourceMap[soundPath];
}

void SFXManager::OnGameStart()
{
	
}

void SFXManager::OnGameStop()
{
	SoundSystem::StopSound(0.5);
}

std::vector<std::string>& SFXManager::GetSoundGroupNames()
{
	return SingleInstance().soundGroupNames;
}

void SFXManager::Clear()
{
	/// 모든 Sound 의 Priority, SoundGroup 과 Volume, MaxAudible, MaxAudibleBehavior 를 초기화합니다.
	/// 굳이 SoundGroup 을 Erase 하지는 않습니다.

	for (int i = 1; i < initSoundGroupCount; i++)
	{
		soundGroupNames[i] = "None";
	}

	for (int i = 0; i < initSoundGroupCount; i++)
	{
		SetSFXGroupVolume(i, 1.0f);
		SetSFXGroupMaxAudible(i, -1);
		SetSFXGroupMaxAudibleBehavior(i, SoundSystem::SOUNDGROUP_BEHAVIOR::FAIL);
		SetSFXGroupPriority(i, defaultSGPriority);
		SetSFXGroupPriorityFlag(i, defaultSGFlag);
		SetSFXGroupPriorityFadeRatio(i, defaultSGFadeRatio);
		SetSFXGroupPriorityFadeOutTime(i, defaultSGFadeOutTime);
		SetSFXGroupPriorityFadeInTime(i, defaultSGFadeInTime);
	}

	for (auto& soundPath : GetLoadedSFXList())
	{
		SettingSFXGroup(soundPath, 0);
		SetSFXPriority(soundPath, 128);
		soundRandomFlagMap[soundPath] = false;
		soundRandomResourceMap[soundPath].resize(0);
	}
}

bool SFXManager::PreEncoding(json& data) const
{
	for (auto& soundPath : GetLoadedSFXList())
	{
		data["SoundList"][soundPath]["Priority"] = GetSFXPriority(soundPath);
		data["SoundList"][soundPath]["SoundGroupIndex"] = GetSFXGroupIndex(soundPath);
		data["SoundList"][soundPath]["RandomFlag"] = GetSFXRandomFlag(soundPath);
		for (auto& each : GetSFXRandomResorces(soundPath))
		{
			data["SoundList"][soundPath]["RandomResources"].push_back(each);
		}
	}

	for (int i = 0; i < initSoundGroupCount; i++)
	{
		data["SoundGroupList"][i]["Volume"] = GetSFXGroupVolume(i);
		data["SoundGroupList"][i]["MaxAudible"] = GetSFXGroupMaxAudible(i);
		data["SoundGroupList"][i]["Behavior"] = (int)GetSFXGroupMaxAudibleBehavior(i);
		data["SoundGroupList"][i]["Name"] = soundGroupNames[i];
		data["SoundGroupList"][i]["Priority"] = GetSFXGroupPriority(i);
		data["SoundGroupList"][i]["PriorityFlag"] = GetSFXGroupPriorityFlag(i);
		data["SoundGroupList"][i]["PriorityFadeRatio"] = GetSFXGroupPriorityFadeRatio(i);
		data["SoundGroupList"][i]["PriorityFadeOutTime"] = GetSFXGroupPriorityFadeOutTime(i);
		data["SoundGroupList"][i]["PriorityFadeInTime"] = GetSFXGroupPriorityFadeInTime(i);
	}

	return true;
}

bool SFXManager::PostEncoding(json& data) const
{
	return true;
}

bool SFXManager::PreDecoding(const json& data)
{
	if (!data.contains("SoundList") || !data.contains("SoundGroupList"))
		return true;

	for (auto& soundPath : GetLoadedSFXList())
	{
		if (!data["SoundList"].contains(soundPath))
		{
			continue;
		}

		SetSFXPriority(soundPath, data["SoundList"][soundPath]["Priority"]);
		SettingSFXGroup(soundPath, data["SoundList"][soundPath]["SoundGroupIndex"]);

		/// 추후 업데이트 된 내용이라 충돌을 방지하고자 검토합니다.
		if (data["SoundList"][soundPath].contains("RandomFlag"))
		{
			SetSFXRandomFlag(soundPath, data["SoundList"][soundPath]["RandomFlag"]);
		}

		if (data["SoundList"][soundPath].contains("RandomResources"))
		{
			for (int i = 0; i < data["SoundList"][soundPath]["RandomResources"].size(); i++)
			{
				AddSFXRandomResorce(soundPath, data["SoundList"][soundPath]["RandomResources"][i]);
			}
		}
	}

	for (int i = 0; i < initSoundGroupCount; i++)
	{
		SetSFXGroupVolume(i, data["SoundGroupList"][i]["Volume"]);
		SetSFXGroupMaxAudible(i, data["SoundGroupList"][i]["MaxAudible"]);
		SetSFXGroupMaxAudibleBehavior(i, (SoundSystem::SOUNDGROUP_BEHAVIOR)data["SoundGroupList"][i]["Behavior"]);
		
		/// 추후 업데이트 된 내용이라 충돌을 방지하고자 검토합니다.
		if (data["SoundGroupList"][i].contains("Name"))
		{
			soundGroupNames[i] = data["SoundGroupList"][i]["Name"];
		}

		if (data["SoundGroupList"][i].contains("Priority"))
		{
			SetSFXGroupPriority(i, data["SoundGroupList"][i]["Priority"]);
		}

		if (data["SoundGroupList"][i].contains("PriorityFlag"))
		{
			SetSFXGroupPriorityFlag(i, data["SoundGroupList"][i]["PriorityFlag"]);
		}

		if (data["SoundGroupList"][i].contains("PriorityFadeRatio"))
		{
			SetSFXGroupPriorityFadeRatio(i, data["SoundGroupList"][i]["PriorityFadeRatio"]);
		}

		if (data["SoundGroupList"][i].contains("PriorityFadeOutTime"))
		{
			SetSFXGroupPriorityFadeOutTime(i, data["SoundGroupList"][i]["PriorityFadeOutTime"]);
		}

		if (data["SoundGroupList"][i].contains("PriorityFadeInTime"))
		{
			SetSFXGroupPriorityFadeInTime(i, data["SoundGroupList"][i]["PriorityFadeInTime"]);
		}
	}

	return true;
}

bool SFXManager::PostDecoding(const json& data)
{
	return true;
}
