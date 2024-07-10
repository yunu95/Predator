#include "DuplicatedUIPool.h"
#include "InWanderLand.h"
#include <DirectXMath.h>

// Borrow 함수는 먼저 해당 fbx 이름에 맞는 풀이 존재하는지 확인하고, 없으면 새로 만든다.
std::weak_ptr<ManagedDuplicatedUI> DuplicatedUIPool::Borrow(UIEnumID id)
{
    if (!poolsByID.contains(id))
    {
        auto pool = std::make_shared<PoolByEnumID>();
        pool->id = id;
        poolsByID[id] = pool;
    }

    auto wptr = poolsByID[id]->Borrow();
    borrowedList.insert(wptr.lock().get());
    wptr.lock()->uiElement->EnableElement();
    return wptr;
}

void DuplicatedUIPool::Return(std::weak_ptr<ManagedDuplicatedUI> ui)
{
    if (ui.expired())
    {
        return;
    }
    borrowedList.erase(ui.lock().get());
    poolsByID.at((UIEnumID)ui.lock()->uiElement->importedUIData.enumID)->Return(ui);
}

void DuplicatedUIPool::OnContentsStop()
{
    for (auto eachPool : poolsByID)
    {
        eachPool.second->Clear();
    }
    poolsByID.clear();
    poolsByUI.clear();
    borrowedList.clear();
    GetComponent()->SetActive(false);
}

GameObject* DuplicatedUIPool::PoolByEnumID::GameObjectInitializer()
{
    return UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(id)).lock()->GetGameObject();
}
void DuplicatedUIPool::PoolByEnumID::ObjectInitializer(std::weak_ptr<ManagedDuplicatedUI> ui)
{
    ui.lock()->uiElement = ui.lock()->GetGameObject()->GetComponent<UIElement>();
    DuplicatedUIPool::Instance().poolsByUI[ui.lock().get()] = DuplicatedUIPool::Instance().poolsByID.at(id);
}

void ManagedDuplicatedUI::OnDisable()
{
    DuplicatedUIPool::Instance().Return(GetWeakPtr<ManagedDuplicatedUI>());
}
