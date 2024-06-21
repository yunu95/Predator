#include "UnitBuff.h"
#include "Unit.h"
#include "UIElement.h"
#include "InWanderLand.h"

void UnitBuff::Init(std::weak_ptr<Unit> owner)
{
    this->owner = owner;
    if (auto id = GetUIEnumID(); id != UIEnumID::None && !owner.lock()->unitStatusUI.expired())
        this->buffIcon1 = owner.lock()->unitStatusUI.lock()->GetLocalUIsByEnumID().at(id)->GetWeakPtr<UIElement>();
    if (auto id = GetUIEnumID(); id != UIEnumID::None && !owner.lock()->unitStatusPortraitUI.expired())
        this->buffIcon2 = owner.lock()->unitStatusPortraitUI.lock()->GetLocalUIsByEnumID().at(id)->GetWeakPtr<UIElement>();
    if (auto id = GetUIEnumID(); id != UIEnumID::None && !owner.lock()->unitStatusPortraitUI2.expired())
        this->buffIcon3 = owner.lock()->unitStatusPortraitUI2.lock()->GetLocalUIsByEnumID().at(id)->GetWeakPtr<UIElement>();
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
