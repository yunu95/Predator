/// 2024. 04. 28 김상준
/// Animator 에 적절한 AnimationEvent 를 등록할 수 있도록 돕는 중간 데이터 양식들의 집합입니다.
/// 해당 양식들이 저장되고, 이를 해석하여 functor 가 등록됩니다.

#pragma once

#include "YunutyEngine.h"

#define GET_TYPE(Class) \
virtual AnimationEventType GetType() override \
{ \
	return AnimationEventType::##Class; \
}

namespace application
{
	enum class AnimationEventType
	{
		GameObject_ActivateEvent,
		GameObject_DisabledEvent,
		Sound_PlayOnceEvent,
		Sound_PlayLoopEvent
	};

	struct AnimationEvent
	{
		virtual ~AnimationEvent() = default;
		virtual AnimationEventType GetType() = 0;
	
		std::string fbxName;
		std::string animationName;
		float frame;
	};

	/// Object 의 GUID 를 통해 Binding 하면 좋을 것 같으나,
	/// 해당 내용이 Engine 에는 현재 반영되지 않은 것으로 보여
	/// 우선은 Object Name 으로 관리하도록 합니다.
	struct GameObject_ActivateEvent
		: public AnimationEvent
	{
		GET_TYPE(GameObject_ActivateEvent)

		std::string objName;
	};

	struct GameObject_DisabledEvent
		: public AnimationEvent
	{
		GET_TYPE(GameObject_DisabledEvent)

		std::string objName;
	};

	struct Sound_PlayOnceEvent
		: public AnimationEvent
	{
		GET_TYPE(Sound_PlayOnceEvent)

		std::string rscPath;
	};

	struct Sound_PlayLoopEvent
		: public AnimationEvent
	{
		GET_TYPE(Sound_PlayLoopEvent)

		std::string rscPath;
	};
}
