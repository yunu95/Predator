#pragma once
#include "Object.h"
#include "Component.h"
#include "Rect.h"
#include "YunuGraphicsInterface.h"
#include "Vector3.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine::graphics
{
	class YunutyCycle;

	/// Animator 에 연동되는 AnimationEvent 를 생성하는 Controller 입니다.
	/// EventBinding 함수가 호출될 때, 동작할 callBack 함수를 등록하여 사용합니다.
	class YUNUTY_API AnimationEventController : public Component
	{
	public:
		AnimationEventController() = default;
		virtual ~AnimationEventController() {};
		
		/// 사전에 정의된 규칙에 맞게 Animator 에 AnimationEvent 를 등록하는 함수입니다.
		/// 규칙은 자유롭게 구성하여 callBack 으로 등록합니다.
		void EventBinding();

		/// EventBinding() 에서 호출할 callBack 을 등록합니다.
		void RegisterEventBindingCallBack(const std::function<void()>& callBack);

	private:
		std::function<void()> eventBindingCallBack = std::function<void()>();
	};
}
