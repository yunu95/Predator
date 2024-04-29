#include "UIPriorityLayout.h"
#include "InWanderLand.h"

void UIPriorityLayout::EnableChildUI(GameObject* ui)
{
    enabledUIs.insert(ui);
    StartTransition();
}
void UIPriorityLayout::DisableChildUI(GameObject* ui)
{
    enabledUIs.erase(ui);
    StartTransition();
}
void UIPriorityLayout::StartTransition()
{
    std::transform(enabledUIs.begin(), enabledUIs.end(), std::back_inserter(startPositions), [](GameObject* ui) { return ui->GetTransform()->GetLocalPosition(); });
    onUpdate = [this](float t)
        {
            int posIdx = 0;
            for (auto ui : enabledUIs)
            {
                ui->GetTransform()->SetLocalPosition(Vector3d::Lerp(startPositions[posIdx], positions[posIdx], sinf(t * 0.5 * yunutyEngine::math::PI)));
                posIdx++;
            }
        };
    ActivateTimer();
}
