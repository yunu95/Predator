/// 2024. 06. 02 김상준
/// SFX 관리를 위해 만든 Manager
/// 우선은 마지막에 입력된 SFX 볼륨만 최고로 유지하고, 나머지는 줄이는 방식을 사용

#pragma once
#include "YunutyEngine.h"
#include "PlayableComponent.h"

class SFXManager
	: public SingletonClass<SFXManager>, public application::PlayableComponent
{
public:
	static void PlaySoundfile(string soundPath);
	static void PlaySoundfile3D(string soundPath, Vector3d worldPosition);
	static void SetSFXVolume(float volume);
	static float GetSFXVolume();

	virtual void OnGameStart() override;
	virtual void OnGameStop() override;


private:
	float localSFXVolume = 1.0f;
	float ratio = 10.f;
};
