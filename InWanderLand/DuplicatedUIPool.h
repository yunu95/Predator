#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"
#include "UIEnumID.h"

class VFXAnimator;
class DuplicatedUIPool;
class ManagedDuplicatedUI;

class ManagedDuplicatedUI : public Component
{
private:
    UIElement* uiElement{ nullptr };
    virtual Component* GetComponent() { return this; };
    virtual void OnDisable() override;
public:
    UIElement* GetUIElement() { return uiElement; };
    friend DuplicatedUIPool;
};

// DuplicatedUIPool은 fbx 이름별로 여러가지 풀들을 갖고 있다.
// DuplicatedUIPool은 풀 집합체와 같다고 할 수 있다.
class DuplicatedUIPool
    : public SingletonComponent<DuplicatedUIPool>, public Component, public PermanentObservee
{
public:
    class PoolByEnumID : public GameObjectPool<ManagedDuplicatedUI>
    {
    public:
        UIEnumID id;
    private:
        virtual void ObjectInitializer(std::weak_ptr<ManagedDuplicatedUI> unit) override;
        GameObject* GameObjectInitializer();
    };
    std::weak_ptr<ManagedDuplicatedUI> Borrow(UIEnumID id);
    void Return(std::weak_ptr<ManagedDuplicatedUI>);
    virtual void OnContentsStop() override;
    virtual Component* GetComponent() override { return this; }
private:

    std::unordered_set<ManagedDuplicatedUI*> borrowedList;

    std::unordered_map<UIEnumID, std::shared_ptr<PoolByEnumID>> poolsByID;
    std::unordered_map<ManagedDuplicatedUI*, std::weak_ptr<PoolByEnumID>> poolsByUI;
};
