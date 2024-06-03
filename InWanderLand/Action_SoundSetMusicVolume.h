/// 2024. 05. 17 이윤우
/// 일정 시간에 걸쳐 음악의 볼륨을 조절합니다.

#pragma once

#include "IAction.h"

namespace application
{
	class Action_SoundSetMusicVolume
		: public IAction
	{
	public:
		DEFINE_ACTION(SoundSetMusicVolume)

		virtual CoroutineObject<void> DoAction() override;

		void SetFadeTime(float fadeTime);
		void SetVolume(float volume);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		float fadeTime = 0;
		float volume = 1.0f;
	};
}
