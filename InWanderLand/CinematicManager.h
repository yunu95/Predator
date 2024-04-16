/// 2024. 04. 15 김상준
/// Cinematic 관련 처리를 위한 Singleton Manager 입니다.

#pragma once

#include "YunutyEngine.h"
#include "SingletonComponent.h"

#include "PlayableComponent.h"

namespace application
{
	class CinematicManager
		: public PlayableComponent, public Component, public SingletonComponent<CinematicManager>
	{
	public:
		virtual void SingletonInitializer() override;

		virtual void Update() override;

		virtual void PreMapLoad() override;
		virtual void OnGameStart() override;
		virtual void OnGameStop() override;

		bool IsCinematicMode() { return isCinematicMode; }
		void SetCinematicMode(bool isCinematicMode);

	private:
		bool isCinematicMode = false;
	};
}

