#include "ParticleBrush.h"

#include "TemplateDataManager.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void ParticleBrush::CreateBrush()
            {
                if (brush != nullptr)
                    return;

                auto brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Cylinder");

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
                            comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 0,0.5,1,0.1 });
                        }
                    }
                }

                brushObj->SetSelfActive(false);
                brushObj->SetParent(GetGameObject());

                brush = brushObj;

                return;
            }

            void ParticleBrush::ReadyBrush(const std::string& dataKey)
            {
                return ReadyBrush(static_cast<Particle_TemplateData*>(TemplateDataManager::GetSingletonInstance().GetTemplateData(dataKey)));
            }

            void ParticleBrush::ReadyBrush(Particle_TemplateData* data)
            {
                if (data == nullptr && brush)
                {
                    brush->SetSelfActive(false);
                    return;
                }

                if (brush)
                {
                    brush->SetSelfActive(true);
                }
            }
        }
    }
}
