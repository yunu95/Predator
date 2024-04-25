/// 2024. 04. 17 김상준
/// Cinematic 에 관련된 Action 목록입니다. 


#pragma once

#include "IAction.h"

namespace application
{
	/// CinematicMode 전환 Action 입니다.
	class Action_CinematicModeChange
		: public IAction
	{
	public:
		DEFINE_ACTION(CinematicModeChange)

		virtual CoroutineObject<void> DoAction() override;

		void SetCinematicMode(bool cinematicMode);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		bool cinematicMode = false;
	};

	enum class FadeDirection
	{
		RIGHT,
		LEFT,
		UP,
		DOWN,
	};

	/// FadeIn 효과입니다.
	class Action_CinematicFadeIn
		: public IAction
	{
	public:
		DEFINE_ACTION(CinematicFadeIn)

		virtual CoroutineObject<void> DoAction() override;

		void SetFadeTime(float fadeTime);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		float fadeTime = 1;
	};

	/// FadeOut 효과입니다.
	class Action_CinematicFadeOut
		: public IAction
	{
	public:
		DEFINE_ACTION(CinematicFadeOut)

		virtual CoroutineObject<void> DoAction() override;

		void SetFadeDirection(FadeDirection direction);

		void SetFadeTime(float fadeTime);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		FadeDirection direction = FadeDirection::RIGHT;
		float fadeTime = 1;
	};
}
