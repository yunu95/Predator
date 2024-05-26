#include "UnitPool.h"

std::weak_ptr<Unit> UnitPool::Borrow(application::editor::UnitData* data)
{
    return std::weak_ptr<Unit>();
}

void UnitPool::Return(std::weak_ptr<Unit>)
{
}

void UnitPool::PoolByTemplate::ObjectInitializer(std::weak_ptr<Unit> unit)
{
}

void UnitPool::PoolByTemplate::OnBorrow(std::weak_ptr<Unit> unit)
{
}
