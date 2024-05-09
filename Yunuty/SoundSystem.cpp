#include "YunutyEngine.h"
#include "SoundSystem.h"
#include "fmod.h"
#include "SoundChannel.h"
#include "fmod.hpp"
#include <iostream>

using namespace FMOD;

SoundSystem* SoundSystem::soundInstance = nullptr;
SoundChannel yunutyEngine::SoundSystem::PlaySoundfile(string soundPath)
{
    return SingleInstance()->mPlaySound(soundPath);
}
SoundChannel yunutyEngine::SoundSystem::PlaySoundfile3D(string soundPath, Vector3d worldPosition)
{
    return SingleInstance()->mPlaySound3D(soundPath, worldPosition);
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
    fmodSystem->init(64 + 1, FMOD_INIT_NORMAL, extradriverdata);
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
    for (int i = 0; i < 64; i++)
    {
        bool isPlaying;
        lastChannelIndex = (lastChannelIndex + 1) % 64;
        channels[lastChannelIndex]->isPlaying(&isPlaying);
        if (!isPlaying)
        {
            fmodSystem->playSound(sounds[soundPath], 0, false, &channels[lastChannelIndex]);
            break;
        }
    }
    return SoundChannel(channels[lastChannelIndex]);
}
SoundChannel yunutyEngine::SoundSystem::mPlaySound3D(string soundPath, Vector3d worldPosition)
{
    // 월드포지션을 뷰 포지션으로 바꾸는 코드
    auto viewPos = graphics::Camera::GetMainCamera()->GetGI().GetViewPos(worldPosition);
    if (!mIs3DSoundLoaded(soundPath))
    {
        mLoad3DSound(soundPath);
    }
    for (int i = 0; i < 64; i++)
    {
        bool isPlaying;
        lastChannelIndex = (lastChannelIndex + 1) % 64;
        channels[lastChannelIndex]->isPlaying(&isPlaying);

        if (!isPlaying)
        {
            FMOD_VECTOR position = { viewPos.x, viewPos.y, viewPos.z };
            FMOD_VECTOR velocity = { 0,0,0 };
            channels[lastChannelIndex]->set3DAttributes(&position, &velocity);
            channels[lastChannelIndex]->set3DMinMaxDistance(0, 50);

            fmodSystem->playSound(sounds3D[soundPath], 0, false, &channels[lastChannelIndex]);
            break;
        }
    }
    return SoundChannel(channels[lastChannelIndex]);
}
bool yunutyEngine::SoundSystem::mLoad3DSound(string soundPath)
{
    if (sounds3D.find(soundPath) == sounds3D.end())
    {
        fmodSystem->createSound(soundPath.c_str(), FMOD_3D, 0, &sounds3D[soundPath]);
        loadedSounds3D.insert(soundPath);
    }
    return true;
}
bool yunutyEngine::SoundSystem::mLoadSound(string soundPath)
{
    if (sounds.find(soundPath) == sounds.end())
    {
        fmodSystem->createSound(soundPath.c_str(), FMOD_LOOP_OFF, 0, &sounds[soundPath]);
        loadedSounds.insert(soundPath);
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
bool yunutyEngine::SoundSystem::IsSoundLoaded(string soundPath)
{
    return SingleInstance()->mIsSoundLoaded(soundPath);
}
const unordered_set<string>& yunutyEngine::SoundSystem::GetLoadedSoundsList()
{
    return SingleInstance()->mGetLoadedSoundsList();
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
