#include "UnitBuffBlinded.h"
#include "InWanderLand.h"

UnitBuffType UnitBuffBlinded::GetBuffType()
{
    return UnitBuffType::Blinded;
}

UIEnumID UnitBuffBlinded::GetUIEnumID()
{
    return UIEnumID::CharInfo_Buff_Blinding;
}

void UnitBuffBlinded::OnStart()
{
    blindReference = owner.lock()->referenceBlindness.Acquire();
}
