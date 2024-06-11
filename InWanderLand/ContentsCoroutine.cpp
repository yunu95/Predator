#include "ContentsCoroutine.h"

void ContentsCoroutine::StartRoutine(coroutine::Coroutine&& coroutine)
{
    Instance().StartCoroutine(std::move(coroutine));
}

Component* ContentsCoroutine::GetComponent()
{
    return this;
}
