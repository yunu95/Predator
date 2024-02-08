/// 2024. 02. 08 김상준
/// Unit Palette 에서 Unit 을 생성할 때 가이드 역할을 할 Brush 입니다.

#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"

#include "Unit_TemplateData.h"
#include "PaletteManager.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class UnitBrush : public yunutyEngine::Component, public yunutyEngine::SingletonComponent<UnitBrush>
            {
            public:
                // pod 의 fbxName 을 설정할 때 호출해주도록 구성합니다.
                bool CreateBrushFBX(Unit_TemplateData* data);
                // Brush 를 사용하기 전에 호출하는 함수입니다.
                // nullptr 의 경우 비활성화됩니다.
                void ReadyBrush(Unit_TemplateData* data);

            private:
                PaletteManager& pm = PaletteManager::GetSingletonInstance();

                // Save / Load 혹은 Create / Delete 시에 리스트를 변경하는 것이 맞으나,
                // 진행 과정에서 삭제되는 경우에는 어차피 접근하지 못할 것으로 보고
                // 추가되는 경우에만 응대하도록 구성합니다.
                // 추가는 TemplateData 의 생성자에서 추가하도록 합니다.
                std::unordered_map<Unit_TemplateData*, GameObject*> brushList = std::unordered_map<Unit_TemplateData*, GameObject*>();
                Unit_TemplateData* currentBrush = nullptr;
            };
        }
    }
}
