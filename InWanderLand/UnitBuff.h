#include "YunutyEngine.h"

class Unit;
class UIElement;
class UnitBuff
{
public:
    std::weak_ptr<Unit> owner;
    std::weak_ptr<UIElement> buffIcon;
    virtual coroutine::Coroutine operator()() = 0;
};