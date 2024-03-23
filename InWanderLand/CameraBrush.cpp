#include "CameraBrush.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void CameraBrush::CreateBrush()
            {
                if (brush != nullptr)
                    return;

                auto brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Camera");

                if (brushObj == nullptr)
                    return;


                for (auto each : brushObj->GetChildren())
                {
                    auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

                    if (comp)
                    {
                        for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
                        {
                            comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
                            comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0.3,1,0.1 });
                        }
                    }
                }

                brushObj->SetSelfActive(false);
                brushObj->SetParent(GetGameObject());

                brush = brushObj;

                return;
            }

            void CameraBrush::ReadyBrush(const std::string& dataKey)
            {
                return ReadyBrush(static_cast<Camera_TemplateData*>(TemplateDataManager::GetSingletonInstance().GetTemplateData(dataKey)));
            }

            void CameraBrush::ReadyBrush(Camera_TemplateData* data)
            {
                if (data == nullptr)
                {
                    brush->SetSelfActive(false);
                    return;
                }

                brush->SetSelfActive(true);
            }
        }
    }
}
