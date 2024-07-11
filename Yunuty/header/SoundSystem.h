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
    class SoundGroup;
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
        enum class SOUNDGROUP_BEHAVIOR
        {
            FAIL,              
            MUTE,              
            STEALLOWEST,       
            MAX, 
        };

        static void Update();
        static SoundChannel PlaySoundfile(string soundPath);
        static SoundChannel PlaySoundfile3D(string soundPath, Vector3d worldPosition, float maxDistance = 50.0f);
        static void StopSound(double fadeLength = 0);
        // Music을 따로 채널에서 관리한다? 이건 찐빠같은 설계로, 복잡한 게임을 구현하기 위해서는 AudioSource 객체에서 채널 컨트롤이 가능하게 해야한다.
        static void PlayMusic(string soundPath);
        static void PauseMusic();
        static void UnpauseMusic();
        static void StopMusic(double fadeLength = 0);
        static bool LoadSound(string soundPath);
        static bool Load3DSound(string soundPath);
        static bool IsSoundLoaded(string soundPath);
        static bool Is3DSoundLoaded(string soundPath);
        static const unordered_set<string>& GetLoadedSoundsList();
        static const unordered_set<string>& GetLoaded3DSoundsList();
        static void SetMusicVolume(float volume);
        static float GetMusicVolume();
        static void SetSFXVolume(float volume);
        static float GetSFXVolume();
        static bool SetSoundPriority(string soundPath, int priority);
        static int GetSoundPriority(string soundPath);
        static bool Set3DSoundPriority(string soundPath, int priority);
        static int Get3DSoundPriority(string soundPath);
        static bool SettingSoundGroupOfSound(string soundPath, unsigned long long groupIndex);
        static unsigned long long GetSoundGroupIndexOfSound(string soundPath);
        static bool SettingSoundGroupOf3DSound(string soundPath, unsigned long long groupIndex);
        static unsigned long long GetSoundGroupIndexOf3DSound(string soundPath);
        static bool CreateSoundGroup(unsigned long long groupIndex, string name = "None");
        static bool EraseSoundGroup(unsigned long long groupIndex);
        static FMOD::SoundGroup* GetSoundGroup(unsigned long long groupIndex);
        static int GetSoundGroupUseCount(unsigned long long groupIndex);
        static bool SetSoundGroupVolume(unsigned long long groupIndex, float volume);
        static float GetSoundGroupVolume(unsigned long long groupIndex);
        static bool SetSoundGroupMaxAudible(unsigned long long groupIndex, int audible);
        static int GetSoundGroupMaxAudible(unsigned long long groupIndex);
        static bool SetSoundGroupMaxAudibleBehavior(unsigned long long groupIndex, SOUNDGROUP_BEHAVIOR behavior);
        static SOUNDGROUP_BEHAVIOR GetSoundGroupMaxAudibleBehavior(unsigned long long groupIndex);
        static void Set3DRolloffScale(float rolloffScale);
        static float Get3DRolloffScale();
    private:
        static SoundSystem* soundInstance;
        static SoundSystem* SingleInstance();
        SoundSystem();
        ~SoundSystem();
        SoundChannel mPlaySound(string soundPath);
        SoundChannel mPlay3DSound(string soundPath, Vector3d worldPosition, float maxDistance);
        void mStopSound(double fadeLength);
        bool mLoad3DSound(string soundPath);
        bool mLoadSound(string soundPath);
        bool mIsSoundLoaded(string soundPath);
        bool mIs3DSoundLoaded(string soundPath);
        const unordered_set<string>& mGetLoadedSoundsList();
        const unordered_set<string>& mGetLoaded3DSoundsList();
        // Music을 따로 채널에서 관리한다? 이건 찐빠같은 설계로, 복잡한 게임을 구현하기 위해서는 AudioSource 객체에서 채널 컨트롤이 가능하게 해야한다.
        void mPlayMusic(string soundPath);
        void mPauseMusic();
        void mContinueMusic();
        void mStopMusic(double fadeLength);
        bool mCreateSoundGroup(unsigned long long groupIndex, string name = "None");
        // fire and forget channels
        FMOD::Channel* channels[128] = { nullptr };
        FMOD::Channel* bgmChannel = nullptr;
        FMOD::System* fmodSystem = nullptr;
        // 0번 Index 는 masterSoundGroup 으로 사용하며, 기본적으로 생성합니다.
        unordered_map<unsigned long long, FMOD::SoundGroup*> soundGroups;
        int lastChannelIndex{ 0 };
        float musicVolume = 1.0f;
        float sfxVolume = 1.0f;
        void* extradriverdata = 0;
        unordered_set<string> loadedSounds;
        unordered_set<string> loadedSounds3D;
        unordered_map<string, FMOD::Sound*> sounds;
        unordered_map<string, FMOD::Sound*> sounds3D;
        unordered_map<FMOD::Sound*, unsigned long long> soundGroupIndexMap;
        unordered_map<FMOD::Sound*, int> soundPriorityMap;
        float rolloffScale = 1.0f;
    };
};
