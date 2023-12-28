#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"

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
                    boxMesh = GetGameObject()->AddComponent<graphics::StaticMeshRenderer>();
                    boxMesh->GetGI().SetMesh(graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(L"Cube"));
                    boxMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());
                    auto& shaderList = rsrcManager->GetShaderList();
                    yunuGI::IShader* shader = nullptr;
                    for (auto each : shaderList)
                    {
                        if (each->GetName() == L"DebugPS.cso")
                        {
                            shader = each;
                        }
                    }
                    boxMesh->GetGI().GetMaterial()->SetPixelShader(shader);
                }
                void SetCoverage(Vector3d pointA, Vector3d pointB) {}
            private:
                graphics::StaticMeshRenderer* boxMesh;
            };
        }
    }
}
