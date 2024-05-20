#include "InteractableBrush.h"

#include "TemplateDataManager.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void InteractableBrush::CreateBrush()
            {
                for (auto& each : TemplateDataManager::GetSingletonInstance().GetDataList(DataType::InteractableData))
                {
                    CreateBrush(each->GetDataKey());
                }
            }

            bool InteractableBrush::CreateBrush(const std::string& dataKey)
            {
                if (brushList.find(dataKey) != brushList.end())
                    return false;

                auto name = static_cast<Interactable_TemplateData*>(TemplateDataManager::GetSingletonInstance().GetTemplateData(dataKey))->pod.fBXName;

                GameObject* brushObj = nullptr;

                if (name == "Trigger_Cube")
                {
                    brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Cube");
                    
                }
                else if (name == "Trigger_Sphere")
                {
                    brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Sphere");
                }
                else
                {
                    brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(name);
                }

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

                brushObj->setName(dataKey);
                brushObj->SetSelfActive(false);
                brushObj->SetParent(GetGameObject());

                brushList[dataKey] = brushObj;

                return true;
            }

            bool InteractableBrush::ChangeBrushResource(const std::string& dataKey, const std::string& fbxName)
            {
                if (brushList.find(dataKey) == brushList.end())
                    return false;

                DestroyBrush(dataKey);

                GameObject* brushObj = nullptr;

                if (fbxName == "Trigger_Cube")
                {
                    brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Cube");

                }
                else if (fbxName == "Trigger_Sphere")
                {
                    brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Sphere");
                }
                else
                {
                    brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName);
                }

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

                brushObj->setName(dataKey);
                brushObj->SetSelfActive(false);
                brushObj->SetParent(GetGameObject());

                brushList[dataKey] = brushObj;

                return true;
            }

            void InteractableBrush::ReadyBrush(const std::string& dataKey)
            {
                return ReadyBrush(static_cast<Interactable_TemplateData*>(TemplateDataManager::GetSingletonInstance().GetTemplateData(dataKey)));
            }

            void InteractableBrush::Clear()
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

            void InteractableBrush::Update()
            {
                if (currentBrush != nullptr && !brushList[currentBrush->GetDataKey()]->GetSelfActive())
                {
                    brushList[currentBrush->GetDataKey()]->SetSelfActive(true);
                }
            }

            bool InteractableBrush::DestroyBrush(const std::string& dataKey)
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

            void InteractableBrush::ReadyBrush(Interactable_TemplateData* data)
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
