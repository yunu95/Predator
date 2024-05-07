#include "UIPriorityLayout.h"
#include "InWanderLand.h"

void UIPriorityLayout::EnableChildUI(GameObject* ui)
{
    enabledUIs.insert(ui);
    StartTransition(ui);
}
void UIPriorityLayout::DisableChildUI(GameObject* ui)
{
    enabledUIs.erase(ui);
    StartTransition(nullptr);
}
void UIPriorityLayout::StartTransition(GameObject* newlyEnabledElement)
{
    std::transform(enabledUIs.begin(), enabledUIs.end(), std::back_inserter(startPositions), [](GameObject* ui) { return ui->GetTransform()->GetLocalPosition(); });
    if (newlyEnabledElement)
    {
        int posIdx = 0;
        for (auto ui : enabledUIs)
        {
            if (ui == newlyEnabledElement)
            {
                ui->GetTransform()->SetLocalPosition(positions[posIdx]);
            }
            posIdx++;
        }
        enabledUIs.insert(newlyEnabledElement);
    }
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
