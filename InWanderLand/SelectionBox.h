#pragma once
#ifdef EDITOR
#include "PaletteManager.h"
#include "SelectionBox.h"
#include "YunutyEngine.h"
#include "SingletonComponent.h"
#include "DebugMeshes.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class SelectionBox : public yunutyEngine::Component,public yunutyEngine::SingletonComponent<SelectionBox>
            {
            public:
                virtual void Start() override
                {
                    auto rsrcManager = graphics::Renderer::SingleInstance().GetResourceManager();
                    boxMesh = AttachDebugMesh(GetGameObject(), DebugMeshType::Rectangle, yunuGI::Color::green(), true);
                    yunutyEngine::physics::BoxCollider* boxCollider = GetGameObject()->AddComponent<yunutyEngine::physics::BoxCollider>();
                }
                void SetCoverage(Vector3d pointA, Vector3d pointB)
                {
                    auto transform = GetTransform();
                    transform->rotation = Quaternion({ 90,0,0 });
                    transform->SetWorldPosition(((pointA + pointB) / 2));
                    transform->scale = { (pointA - pointB).Abs() };
                    transform->scale.y = transform->scale.z;
                }
                void ShowSelectionBox(bool box)
                {
                    if (boxMesh)
                        boxMesh->SetActive(box);
                }
                unordered_set<PaletteInstance*>& GetContactingInstances()
                {
                    return contactingInstances;
                }
            protected:
                virtual void OnTriggerEnter(physics::Collider* other) override
                {
                    if (auto instance = other->GetGameObject()->GetComponent<PaletteInstance>())
                    {
                        contactingInstances.insert(instance);
                        if (PaletteManager::GetCurrentPalette()->ShouldSelect(instance))
                            PaletteManager::GetCurrentPalette()->OnSelectionContactEnter(instance);
                    }
                }
                virtual void OnTriggerExit(physics::Collider* other) override
                {
                    if (other == nullptr)
                        return;
                    if (auto instance = other->GetGameObject()->GetComponent<PaletteInstance>(); instance != nullptr)
                    {
                        contactingInstances.erase(instance);
                        if (PaletteManager::GetCurrentPalette()->ShouldSelect(instance))
                            PaletteManager::GetCurrentPalette()->OnSelectionContactExit(instance);
                    }
                }
            private:
                unordered_set<PaletteInstance*> contactingInstances;
                graphics::StaticMeshRenderer* boxMesh;
            };
        }
    }
}
#endif
