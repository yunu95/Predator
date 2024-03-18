/// 2024. 03. 07 김상준
/// Palette 에서 사용할 Brush 인터페이스 클래스

#pragma once

#include "PaletteManager.h"
#include "EditorResourceManager.h"

#include <string>

namespace application
{
    namespace editor
    {
        class ITemplateData;
    }
}

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class PaletteBrush
            {
            public:
                virtual ~PaletteBrush() = default;

                virtual void Initialize() = 0;

                // Load 시 호출 할 모든 Brush 일괄 생성 함수입니다. 
                virtual void CreateBrush() = 0;

                // TemplateData Key 값으로부터 개별 Brush 를 생성하는 함수입니다.
                virtual bool CreateBrush(const std::string& dataKey) = 0;

                // data 의 Resource 를 수정할 때 호출하는 함수입니다.
                virtual bool ChangeBrushResource(const std::string& dataKey, const std::string& fbxName) = 0;

                // Brush 를 사용하기 전에 호출하는 함수입니다.
                // dataKey 값을 찾을 수 없는 경우 비활성화됩니다.
                virtual void ReadyBrush(const std::string& dataKey) = 0;

                // 초기화를 위한 Clear 함수입니다.
                virtual void Clear() = 0;

            protected:
                // data가 변경될 때, 기존의 fbx obj를 삭제하기 위한 함수입니다.
                virtual bool DestroyBrush(const std::string& dataKey) = 0;
 
                PaletteManager& pm = PaletteManager::GetSingletonInstance();
                ResourceManager& erm = ResourceManager::GetSingletonInstance();
            };
        }
    }
}
