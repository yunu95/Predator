#include "UnitBrush.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void UnitBrush::ReadyBrush(Unit_TemplateData* data)
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

                brushList[data]->SetSelfActive(true);
                currentBrush = data;
            }

            bool UnitBrush::CreateBrushFBX(Unit_TemplateData* data)
            {
                if (data == nullptr || brushList.find(data) != brushList.end())
                {
                    return false;
                }

                auto brushObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(data->pod.fbxName);

                brushList[data] = 

                return true;
            }
        }
    }
}
