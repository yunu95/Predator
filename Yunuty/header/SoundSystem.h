#include <iostream>
#include <unordered_map>
#include <unordered_set>

#pragma once
#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;

namespace FMOD
{
    class System;
    class Sound;
    class Channel;
}

// audio kinetic : wwise
// 위 솔루션은 층간 소음을 구현하는데에 도움이 된다.
// 근데 구독형이라 비싸다고 함.
namespace yunutyEngine
{
    class SoundChannel;
    class YUNUTY_API SoundSystem
    {
    public:
        static SoundChannel PlaySoundfile(string soundPath);
        static SoundChannel PlaySoundfile3D(string soundPath, Vector3d viewSpacePosition);
        // Music을 따로 채널에서 관리한다? 이건 찐빠같은 설계로, 복잡한 게임을 구현하기 위해서는 AudioSource 객체에서 채널 컨트롤이 가능하게 해야한다.
        static void PlayMusic(string soundPath);
        static void PauseMusic();
        static void UnpauseMusic();
        static void StopMusic(double fadeLength = 0);
        static bool LoadSound(string soundPath);
        static bool IsSoundLoaded(string soundPath);
        static const unordered_set<string>& GetLoadedSoundsList();
        static void SetMusicVolume(float volume);
        static float GetMusicVolume();
    private:
        static SoundSystem* soundInstance;
        static SoundSystem* SingleInstance();
        SoundSystem();
        ~SoundSystem();
        SoundChannel mPlaySound(string soundPath);
        bool mLoad3DSound(string soundPath);
        bool mLoadSound(string soundPath);
        bool mIsSoundLoaded(string soundPath);
        const unordered_set<string>& mGetLoadedSoundsList();
        // Music을 따로 채널에서 관리한다? 이건 찐빠같은 설계로, 복잡한 게임을 구현하기 위해서는 AudioSource 객체에서 채널 컨트롤이 가능하게 해야한다.
        void mPlayMusic(string soundPath);
        void mPauseMusic();
        void mContinueMusic();
        void mStopMusic(double fadeLength);
        // fire and forget channels
        FMOD::Channel* channels[64] = { nullptr };
        FMOD::Channel* bgmChannel = nullptr;
        FMOD::System* fmodSystem = nullptr;
        float musicVolume = 1.0f;
        void* extradriverdata = 0;
        unordered_set<string> loadedSounds;
        unordered_set<string> loadedSounds3D;
        unordered_map<string, FMOD::Sound*> sounds;
        unordered_map<string, FMOD::Sound*> sounds3D;
    };
};
