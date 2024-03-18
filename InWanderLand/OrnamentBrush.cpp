#include "OrnamentBrush.h"

#include "TemplateDataManager.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void OrnamentBrush::CreateBrush()
            {
                for (auto& each : TemplateDataManager::GetSingletonInstance().GetDataList(DataType::OrnamentData))
                {
                    CreateBrush(each->GetDataKey());
                }
            }

            bool OrnamentBrush::CreateBrush(const std::string& dataKey)
            {
                if (brushList.find(dataKey) != brushList.end())
                    return false;

                auto name = static_cast<Ornament_TemplateData*>(TemplateDataManager::GetSingletonInstance().GetTemplateData(dataKey))->pod.fbxName;

                auto brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(name);

                if (brushObj == nullptr)
                    return false;

                brushObj->SetParent(GetGameObject());

                for (auto each : brushObj->GetChildren())
                {
                    auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

                    if (comp)
                    {
                        for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
                        {
                            comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
                            comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,1,1,0.1 });
                        }
                    }
                }

                brushObj->SetSelfActive(false);

                brushList[dataKey] = brushObj;

                return true;
            }

            bool OrnamentBrush::ChangeBrushResource(const std::string& dataKey, const std::string& fbxName)
            {
                if (brushList.find(dataKey) == brushList.end())
                    return false;

                DestroyBrush(dataKey);

                auto brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName);

                if (brushObj == nullptr)
                    return false;

                brushObj->setName(dataKey);
                brushObj->SetParent(GetGameObject());

                for (auto each : brushObj->GetChildren())
                {
                    auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

                    if (comp)
                    {
                        for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
                        {
                            comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
                            comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,1,1,0.1 });
                        }
                    }
                }

                brushObj->SetSelfActive(false);

                brushList[dataKey] = brushObj;

                return true;
            }

            void OrnamentBrush::ReadyBrush(const std::string& dataKey)
            {
                return ReadyBrush(static_cast<Ornament_TemplateData*>(TemplateDataManager::GetSingletonInstance().GetTemplateData(dataKey)));
            }

            void OrnamentBrush::Clear()
            {
                std::vector<std::string> list;
                list.reserve(brushList.size());

                for (auto& each : brushList)
                {
                    list.push_back(each.first);
                }

                for (auto& each : list)
                {
                    DestroyBrush(each);
                }

                currentBrush = nullptr;
            }

            void OrnamentBrush::Update()
            {
                if (currentBrush != nullptr && !brushList[currentBrush->GetDataKey()]->GetSelfActive())
                {
                    brushList[currentBrush->GetDataKey()]->SetSelfActive(true);
                }
            }

            bool OrnamentBrush::DestroyBrush(const std::string& dataKey)
            {
                auto itr = brushList.find(dataKey);
                if (itr != brushList.end())
                {
                    yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(itr->second);
                    brushList.erase(dataKey);
                    return true;
                }
                return false;
            }

            void OrnamentBrush::ReadyBrush(Ornament_TemplateData* data)
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
