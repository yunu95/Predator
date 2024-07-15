#include "YunutyEngine.h"
#include "SoundSystem.h"
#include "fmod.h"
#include "SoundChannel.h"
#include "fmod.hpp"
#include <iostream>
#include <algorithm>

using namespace FMOD;

SoundSystem* SoundSystem::soundInstance = nullptr;
void yunutyEngine::SoundSystem::Update()
{
    FMOD_VECTOR listenerPos = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR listenerForward = { 0.0f, 0.0f, 1.0f };
    FMOD_VECTOR listenerUp = { 0.0f, 1.0f, 0.0f };

    if (graphics::Camera::GetMainCamera())
    {
        auto camTransform = graphics::Camera::GetMainCamera()->GetTransform();
        auto worldPos = camTransform->GetWorldPosition();
        auto camForward = camTransform->GetWorldRotation().Forward();
        auto camUp = camTransform->GetWorldRotation().Up();
        
        listenerPos.x = worldPos.x;
        listenerPos.y = worldPos.y;
        listenerPos.z = worldPos.z;
        listenerForward.x = camForward.x;
        listenerForward.y = camForward.y;
        listenerForward.z = camForward.z;
        listenerUp.x = camUp.x;
        listenerUp.y = camUp.y;
        listenerUp.z = camUp.z;
    }

    auto soundSystemInstance = SingleInstance();
    soundSystemInstance->fmodSystem->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
    soundSystemInstance->fmodSystem->set3DSettings(1.0f, 1.0f, soundSystemInstance->rolloffScale);

    static std::vector<unsigned long long> sortedSoundGroups;
    static std::unordered_map<unsigned long long, bool> flagsMap;

    /// pair[FadingOut / FadingIn]
    static std::unordered_map<unsigned long long, std::pair<bool, bool>> fadingMap;
    sortedSoundGroups.clear();
    sortedSoundGroups.reserve(16);
    for (auto& [idx, soundGroup] : soundSystemInstance->soundGroups)
    {
        sortedSoundGroups.push_back(idx);
        if (!flagsMap.contains(idx))
        {
            flagsMap[idx] = false;
        }
        if (!fadingMap.contains(idx))
        {
            fadingMap[idx] = std::make_pair(false, false);
        }
    }

    std::sort(sortedSoundGroups.begin(), sortedSoundGroups.end(), [=](const unsigned long long& left, const unsigned long long& right)
        { 
            return soundSystemInstance->soundGroupPriorityMap[left] < soundSystemInstance->soundGroupPriorityMap[right]; 
        });

    static auto soundDown = [=](const unsigned long long idxDown)
        {
            static double localTimer = 0;
            float volume = 1.0f;
            GetSoundGroup(idxDown)->getVolume(&volume);

            if (volume <= GetSoundGroupVolume(idxDown) * GetSoundGroupPriorityFadeRatio(idxDown) + 0.0000001f)
            {
                GetSoundGroup(idxDown)->setVolume(GetSoundGroupVolume(idxDown) * GetSoundGroupPriorityFadeRatio(idxDown));
                fadingMap[idxDown].first = false;
                return;
            }
            else if (volume == GetSoundGroupVolume(idxDown))
            {
                localTimer = 0;
            }

            if (GetSoundGroupPriorityFadeOutTime(idxDown) == 0)
            {
                GetSoundGroup(idxDown)->setVolume(GetSoundGroupVolume(idxDown) * GetSoundGroupPriorityFadeRatio(idxDown));
            }
            else
            {
                localTimer += Time::GetDeltaTimeUnscaled() / GetSoundGroupPriorityFadeOutTime(idxDown);
                if (localTimer > 1)
                {
                    localTimer = 1;
                }
                GetSoundGroup(idxDown)->setVolume(math::LerpF(GetSoundGroupVolume(idxDown), GetSoundGroupVolume(idxDown) * GetSoundGroupPriorityFadeRatio(idxDown), localTimer));
            }
            return;
        };

    static auto soundUp = [=](const unsigned long long idxUp)
        {
            static double localTimer = 0;
            float volume = 1.0f;
            GetSoundGroup(idxUp)->getVolume(&volume);

            if (volume >= GetSoundGroupVolume(idxUp) - 0.0000001f)
            {
                GetSoundGroup(idxUp)->setVolume(GetSoundGroupVolume(idxUp));
                fadingMap[idxUp].second = false;
                return;
            }
            else if (volume <= GetSoundGroupVolume(idxUp) * GetSoundGroupPriorityFadeRatio(idxUp) + 0.0000001f)
            {
                localTimer = 0;
            }

            if (GetSoundGroupPriorityFadeInTime(idxUp) == 0)
            {
                GetSoundGroup(idxUp)->setVolume(GetSoundGroupVolume(idxUp));
            }
            else
            {
                localTimer += Time::GetDeltaTimeUnscaled() / GetSoundGroupPriorityFadeInTime(idxUp);
                if (localTimer > 1)
                {
                    localTimer = 1;
                }
                GetSoundGroup(idxUp)->setVolume(math::LerpF(GetSoundGroupVolume(idxUp) * GetSoundGroupPriorityFadeRatio(idxUp), GetSoundGroupVolume(idxUp), localTimer));
            }
            return;
        };

    int currentItr = 0;
    for (auto& sIdx : sortedSoundGroups)
    {
        auto& flag = flagsMap[sortedSoundGroups[currentItr]];
        flag = false;
        int numPlaying = 0;
        int priority = GetSoundGroupPriority(sIdx);
        GetSoundGroup(sIdx)->getNumPlaying(&numPlaying);
        flag |= numPlaying;

        int itr = 0;
        while (true)
        {
            if (itr == currentItr)
            {
                break;
            }

            if (GetSoundGroupPriority(sortedSoundGroups[itr]) != priority)
            {
                flag |= flagsMap[sortedSoundGroups[itr]];
                if (flag)
                {
                    break;
                }
            }
            itr++;
        }

        if (!GetSoundGroupPriorityFlag(sIdx))
        {
            flag = false;
        }

        if (currentItr != 0)
        {
            float volume = 1.0f;
            GetSoundGroup(sIdx)->getVolume(&volume);
            if (flag)
            {
                if (!fadingMap[sIdx].second && volume == GetSoundGroupVolume(sIdx))
                {
                    fadingMap[sIdx].first = true;
                }
            }
            else
            {
                if (!fadingMap[sIdx].first &&volume <= GetSoundGroupVolume(sIdx) * GetSoundGroupPriorityFadeRatio(sIdx) + 0.0000001f)
                {
                    fadingMap[sIdx].second = true;
                }
            }

            if (fadingMap[sIdx].first)
            {
                soundDown(sIdx);
            }
            else if (fadingMap[sIdx].second)
            {
                soundUp(sIdx);
            }
        }

        currentItr++;
    }

    soundSystemInstance->fmodSystem->update();
}
SoundChannel yunutyEngine::SoundSystem::PlaySoundfile(string soundPath)
{
    return SingleInstance()->mPlaySound(soundPath);
}
SoundChannel yunutyEngine::SoundSystem::PlaySoundfile3D(string soundPath, Vector3d worldPosition, float maxDistance)
{
    return SingleInstance()->mPlay3DSound(soundPath, worldPosition, maxDistance);
}
void yunutyEngine::SoundSystem::StopSound(double fadeLength)
{
    SoundSystem::SingleInstance()->mStopSound(fadeLength);
}
SoundSystem* yunutyEngine::SoundSystem::SingleInstance()
{
    if (!soundInstance)
        soundInstance = new SoundSystem();
    return soundInstance;
}
SoundSystem::SoundSystem()
{
    FMOD::System_Create(&fmodSystem);
    fmodSystem->init(128 + 1, FMOD_INIT_NORMAL, extradriverdata);
    fmodSystem->getMasterSoundGroup(&soundGroups[0]);
    soundGroupVolumes[0] = 1.0f;
    soundGroupPriorityMap[0] = 128;
    soundGroupPriorityFlagMap[0] = false;
    soundGroupPriorityFadeRatioMap[0] = 0.5f;
    soundGroupPriorityFadeOutTimeMap[0] = 1.0f;
    soundGroupPriorityFadeInTimeMap[0] = 1.0f;
}
SoundSystem::~SoundSystem()
{
    fmodSystem->release();
}
SoundChannel yunutyEngine::SoundSystem::mPlaySound(string soundPath)
{
    if (!mIsSoundLoaded(soundPath))
    {
        mLoadSound(soundPath);
    }
    for (int i = 0; i < 128; i++)
    {
        bool isPlaying = false;
        lastChannelIndex = (lastChannelIndex + 1) % 128;
        if (channels[lastChannelIndex])
        {
            channels[lastChannelIndex]->isPlaying(&isPlaying);
        }

        if (!isPlaying)
        {
            fmodSystem->playSound(sounds[soundPath], 0, false, &channels[lastChannelIndex]);
            channels[lastChannelIndex]->setPriority(soundPriorityMap[sounds[soundPath]]);
            SetSFXVolume(sfxVolume);
            break;
        }
    }
    return SoundChannel(channels[lastChannelIndex]);
}
SoundChannel yunutyEngine::SoundSystem::mPlay3DSound(string soundPath, Vector3d worldPosition, float maxDistance)
{
    if (!mIs3DSoundLoaded(soundPath))
    {
        mLoad3DSound(soundPath);
    }
    for (int i = 0; i < 128; i++)
    {
        bool isPlaying = false;
        lastChannelIndex = (lastChannelIndex + 1) % 128;
        if (channels[lastChannelIndex])
        {
            channels[lastChannelIndex]->isPlaying(&isPlaying);
        }

        if (!isPlaying)
        {
            FMOD_VECTOR position = { worldPosition.x, worldPosition.y, worldPosition.z };
            FMOD_VECTOR velocity = { 0,0,0 };

            fmodSystem->playSound(sounds3D[soundPath], 0, false, &channels[lastChannelIndex]);
            channels[lastChannelIndex]->set3DAttributes(&position, &velocity);
            channels[lastChannelIndex]->set3DMinMaxDistance(1, maxDistance);
            channels[lastChannelIndex]->setPriority(soundPriorityMap[sounds3D[soundPath]]);
            SetSFXVolume(sfxVolume);
            break;
        }
    }
    return SoundChannel(channels[lastChannelIndex]);
}
void yunutyEngine::SoundSystem::mStopSound(double fadeLength)
{
    for (int i = 0; i < 128; i++)
    {
        if (channels[i])
        {
            channels[i]->stop();
        }
    }
}
bool yunutyEngine::SoundSystem::mLoad3DSound(string soundPath)
{
    if (sounds3D.find(soundPath) == sounds3D.end())
    {
        fmodSystem->createSound(soundPath.c_str(), FMOD_3D | FMOD_LOOP_OFF, 0, &sounds3D[soundPath]);
        loadedSounds3D.insert(soundPath);
        sounds3D[soundPath]->setSoundGroup(GetSoundGroup(0));
        soundGroupIndexMap[sounds3D[soundPath]] = 0;
        soundPriorityMap[sounds3D[soundPath]] = 128;
    }
    return true;
}
bool yunutyEngine::SoundSystem::mLoadSound(string soundPath)
{
    if (sounds.find(soundPath) == sounds.end())
    {
        fmodSystem->createSound(soundPath.c_str(), FMOD_LOOP_OFF, 0, &sounds[soundPath]);
        loadedSounds.insert(soundPath);
        sounds[soundPath]->setSoundGroup(GetSoundGroup(0));
        soundGroupIndexMap[sounds[soundPath]] = 0;
        soundPriorityMap[sounds[soundPath]] = 128;
    }
    return true;
}
bool yunutyEngine::SoundSystem::mIsSoundLoaded(string soundPath)
{
    return loadedSounds.find(soundPath) != loadedSounds.end();
}
bool yunutyEngine::SoundSystem::mIs3DSoundLoaded(string soundPath)
{
    return loadedSounds3D.find(soundPath) != loadedSounds3D.end();
}
const unordered_set<string>& yunutyEngine::SoundSystem::mGetLoadedSoundsList()
{
    return loadedSounds;
}
const unordered_set<string>& yunutyEngine::SoundSystem::mGetLoaded3DSoundsList()
{
    return loadedSounds3D;
}
void yunutyEngine::SoundSystem::PlayMusic(string soundPath)
{
    return SoundSystem::SingleInstance()->mPlayMusic(soundPath);
}
void yunutyEngine::SoundSystem::PauseMusic()
{
    return SoundSystem::SingleInstance()->mPauseMusic();
}
void yunutyEngine::SoundSystem::UnpauseMusic()
{
    return SoundSystem::SingleInstance()->mContinueMusic();
}
void yunutyEngine::SoundSystem::StopMusic(double fadeLength)
{
    SoundSystem::SingleInstance()->mStopMusic(fadeLength);
}
bool yunutyEngine::SoundSystem::LoadSound(string soundPath)
{
    return SingleInstance()->mLoadSound(soundPath);
}
bool yunutyEngine::SoundSystem::Load3DSound(string soundPath)
{
    return SingleInstance()->mLoad3DSound(soundPath);
}
bool yunutyEngine::SoundSystem::IsSoundLoaded(string soundPath)
{
    return SingleInstance()->mIsSoundLoaded(soundPath);
}
bool yunutyEngine::SoundSystem::Is3DSoundLoaded(string soundPath)
{
    return SingleInstance()->mIs3DSoundLoaded(soundPath);
}
const unordered_set<string>& yunutyEngine::SoundSystem::GetLoadedSoundsList()
{
    return SingleInstance()->mGetLoadedSoundsList();
}
const unordered_set<string>& yunutyEngine::SoundSystem::GetLoaded3DSoundsList()
{
    return SingleInstance()->mGetLoaded3DSoundsList();
}
void yunutyEngine::SoundSystem::SetMusicVolume(float volume)
{
    SingleInstance()->musicVolume = volume;
    if (soundInstance->bgmChannel)
    {
        soundInstance->bgmChannel->setVolume(volume);
    }
}
float yunutyEngine::SoundSystem::GetMusicVolume()
{
    return SingleInstance()->musicVolume;
}
void yunutyEngine::SoundSystem::SetSFXVolume(float volume)
{
    /// channel 의 Priority 는 SoundGroup 에서 영향을 주지 않아,
    /// Priority 로 Volume 을 미세하게 조작하여 SoundGroup 로직에 반영할 수 있도록 합니다.
    
    SingleInstance()->sfxVolume = volume;
    for (int i = 0; i < 128; i++)
    {
        if (soundInstance->channels[i])
        {
            float instanceVolume = volume;
            int priority = 0;
            soundInstance->channels[i]->getPriority(&priority);
            instanceVolume -= (priority * 0.0000001f);
            soundInstance->channels[i]->setVolume(instanceVolume);
        }
    }
}
float yunutyEngine::SoundSystem::GetSFXVolume()
{
    return SingleInstance()->sfxVolume;
}
bool yunutyEngine::SoundSystem::SetSoundPriority(string soundPath, int priority)
{
    auto soundSystem = SingleInstance();
    if (soundSystem->sounds.contains(soundPath))
    {
        soundSystem->soundPriorityMap[soundSystem->sounds[soundPath]] = priority;
        return true;
    }
    return false;
}
int yunutyEngine::SoundSystem::GetSoundPriority(string soundPath)
{
    int finalPriority = 0;
    auto soundSystem = SingleInstance();
    if (soundSystem->sounds.contains(soundPath))
    {
        finalPriority = soundSystem->soundPriorityMap[soundSystem->sounds[soundPath]];
    }
    return finalPriority;
}
bool yunutyEngine::SoundSystem::Set3DSoundPriority(string soundPath, int priority)
{
    auto soundSystem = SingleInstance();
    if (soundSystem->sounds3D.contains(soundPath))
    {
        soundSystem->soundPriorityMap[soundSystem->sounds3D[soundPath]] = priority;
        return true;
    }
    return false;
}
int yunutyEngine::SoundSystem::Get3DSoundPriority(string soundPath)
{
    int finalPriority = 0;
    auto soundSystem = SingleInstance();
    if (soundSystem->sounds3D.contains(soundPath))
    {
        finalPriority = soundSystem->soundPriorityMap[soundSystem->sounds3D[soundPath]];
    }
    return finalPriority;
}
bool yunutyEngine::SoundSystem::SettingSoundGroupOfSound(string soundPath, unsigned long long groupIndex)
{
    auto soundSystem = SingleInstance();
    if (!soundSystem->mIsSoundLoaded(soundPath))
    {
        LoadSound(soundPath);
    }
    soundSystem->mCreateSoundGroup(groupIndex);
    soundSystem->sounds[soundPath]->setSoundGroup(GetSoundGroup(groupIndex));
    soundSystem->soundGroupIndexMap[soundSystem->sounds[soundPath]] = groupIndex;
    return true;
}
unsigned long long yunutyEngine::SoundSystem::GetSoundGroupIndexOfSound(string soundPath)
{
    auto soundSystem = SingleInstance();
    if (!soundSystem->mIsSoundLoaded(soundPath))
    {
        LoadSound(soundPath);
    }
    return soundSystem->soundGroupIndexMap[soundSystem->sounds[soundPath]];
}
bool yunutyEngine::SoundSystem::SettingSoundGroupOf3DSound(string soundPath, unsigned long long groupIndex)
{
    auto soundSystem = SingleInstance();
    if (!soundSystem->mIs3DSoundLoaded(soundPath))
    {
        Load3DSound(soundPath);
    }
    soundSystem->mCreateSoundGroup(groupIndex);
    soundSystem->sounds3D[soundPath]->setSoundGroup(GetSoundGroup(groupIndex));
    soundSystem->soundGroupIndexMap[soundSystem->sounds3D[soundPath]] = groupIndex;
    return true;
}
unsigned long long yunutyEngine::SoundSystem::GetSoundGroupIndexOf3DSound(string soundPath)
{
    auto soundSystem = SingleInstance();
    if (!soundSystem->mIs3DSoundLoaded(soundPath))
    {
        Load3DSound(soundPath);
    }
    return soundSystem->soundGroupIndexMap[soundSystem->sounds3D[soundPath]];
}
bool yunutyEngine::SoundSystem::CreateSoundGroup(unsigned long long groupIndex, string name)
{
    return SingleInstance()->mCreateSoundGroup(groupIndex, name);
}
bool yunutyEngine::SoundSystem::EraseSoundGroup(unsigned long long groupIndex)
{
    auto soundSystem = SingleInstance();
    if (groupIndex == 0 || !soundSystem->soundGroups.contains(groupIndex))
    {
        return false;
    }

    for (auto& [sound, idx] : soundSystem->soundGroupIndexMap)
    {
        if (idx == groupIndex)
        {
            sound->setSoundGroup(GetSoundGroup(0));
            idx = 0;
        }
    }

    soundSystem->soundGroups[groupIndex]->release();
    soundSystem->soundGroups.erase(groupIndex);
    return true;
}
FMOD::SoundGroup* yunutyEngine::SoundSystem::GetSoundGroup(unsigned long long groupIndex)
{
    auto soundSystem = SingleInstance(); 
    if (soundSystem->soundGroups.contains(groupIndex))
    {
        return soundSystem->soundGroups[groupIndex];
    }
    return nullptr;
}
int yunutyEngine::SoundSystem::GetSoundGroupUseCount(unsigned long long groupIndex)
{
    auto soundSystem = SingleInstance();
    if (soundSystem->soundGroups.contains(groupIndex))
    {
        int count = -1;
        soundSystem->soundGroups[groupIndex]->getNumSounds(&count);
        return count;
    }
    return -1;
}
bool yunutyEngine::SoundSystem::SetSoundGroupVolume(unsigned long long groupIndex, float volume)
{
    auto soundSystem = SingleInstance();
    if (auto soundGroup = GetSoundGroup(groupIndex))
    {
        soundSystem->soundGroupVolumes[groupIndex] = volume;
        soundGroup->setVolume(volume);
        return true;
    }
    return false;
}
float yunutyEngine::SoundSystem::GetSoundGroupVolume(unsigned long long groupIndex)
{
    auto soundSystem = SingleInstance();
    float finalVolume = 0.0f;
    if (auto soundGroup = GetSoundGroup(groupIndex))
    {
        finalVolume = soundSystem->soundGroupVolumes[groupIndex];
    }
    return finalVolume;
}
bool yunutyEngine::SoundSystem::SetSoundGroupMaxAudible(unsigned long long groupIndex, int audible)
{
    if (auto soundGroup = GetSoundGroup(groupIndex))
    {
        soundGroup->setMaxAudible(audible);
        return true;
    }
    return false;
}
int yunutyEngine::SoundSystem::GetSoundGroupMaxAudible(unsigned long long groupIndex)
{
    int finalAudible = 0;
    if (auto soundGroup = GetSoundGroup(groupIndex))
    {
        soundGroup->getMaxAudible(&finalAudible);
    }
    return finalAudible;
}
bool yunutyEngine::SoundSystem::SetSoundGroupMaxAudibleBehavior(unsigned long long groupIndex, SOUNDGROUP_BEHAVIOR behavior)
{
    if (auto soundGroup = GetSoundGroup(groupIndex))
    {
        soundGroup->setMaxAudibleBehavior((FMOD_SOUNDGROUP_BEHAVIOR)behavior);
        return true;
    }
    return false;
}
yunutyEngine::SoundSystem::SOUNDGROUP_BEHAVIOR yunutyEngine::SoundSystem::GetSoundGroupMaxAudibleBehavior(unsigned long long groupIndex)
{
    FMOD_SOUNDGROUP_BEHAVIOR finalBehavior = FMOD_SOUNDGROUP_BEHAVIOR::FMOD_SOUNDGROUP_BEHAVIOR_FAIL;
    if (auto soundGroup = GetSoundGroup(groupIndex))
    {
        soundGroup->getMaxAudibleBehavior(&finalBehavior);
    }
    return (yunutyEngine::SoundSystem::SOUNDGROUP_BEHAVIOR)finalBehavior;
}
void yunutyEngine::SoundSystem::Set3DRolloffScale(float rolloffScale)
{
    SingleInstance()->rolloffScale = rolloffScale;
}
float yunutyEngine::SoundSystem::Get3DRolloffScale()
{
    return SingleInstance()->rolloffScale;
}
void yunutyEngine::SoundSystem::SetSoundGroupPriority(unsigned long long groupIndex, int priority)
{
    SingleInstance()->soundGroupPriorityMap[groupIndex] = priority;
}
int yunutyEngine::SoundSystem::GetSoundGroupPriority(unsigned long long groupIndex)
{
    return SingleInstance()->soundGroupPriorityMap[groupIndex];
}
void yunutyEngine::SoundSystem::SetSoundGroupPriorityFlag(unsigned long long groupIndex, bool flag)
{
    SingleInstance()->soundGroupPriorityFlagMap[groupIndex] = flag;
}
bool yunutyEngine::SoundSystem::GetSoundGroupPriorityFlag(unsigned long long groupIndex)
{
    return SingleInstance()->soundGroupPriorityFlagMap[groupIndex];
}
void yunutyEngine::SoundSystem::SetSoundGroupPriorityFadeRatio(unsigned long long groupIndex, float ratio)
{
    if (ratio >= 1.0f)
    {
        ratio = 0.999f;
    }
    SingleInstance()->soundGroupPriorityFadeRatioMap[groupIndex] = ratio;
}
float yunutyEngine::SoundSystem::GetSoundGroupPriorityFadeRatio(unsigned long long groupIndex)
{
    return SingleInstance()->soundGroupPriorityFadeRatioMap[groupIndex];
}
void yunutyEngine::SoundSystem::SetSoundGroupPriorityFadeOutTime(unsigned long long groupIndex, float time)
{
    SingleInstance()->soundGroupPriorityFadeOutTimeMap[groupIndex] = time;
}
float yunutyEngine::SoundSystem::GetSoundGroupPriorityFadeOutTime(unsigned long long groupIndex)
{
    return SingleInstance()->soundGroupPriorityFadeOutTimeMap[groupIndex];
}
void yunutyEngine::SoundSystem::SetSoundGroupPriorityFadeInTime(unsigned long long groupIndex, float time)
{
    SingleInstance()->soundGroupPriorityFadeInTimeMap[groupIndex] = time;
}
float yunutyEngine::SoundSystem::GetSoundGroupPriorityFadeInTime(unsigned long long groupIndex)
{
    return SingleInstance()->soundGroupPriorityFadeInTimeMap[groupIndex];
}
void yunutyEngine::SoundSystem::mPlayMusic(string soundPath)
{
    if (sounds.find(soundPath) == sounds.end())
    {
        sounds[soundPath] = nullptr;
        fmodSystem->createSound(soundPath.c_str(), FMOD_LOOP_NORMAL, 0, &sounds[soundPath]);
    }
    bool isPlaying = false;

    if (bgmChannel)
    {
        bgmChannel->isPlaying(&isPlaying);
    }
    if (!isPlaying)
    {
        sounds[soundPath]->setMode(FMOD_LOOP_NORMAL);
        sounds[soundPath]->setLoopCount(-1);
        fmodSystem->playSound(sounds[soundPath], 0, false, &bgmChannel);
        SetMusicVolume(musicVolume);
    }
}
void yunutyEngine::SoundSystem::mPauseMusic()
{
    if (bgmChannel)
        bgmChannel->setPaused(true);
}
void yunutyEngine::SoundSystem::mContinueMusic()
{
    if (bgmChannel)
        bgmChannel->setPaused(false);
}
void yunutyEngine::SoundSystem::mStopMusic(double fadeLength)
{
    if (bgmChannel)
        bgmChannel->stop();
}

bool yunutyEngine::SoundSystem::mCreateSoundGroup(unsigned long long groupIndex, string name)
{
    if (soundGroups.contains(groupIndex))
    {
        return false;
    }

    fmodSystem->createSoundGroup(name.c_str(), &soundGroups[groupIndex]);
    soundGroupVolumes[groupIndex] = 1.0f;
    soundGroupPriorityMap[groupIndex] = 128;
    soundGroupPriorityFlagMap[groupIndex] = false;
    soundGroupPriorityFadeRatioMap[groupIndex] = 0.5f;
    soundGroupPriorityFadeOutTimeMap[groupIndex] = 1.0f;
    soundGroupPriorityFadeInTimeMap[groupIndex] = 1.0f;
    return true;
}
