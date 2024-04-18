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
		void SetFadeDirection(FadeDirection direction);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		float fadeTime = 0;
		FadeDirection direction = FadeDirection::RIGHT;
	};

	/// FadeOut 효과입니다.
	class Action_CinematicFadeOut
		: public IAction
	{
	public:
		DEFINE_ACTION(CinematicFadeOut)

		virtual CoroutineObject<void> DoAction() override;

		void SetFadeTime(float fadeTime);
		void SetFadeDirection(FadeDirection direction);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		float fadeTime = 0;
		FadeDirection direction = FadeDirection::RIGHT;
	};
}
