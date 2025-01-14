#include "InWanderLand.h"
#include "UnitPool.h"
#include "UnitData.h"
#include "Unit.h"

void UnitPool::Reset()
{
    poolsByTemplate.clear();
}

std::weak_ptr<Unit> UnitPool::Borrow(application::editor::UnitData* data)
{
    auto unit = Borrow(data->GetTemplateData());
    unit.lock()->unitTemplateData = data->GetTemplateData();
    unit.lock()->Summon(data);
    return unit;
}

std::weak_ptr<Unit> UnitPool::Borrow(application::editor::Unit_TemplateData* td, const Vector3d& position, float rotation)
{
    auto unit = Borrow(td);
    unit.lock()->unitTemplateData = td;
    unit.lock()->Summon(td, position, rotation);
    return unit;
}

std::weak_ptr<Unit> UnitPool::Borrow(application::editor::Unit_TemplateData* td, const Vector3d& position, const Quaternion& rotation)
{
    auto unit = Borrow(td);
    unit.lock()->unitTemplateData = td;
    unit.lock()->Summon(td, position, rotation);
    return unit;
}

std::weak_ptr<Unit> UnitPool::Borrow(application::editor::Unit_TemplateData* td)
{
    if (!poolsByTemplate.contains(td))
    {
        poolsByTemplate[td] = std::make_shared<PoolByTemplate>();
        poolsByTemplate[td]->templateData = td;
    }
    return poolsByTemplate[td]->Borrow();
}

void UnitPool::Return(std::weak_ptr<Unit> unit)
{
    if (unit.expired())
    {
        return;
    }
    for (auto& each : unit.lock()->controllers)
    {
        each->UnRegisterUnit(unit);
    }
    for (auto unitStatusUI : unit.lock()->unitStatusUIs)
    {
        if (!unitStatusUI.expired())
        {
            unitStatusUI.lock()->DisableElement();
        }
    }
    poolsByTemplate[&unit.lock()->GetUnitTemplateData()]->Return(unit);
}

void UnitPool::PoolByTemplate::ObjectInitializer(std::weak_ptr<Unit> unit)
{
    std::stringstream ss;
    ss << templateData->pod.skinnedFBXName << "_" << poolObjects.size();
    unit.lock()->name = ss.str();
    unit.lock()->Init(templateData);
}

void UnitPool::PoolByTemplate::OnBorrow(std::weak_ptr<Unit> unit)
{

}
