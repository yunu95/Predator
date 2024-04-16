/// 2024. 04. 15 김상준
/// 게임의 시작 및 종료 시, 통일된 동작을 할 수 있도록
/// 제공하는 Component 입니다.
/// Editor 의 Play Mode 를 위해서 사용합니다.

#pragma once

#include <unordered_set>

namespace application
{
	class PlayableComponent
	{
	public:
		PlayableComponent();
		virtual ~PlayableComponent();

		static void PreMapLoadAll();
		static void PostMapLoadAll();

		static void OnGameStartAll();
		static void OnGamePauseAll();
		static void OnGameResumeAll();
		static void OnGameStopAll();

	protected:
		/// Editor 에서 Map 을 로드하게 되었을 때의 동작을 정의합니다.
		/// Clear 등 리소스를 정리하는 등의 동작이 이에 해당합니다.
		virtual void PreMapLoad() {};

		/// Editor 에서 Map 을 로드한 직후에 수행할 동작을 정의합니다.
		virtual void PostMapLoad() {};

		/// 게임 시작 시에 호출될 함수로,
		/// 해당 함수를 override 하여 동작을 정의합니다.
		virtual void OnGameStart() {};

		/// 게임 일시 정지 시에 호출될 함수로,
		/// 해당 함수를 override 하여 동작을 정의합니다.
		virtual void OnGamePause() {};

		/// 게임 재시작 시에 호출될 함수로,
		/// 해당 함수를 override 하여 동작을 정의합니다.
		virtual void OnGameResume() {};

		/// 게임 종료 시에 호출될 함수로,
		/// 해당 함수를 override 하여 동작을 정의합니다.
		virtual void OnGameStop() {};

	private:
		static std::unordered_set<PlayableComponent*> list;
	};
}

