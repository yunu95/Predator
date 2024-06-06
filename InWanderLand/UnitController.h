#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"
class Unit;

class UnitController : public Component, protected PermanentObservee
{
public:
    virtual void RegisterUnit(std::weak_ptr<Unit> unit) = 0;
    virtual void UnRegisterUnit(std::weak_ptr<Unit> unit) = 0;
    virtual Component* GetComponent() override { return this; }
};