#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"

/// <summary>
/// Observee를 상속받지 못하는 컴포넌트를 위한 클래스.
/// 해당 컴포넌트가 붙은 오브젝트에 이 컴포넌트를 붙여주자.
/// </summary>
class ObserveeHelper : public Component, public ContentsObservee
{
public:
	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

