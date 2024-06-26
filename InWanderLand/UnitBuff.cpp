#include "UnitBuff.h"
#include "Unit.h"
#include "UIElement.h"
#include "InWanderLand.h"

void UnitBuff::Init(std::weak_ptr<Unit> owner)
{
    this->owner = owner;
    auto id = GetUIEnumID();
    if (id != UIEnumID::None)
    {
        if (auto ui = owner.lock()->unitStatusUI; !ui.expired() && ui.lock()->GetLocalUIsByEnumID().at(id))
            this->buffIcon1 = ui.lock()->GetLocalUIsByEnumID().at(id)->GetWeakPtr<UIElement>();
        if (auto ui = owner.lock()->unitStatusPortraitUI; !ui.expired() && ui.lock()->GetLocalUIsByEnumID().at(id))
            this->buffIcon2 = ui.lock()->GetLocalUIsByEnumID().at(id)->GetWeakPtr<UIElement>();
        if (auto ui = owner.lock()->unitStatusPortraitUI2; !ui.expired() && ui.lock()->GetLocalUIsByEnumID().at(id))
            this->buffIcon3 = ui.lock()->GetLocalUIsByEnumID().at(id)->GetWeakPtr<UIElement>();
    }
    if (!buffIcon1.expired())
        buffIcon1.lock()->EnableElement();
    if (!buffIcon2.expired())
        buffIcon2.lock()->EnableElement();
    if (!buffIcon3.expired())
        buffIcon3.lock()->EnableElement();
}

UnitBuff::~UnitBuff()
{
    if (!buffIcon1.expired())
        buffIcon1.lock()->DisableElement();
    if (!buffIcon2.expired())
        buffIcon2.lock()->DisableElement();
    if (!buffIcon3.expired())
        buffIcon3.lock()->DisableElement();
};
