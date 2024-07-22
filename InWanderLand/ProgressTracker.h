/// 2024. 07. 17 김상준
/// 게임 진행 상황에 대해 기록하는 class 입니다.

#pragma once

#include <unordered_map>
#include <functional>

namespace application
{
	class ProgressTracker
	{
	public:
		ProgressTracker();
		virtual ~ProgressTracker();

		// Game 을 재시작하게 될 경우,
		// 초기화 세팅을 하기 위한 함수입니다.
		virtual void ProgressInitialize() {}

		// Game 진행 도중에 호출되어 현재의 상태를 저장하여
		// 추후에 Recovery 에서 사용할 수 있도록 개별적으로 적합한
		// Save Data 를 생성하는 함수입니다.
		virtual void CurrentProgressSave() {}

		// Game 진행 도중에 호출되어 저장되었던 상태를 통해 Load 하는 함수입니다.
		// Callback 지원을 위해 DoRecoveryCallbacks 함수를 호출해야합니다.
		virtual void Recovery() {}

		// Recovery 로직 후에 반영되어야 할 로직을 적용하는 함수입니다.
		virtual void PostRecovery() {}

		// Recovery 시에 혹시 모를 Callback 요청을 위해 지원하는 함수입니다.
		void DoRecoveryCallbacks();
		unsigned long long AddRecoveryCallback(const std::function<void()>& func);
		bool RemoveRecoveryCallback(unsigned long long funcIdx);

	protected:
		std::unordered_map<unsigned long long, std::function<void()>> recoveryCallbacks = std::unordered_map<unsigned long long, std::function<void()>>();
		unsigned long long callbackIndex = 0;
	};
}
