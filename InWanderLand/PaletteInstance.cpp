#include "InWanderLand.h"
#include "PaletteInstance.h"
#include "DebugMeshes.h"
#include "Palette.h"

namespace application::editor::palette
{
    void PaletteInstance::Start()
    {
        selectCircle = AttachDebugMesh(GetGameObject()->AddGameObject(), DebugMeshType::Rectangle, yunuGI::Color{1, 1, 1}, true);
        selectCircle->GetTransform()->rotation = Quaternion({ 90,0,0 });
        pickingCollider = GetGameObject()->AddComponent<yunutyEngine::physics::BoxCollider>();
        rigidBody = GetGameObject()->AddComponent<yunutyEngine::physics::RigidBody>();
        pickingCollider->SetHalfExtent({ 0.01,100,0.01 });
        rigidBody->SetAsKinematic(true);
        selectCircle->GetTransform()->scale = Vector3d::one * selectCircleRadius;
        selectCircle->SetActive(false);
    }
    void PaletteInstance::OnHover()
    {
        isHovering = true;
        selectCircle->SetActive(true);
        selectCircle->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 1,1,1 }, true));
    }
    void PaletteInstance::OnHoverLeft()
    {
        isHovering = false;
        if (isSelected)
            OnSelected();
        else
        {
            selectCircle->SetActive(false);
        }
    }
    void PaletteInstance::OnSelected()
    {
        isSelected = true;
        selectCircle->SetActive(true);
        selectCircle->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 0,1,0 }, true));
    }
    void PaletteInstance::OnDeselected()
    {
        isSelected = false;
        if (isHovering)
            OnHover();
        else
        {
            selectCircle->SetActive(false);
        }
    }
    void PaletteInstance::EnablePickingCollider()
    {
        pickingCollider->SetActive(true);
    }
    void PaletteInstance::DisablePickingCollider()
    {
        pickingCollider->SetActive(false);
    }
    void PaletteInstance::ResetInstances()
    {
    }
    IEditableData* PaletteInstance::GetEditableData()
    {
        return editableData;
    }
    void PaletteInstance::ApplyInstances()
    {
    }
}
