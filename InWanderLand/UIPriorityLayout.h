#pragma once
#include "YunutyEngine.h"
#include "Timer.h"

class UIPriorityLayout : public TimerComponent
{
public:
    struct ChildIndexComparator
    {
        bool operator()(const GameObject* a, const GameObject* b) const
        {
            return a->GetChildIndex() < b->GetChildIndex();
        }
    };
    std::vector<Vector3d> startPositions;
    std::vector<Vector3d> positions;
    std::set<GameObject*, ChildIndexComparator> enabledUIs;
    void EnableChildUI(GameObject* ui);
    void DisableChildUI(GameObject* ui);
private:
    void StartTransition();
};
