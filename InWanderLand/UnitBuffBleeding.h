#include "UnitBuff.h"

class UnitBuffBleeding : public UnitBuff
{
private:
    virtual coroutine::Coroutine operator()() override;
    // 피해를 가하는 유닛
    std::weak_ptr<Unit> Inflictor;
    float duration;
    float damagePerSecond;
public:
    UnitBuffBleeding(std::weak_ptr<Unit> Inflictor, float duration, float damagePerSecond) :
        Inflictor(Inflictor), duration(duration), damagePerSecond(damagePerSecond) {}
};