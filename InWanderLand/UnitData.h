/// 2023. 11. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 유닛

#pragma once

#include "IEditableData.h"
#include "Unit_TemplateData.h"
#include "PodStructs.h"
#include "UnitEditorInstance.h"
#include "UnitPalette.h"

#include <memory>
#include <string>

namespace application
{
    namespace editor
    {
        class TemplateDataManager;
    }
}

namespace application
{
    namespace editor
    {
        namespace unit
        {
            enum class Affiliation
            {
                None,	// 예외용
                Player,
                Enemy
            };
        }

        class UnitData;

        struct POD_Unit
        {
            Unit_TemplateData* templateData = nullptr;
            WaveData* waveData = nullptr;
            unit::Affiliation affiliation = unit::Affiliation::None;
            POD_Vector3<float> position = POD_Vector3<float>();
            POD_Quaternion<float> rotation = POD_Quaternion<float>();
            POD_Vector3<float> scale = { 1,1,1 };

            TO_JSON(POD_Unit)
            FROM_JSON(POD_Unit)
        };

        class UnitData
            : public IEditableData
        {
            friend class InstanceManager;

        public:
            virtual bool EnterDataFromTemplate() override;
            virtual ITemplateData* GetTemplateData() override;
            virtual bool SetTemplateData(const std::string& dataName) override;
            virtual IEditableData* Clone() const override;
            virtual void OnRelocate(const Vector3d& newLoc) override;
            virtual void OnRerotate(const Quaternion& newRot) override;
            virtual void OnRescale(const Vector3d& newScale) override;
            virtual void OnDataResourceChange(std::string newName) override;
            virtual palette::PaletteInstance* ApplyAsPaletteInstance()override;
            virtual void ApplyAsPlaytimeObject() override;

            POD_Unit pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static TemplateDataManager& templateDataManager;
            palette::UnitEditorInstance* unitInstance{ nullptr };

            UnitData();
            UnitData(const std::string& name);
            UnitData(const UnitData& prototype);
            UnitData& operator=(const UnitData& prototype);
        };
    }
}
