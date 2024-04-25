#include "LightBrush.h"

#include "TemplateDataManager.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void LightBrush::CreateBrush()
            {
                for (auto& each : TemplateDataManager::GetSingletonInstance().GetDataList(DataType::LightData))
                {
                    CreateBrush(each->GetDataKey());
                }
            }

            bool LightBrush::CreateBrush(const std::string& dataKey)
            {
                if (brushList.find(dataKey) != brushList.end())
                    return false;

                auto type = static_cast<Light_TemplateData*>(TemplateDataManager::GetSingletonInstance().GetTemplateData(dataKey))->pod.type;

                std::string fbxType;

                switch (type)
                {
                    case LightType::Directional:
                    {
                        fbxType = "Directional";
                        break;
                    }
                    case LightType::Point:
                    {
                        fbxType = "Sphere";
                        break;
                    }
                    default:
                        break;
                }

                auto brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxType);

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
                            comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0.3,1,0.1 });
                        }
                    }
                }

                brushObj->SetSelfActive(false);
                brushObj->SetParent(GetGameObject());

                brushList[dataKey] = brushObj;

                return true;
            }

            void LightBrush::ReadyBrush(const std::string& dataKey)
            {
                return ReadyBrush(static_cast<Light_TemplateData*>(TemplateDataManager::GetSingletonInstance().GetTemplateData(dataKey)));
            }

            void LightBrush::Update()
            {
                if (currentBrush != nullptr && !brushList[currentBrush->GetDataKey()]->GetSelfActive())
                {
                    brushList[currentBrush->GetDataKey()]->SetSelfActive(true);
                }
            }

            void LightBrush::ReadyBrush(Light_TemplateData* data)
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
