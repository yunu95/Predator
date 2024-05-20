#pragma once

#include "YunutyEngine.h"

class ContentsObservee
{
public:
	virtual ~ContentsObservee() = default;
	ContentsObservee();

	/// 해당 ContentsObservee 를 상속받는 구체화된 Component 에서
	/// 자기 자신을 반환하여 이를 통한 작업이 가능하도록 해야합니다.
	virtual Component* GetComponent() = 0;
};

