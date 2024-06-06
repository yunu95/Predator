#include "UnitPool.h"
#include "UnitData.h"
#include "Unit.h"

std::weak_ptr<Unit> UnitPool::Borrow(application::editor::UnitData* data)
{
    if (!poolsByTemplate.contains(data->GetTemplateData()))
    {
        poolsByTemplate[data->GetTemplateData()] = std::make_shared<PoolByTemplate>();
        poolsByTemplate[data->GetTemplateData()]->templateData = data->GetTemplateData();
    }
    auto unit = poolsByTemplate[data->GetTemplateData()]->Borrow();
    unit.lock()->Summon(data);
    return unit;
}

void UnitPool::Return(std::weak_ptr<Unit> unit)
{
    unit.lock()->unitData->inGameUnit.reset();
    for (auto& each : unit.lock()->controllers)
    {
        each->UnRegisterUnit(unit);
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
