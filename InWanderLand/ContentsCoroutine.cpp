#include "ContentsCoroutine.h"

std::weak_ptr<coroutine::Coroutine> ContentsCoroutine::StartRoutine(coroutine::Coroutine&& coroutine)
{
    return Instance().StartCoroutine(std::move(coroutine));
}

Component* ContentsCoroutine::GetComponent()
{
    return this;
}
