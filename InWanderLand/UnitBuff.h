#pragma once
#include "YunutyEngine.h"
#include "UnitBuffType.h"
#include "UIEnumID.h"
#include "VFXAnimator.h"

class Unit;
class UIElement;
class ManagedFBX;
// 유닛에게 적용된 버프는 최초로 적용될 때 OnStart()가 호출되고, 만료할 때 OnEnd()가 호출된다.
// 타입이 같은 버프가 연속적으로 적용될 경우 이미 적용되어 있는 버프로부터 OnOverlap 함수가 호출된다.
class UnitBuff
    : public ITacticObject
{
private:
    std::weak_ptr<UIElement> buffIcon1;
    std::weak_ptr<UIElement> buffIcon2;
    void Init(std::weak_ptr<Unit> owner);
protected:
    std::weak_ptr<Unit> owner;
    std::weak_ptr<ManagedFBX> buffEffect;
    virtual coroutine::Coroutine EffectCoroutine() { co_return; }
public:
    float durationLeft = 1;
    virtual UnitBuffType GetBuffType() = 0;
    virtual UIEnumID GetUIEnumID() { return UIEnumID::None; };
    virtual void OnStart() {};
    virtual void OnUpdate() {};
    virtual void OnEnd() {};
    virtual void OnOverlap(UnitBuff&& overlapping)
    {
        if (durationLeft < overlapping.durationLeft)
            durationLeft = overlapping.durationLeft;
    };
    virtual ~UnitBuff();

    virtual void OnPause() {}
    virtual void OnResume() {}

    friend Unit;
};