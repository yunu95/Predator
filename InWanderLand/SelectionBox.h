#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"
#include "DebugMeshes.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class SelectionBox : public yunutyEngine::SingletonComponent<SelectionBox>
            {
            public:
                virtual void Start() override
                {
                    auto rsrcManager = graphics::Renderer::SingleInstance().GetResourceManager();
                    boxMesh = AttachDebugMesh(GetGameObject(), DebugMeshType::Cube, yunuGI::Color::green(), false);
                }
                void SetCoverage(Vector3d pointA, Vector3d pointB)
                {
                    auto transform = GetTransform();
                    transform->SetWorldPosition(((pointA + pointB) / 2));
                    transform->scale = { (pointA - pointB).Abs() };
                }
            private:
                graphics::StaticMeshRenderer* boxMesh;
            };
        }
    }
}
