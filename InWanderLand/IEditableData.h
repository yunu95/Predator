/// 2023. 11. 23 김상준
/// Template Data 와 Instance Data 로 구성되는 실제 Instance 의 인터페이스
/// InstanceManager 를 통해서 관리됨

#pragma once

#ifdef EDITOR
#include "Identifiable.h"
#include "Storable.h"

#include <memory>
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
        enum class DataType
        {
            None,			// 예외용
            TerrainData,
            UnitData,
            OrnamentData
        };
        template<typename T>
        DataType GetDataTypeEnum() { return DataType::None; }

        class IEditableData
            : public Identifiable, public Storable
        {
            friend class InstanceManager;

        public:
            virtual ~IEditableData() = default;

            virtual bool EnterDataFromTemplate() = 0;
            virtual ITemplateData* GetTemplateData() = 0;
            virtual bool SetTemplateData(const std::string& dataName) = 0;
            virtual IEditableData* Clone() const = 0;

        protected:
            virtual bool PreEncoding(json& data) const = 0;
            virtual bool PostEncoding(json& data) const = 0;
            virtual bool PreDecoding(const json& data) = 0;
            virtual bool PostDecoding(const json& data) = 0;
        };
    }
}
#endif
