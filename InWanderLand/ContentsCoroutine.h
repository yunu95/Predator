#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"

class ContentsCoroutine : public SingletonComponent<ContentsCoroutine>, public Component, public PermanentObservee
{
public:
    static std::weak_ptr<coroutine::Coroutine> StartRoutine(coroutine::Coroutine&& coroutine);
    virtual void OnContentsPlay() { GetComponent()->SetActive(true); GetComponent()->Start(); }
    virtual void OnContentsStop() { GetComponent()->SetActive(false); ClearCoroutines(); }
    virtual Component* GetComponent() override;
private:
    friend SingletonComponent<ContentsCoroutine>;
};