#ifdef EDITOR
#include "InWanderLand.h"
#include "RegionInstance.h"
#include "DebugMeshes.h"

namespace application::editor::palette
{
    void RegionInstance::Start()
    {
        PaletteInstance::Start();
        mesh = AttachDebugMesh(GetGameObject()->AddGameObject(), DebugMeshType::Rectangle, yunuGI::Color{0, 0, 1}, true);
        mesh->GetTransform()->rotation = Quaternion({ 90,0,0 });
        SetHalfExtent(halfExtent);
    }
    void RegionInstance::OnHover()
    {
        isHovering = true;
        mesh->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 1,1,1 }, true));
    }
    void RegionInstance::OnHoverLeft()
    {
        isHovering = false;
        if (isSelected)
            OnSelected();
        else
        {
            mesh->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 0,0,1 }, true));
        }
    }
    void RegionInstance::OnSelected()
    {
        isSelected = true;
        mesh->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 0,1,0 }, true));
    }
    void RegionInstance::OnDeselected()
    {
        isSelected = false;
        if (isHovering)
            OnHover();
        else
        {
            mesh->GetGI().SetMaterial(0, GetColoredDebugMaterial({ 0,0,1 }, true));
        }
    }
    void RegionInstance::SetHalfExtent(const Vector2f& halfExtent)
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
#endif
