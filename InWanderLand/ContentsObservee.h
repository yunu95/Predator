#pragma once

#include "YunutyEngine.h"

class ContentsObservee
	: virtual public yunutyEngine::Component
{
public:
	virtual ~ContentsObservee() = default;
	ContentsObservee();
};

