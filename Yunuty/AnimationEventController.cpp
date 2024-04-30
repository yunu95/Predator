#include "YunutyEngine.h"
#include "AnimationEventController.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

void AnimationEventController::EventBinding()
{
	eventBindingCallBack();
}

void AnimationEventController::RegisterEventBindingCallBack(const std::function<void()>& callBack)
{
	eventBindingCallBack = callBack;
}
