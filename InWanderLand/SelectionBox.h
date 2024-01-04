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
                    boxMesh = AttachDebugMesh(GetGameObject(), DebugMeshType::Cube, yunuGI::Color::green(), true);
                    //boxMesh->GetGI().SetMesh(graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(L"Cube"));
                    //boxMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());
                    //auto& shaderList = rsrcManager->GetShaderList();
                    //yunuGI::IShader* shader = nullptr;
                    /*for (auto each : shaderList)
                    {
                        if (each->GetName() == L"DebugPS.cso")
                        {
                            shader = each;
                        }
                    }
                    boxMesh->GetGI().GetMaterial()->SetPixelShader(shader);*/
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
