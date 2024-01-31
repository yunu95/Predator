#include "InWanderLand.h"
#include "RegionEditorInstance.h"
#include "DebugMeshes.h"

namespace application::editor::palette
{
    void RegionEditorInstance::Start()
    {
        PaletteInstance::Start();
        mesh = AttachDebugMesh(GetGameObject()->AddGameObject(), DebugMeshType::Rectangle, yunuGI::Color{0, 0, 1}, true);
        mesh->GetTransform()->rotation = Quaternion({ 90,0,0 });
        SetHalfExtent(halfExtent);
    }
    void RegionEditorInstance::OnHover()
    {
        isHovering = true;
        mesh->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 1,1,1 }, true));
    }
    void RegionEditorInstance::OnHoverLeft()
    {
        isHovering = false;
        if (isSelected)
            OnSelected();
        else
        {
            mesh->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 0,0,1 }, true));
        }
    }
    void RegionEditorInstance::OnSelected()
    {
        isSelected = true;
        mesh->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 0,1,0 }, true));
    }
    void RegionEditorInstance::OnDeselected()
    {
        isSelected = false;
        if (isHovering)
            OnHover();
        else
        {
            mesh->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 0,0,1 }, true));
        }
    }
    void RegionEditorInstance::Apply(const application::editor::RegionData* regionData)
    {
        SetHalfExtent({ regionData->pod.width, regionData->pod.height });
        GetTransform()->SetWorldPosition({ regionData->pod.x, 0, regionData->pod.z });
        GetTransform()->SetWorldRotation(Vector3d{ 0,regionData->pod.angle ,0 });
    }
    void RegionEditorInstance::SetHalfExtent(const Vector2f& halfExtent)
    {
        this->halfExtent = halfExtent;

        if (this->halfExtent.x < 0.01)
            this->halfExtent.x = 0.01;
        if (this->halfExtent.y < 0.01)
            this->halfExtent.y = 0.01;

        if (wasStartCalled())
        {
            pickingCollider->SetHalfExtent({ halfExtent.x,100,halfExtent.y });
            mesh->GetTransform()->scale = Vector3d{ halfExtent.x * 2,halfExtent.y * 2,1 };
        }
    }
}
