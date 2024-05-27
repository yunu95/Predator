#include "YunutyEngine.h"

class Unit;
class UIElement;
class UnitBuff
{
private:
    std::weak_ptr<Unit> owner;
    std::weak_ptr<UIElement> buffIcon;
public:
    virtual coroutine::Coroutine operator()() = 0;
    friend Unit;
};