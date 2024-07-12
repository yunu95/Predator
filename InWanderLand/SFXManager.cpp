#include "SFXManager.h"

SFXManager::SFXManager()
{
	soundGroupNames.push_back("Default");
	soundGroupPriorityMap[0] = 128;
	soundGroupPriorityFlagMap[0] = false;
	soundGroupPriorityFadeRatioMap[0] = 0.5f;
	soundGroupPriorityFadeTimeMap[0] = 1.0f;
	for (int i = 1; i < initSoundGroupCount; i++)
	{
		soundGroupNames.push_back("None");
		soundGroupPriorityMap[i] = 128;
		soundGroupPriorityFlagMap[i] = false;
		soundGroupPriorityFadeRatioMap[i] = 0.5f;
		soundGroupPriorityFadeTimeMap[i] = 1.0f;
		if (!SoundSystem::GetSoundGroup(i))
		{
			SoundSystem::CreateSoundGroup(i);
		}
	}
}

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
	}

	for (auto& soundPath : GetLoadedSFXList())
	{
		SettingSFXGroup(soundPath, 0);
		SetSFXPriority(soundPath, 128);
	}
}

bool SFXManager::PreEncoding(json& data) const
{
	for (auto& soundPath : GetLoadedSFXList())
	{
		data["SoundList"][soundPath]["Priority"] = GetSFXPriority(soundPath);
		data["SoundList"][soundPath]["SoundGroupIndex"] = GetSFXGroupIndex(soundPath);
	}

	for (int i = 0; i < initSoundGroupCount; i++)
	{
		data["SoundGroupList"][i]["Volume"] = GetSFXGroupVolume(i);
		data["SoundGroupList"][i]["MaxAudible"] = GetSFXGroupMaxAudible(i);
		data["SoundGroupList"][i]["Behavior"] = (int)GetSFXGroupMaxAudibleBehavior(i);
		data["SoundGroupList"][i]["Name"] = soundGroupNames[i];
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
	}

	return true;
}

bool SFXManager::PostDecoding(const json& data)
{
	return true;
}
