#pragma once
#include "YunutyEngine.h"
#include "Unit.h"

class DummyComponent : public Component
{
public:
	Unit* m_pairUnit;
};