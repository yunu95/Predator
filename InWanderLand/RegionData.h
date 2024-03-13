/// 2024. 01. 30. 이윤우
/// IEditableData 의 구체화된 클래스
/// 지역

#pragma once

#include "IEditableData.h"
#include <memory>
#include <string>
#include "RegionEditorInstance.h"
#include "StaticInstanceRegistry.h"
#include "RegionPalette.h"

namespace application
{
    namespace editor
    {
        class TemplateDataManager;
        namespace palette
        {
            class RegionEditorInstance;
        }
    }
}

namespace application
{
    namespace editor
    {
        class RegionData;
        class OrnamentData;
        class Region_TemplateData;

        struct POD_Region
        {
            // 지역이 배치된 위치, 각도, 크기를 저장합니다.
            wstring name;
            float x, z, angle, width, height;
            bool isObstacle{ false };
            int specialEvent{ 0 };
            Region_TemplateData* templateData;
            // 지역에 들어갔을 때 비활성화시킬 장식물들의 UUID를 저장합니다.
            std::vector<string> disablingOrnamentUUIDS;
            TO_JSON(POD_Region)
                FROM_JSON(POD_Region)
        };

        class RegionData
            : public IEditableData, public StaticInstanceRegistry<RegionData>
        {
            friend class InstanceManager;

        public:
            // 에디터에서 특정 지역에 종속적인 액션을 취하고 싶을 때 부르는 함수입니다.
            // 지역에 들어갔을 때 가려야 할 장식물을 지정해야 할 때 사용됩니다.
            // 지역에 종속적이지 않은 동작을 하고 싶다면 이 함수의 매개변수로 nullptr을 넘겨주십시오.
            static void SelectEditingRegion(RegionData* regionData);
            virtual bool EnterDataFromTemplate() override;
            virtual ITemplateData* GetTemplateData() override;
            virtual bool SetTemplateData(const std::string& dataName) override;
            virtual IEditableData* Clone() const override;
            virtual void OnRelocate(const Vector3d& newLoc) override;
            virtual palette::PaletteInstance* ApplyAsPaletteInstance()override;
            virtual void ApplyAsPlaytimeObject() override {};
            const std::unordered_set<OrnamentData*>& GetDisablingOrnaments()const;
            void AddDisablingOrnament(OrnamentData* ornament);
            void EraseDisablingOrnament(OrnamentData* ornament);

            POD_Region pod;

        protected:
            virtual bool PreSaveCallback() override;
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;
            virtual bool PostLoadCallback() override;

        private:

            std::wstring MakeUpName();
            static TemplateDataManager& templateDataManager;
            static RegionData* selectedEditorRegion;
            palette::RegionEditorInstance* regionInstance{ nullptr };
            std::unordered_set<OrnamentData*> disablingOrnaments;

            RegionData();
            RegionData(const std::string& name);
            RegionData(const RegionData& prototype);
            RegionData& operator=(const RegionData& prototype);
        };
    }
}
