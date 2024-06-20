/// 2024. 04. 19 김상준
/// Tutorial 관련 처리를 위한 Singleton Manager 입니다.

#pragma once

#include "YunutyEngine.h"
#include "SingletonComponent.h"

#include "PlayableComponent.h"

#include "Unit.h"

namespace application
{
	class TutorialManager
		: public PlayableComponent, public Component, public SingletonComponent<TutorialManager>
	{
	public:
		virtual void SingletonInitializer() override;

		virtual void Update() override;

		virtual void PreMapLoad() override;
		virtual void OnGameStart() override;
		virtual void OnGameStop() override;

		bool IsTutorialMode() { return isTutorialMode; }
		void SetTutorialMode(bool mode);

	private:
		bool isTutorialMode = false;
	};
}

