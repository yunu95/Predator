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
                for (auto& each : TemplateDataManager::GetSingletonInstance().GetDataList(DataType::ParticleData))
                {
                    CreateBrush(each->GetDataKey());
                }
            }

            bool ParticleBrush::CreateBrush(const std::string& dataKey)
            {
                if (brushList.find(dataKey) != brushList.end())
                    return false;

                /// Cone FBX 있는지 체크
                auto brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Cylinder");

                if (brushObj == nullptr)
                    return false;

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

                brushList[dataKey] = brushObj;

                return true;
            }

            void ParticleBrush::ReadyBrush(const std::string& dataKey)
            {
                return ReadyBrush(static_cast<Particle_TemplateData*>(TemplateDataManager::GetSingletonInstance().GetTemplateData(dataKey)));
            }

            void ParticleBrush::Update()
            {
                if (currentBrush != nullptr && !brushList[currentBrush->GetDataKey()]->GetSelfActive())
                {
                    brushList[currentBrush->GetDataKey()]->SetSelfActive(true);
                }
            }

            void ParticleBrush::ReadyBrush(Particle_TemplateData* data)
            {
                if (data == nullptr)
                {
                    if (currentBrush != nullptr)
                    {
                        brushList[currentBrush->GetDataKey()]->SetSelfActive(false);
                        currentBrush = nullptr;
                    }
                    return;
                }

                if (currentBrush != nullptr)
                {
                    brushList[currentBrush->GetDataKey()]->SetSelfActive(false);
                }

                currentBrush = data;
                brushList[currentBrush->GetDataKey()]->SetSelfActive(true);
            }
        }
    }
}
