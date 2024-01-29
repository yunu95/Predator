/// 2023. 11. 23 김상준
/// Template Data 와 Instance Data 로 구성되는 실제 Instance 의 인터페이스
/// InstanceManager 를 통해서 관리됨

#pragma once

#include "Identifiable.h"
#include "Storable.h"

#include <memory>
#include <string>

namespace application
{
    namespace editor
    {
        class ITemplateData;
        namespace palette
        {
            class PaletteInstance;
        }
    }
}

namespace application
{
    namespace editor
    {
        enum class DataType
        {
            None,			// 예외용
            TerrainData,
            UnitData,
            OrnamentData
        };
        template<typename T>
        DataType GetDataTypeEnum() { return DataType::None; }

        // 이 클래스는 하나의 인스턴스 데이터를 나타냅니다.
        class IEditableData
            : public Identifiable, public Storable
        {
            friend class InstanceManager;

        public:
            virtual ~IEditableData();

            virtual bool EnterDataFromTemplate() = 0;
            virtual ITemplateData* GetTemplateData() = 0;
            virtual bool SetTemplateData(const std::string& dataName) = 0;
            virtual IEditableData* Clone() const = 0;
            virtual palette::PaletteInstance* ApplyAsPaletteInstance() = 0;
            virtual void OnRelocate(const Vector3d& newLoc) {};
            palette::PaletteInstance* GetPaletteInstance();

        protected:
            virtual bool PreEncoding(json& data) const = 0;
            virtual bool PostEncoding(json& data) const = 0;
            virtual bool PreDecoding(const json& data) = 0;
            virtual bool PostDecoding(const json& data) = 0;
            void SetPaletteInstance(palette::PaletteInstance* paletteInstance);
        private:
            palette::PaletteInstance* paletteInstance{ nullptr };
        };
    }
}
