#include "OrnamentBrush.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void OrnamentBrush::ReadyBrush(Ornament_TemplateData* data)
            {
                if (data == nullptr)
                {
                    if (currentBrush != nullptr)
                    {
                        brushList[currentBrush]->SetSelfActive(false);
                        currentBrush = nullptr;
                    }
                    return;
                }

                if (currentBrush != nullptr)
                {
                    brushList[currentBrush]->SetSelfActive(false);
                }
                brushList[data]->SetSelfActive(true);
                currentBrush = data;
            }

            bool OrnamentBrush::CreateBrushFBX(Ornament_TemplateData* data)
            {
                if (data == nullptr || brushList.find(data) != brushList.end())
                {
                    return false;
                }

                auto brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(data->pod.fbxName);
                brushObj->SetParent(GetGameObject());

                for (auto each : brushObj->GetChildren())
                {
                    auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

                    if (comp)
                    {
                        for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
                        {
                            comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
                            comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,1,1,0.3 });
                        }
                    }
                }

                brushObj->SetSelfActive(false);

                brushList[data] = brushObj;

                return true;
            }
        }
    }
}
