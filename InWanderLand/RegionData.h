/// 2024. 01. 30. 이윤우
/// IEditableData 의 구체화된 클래스
/// 지역

#pragma once

#include "IEditableData.h"
#include <memory>
#include <string>
#include "RegionEditorInstance.h"
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

        struct POD_Region
        {
            // 웨이브가 배치된 위치, 각도, 크기를 저장합니다.
            float x, z, angle, width, height;
            // 웨이브가 진행되면서 소환되는 유닛들의 고유번호를 저장합니다.
            std::vector<string> waveUnitUUIDS;
            // 유닛들의 소환시점을 저장합니다.
            std::vector<float> waveDelays;
            // waveSizes가 {3, 5, 2}라면 첫번째 웨이브는 waveUnitUUIDS의 0, 1, 2번째 유닛을, 두번째 웨이브는 3, 4, 5, 6, 7번째 유닛을, 세번째 웨이브는 8, 9번째 유닛을 소환합니다.
            // 각 웨이브는 이전 웨이브에서 소환된 유닛들이 모두 사망하면 시작됩니다.
            std::vector<int> waveSizes;
            TO_JSON(POD_Region)
                FROM_JSON(POD_Region)
        };

        class RegionData
            : public IEditableData
        {
            friend class InstanceManager;

        public:
            // 에디터에서 특정 지역에 종속적인 액션을 취하고 싶을 때 부르는 함수입니다.
            // 유닛을 지역 웨이브로서 배치하거나, 지역에 들어갔을 때 가려야 할 장식물을 지정해야 할 때 사용됩니다.
            // 지역에 종속적이지 않은 동작을 하고 싶다면 이 함수의 매개변수로 nullptr을 넘겨주십시오.
            static void SelectEditingRegion(RegionData* regionData);
            virtual bool EnterDataFromTemplate() override;
            virtual ITemplateData* GetTemplateData() override;
            virtual bool SetTemplateData(const std::string& dataName) override;
            virtual IEditableData* Clone() const override;
            virtual void OnRelocate(const Vector3d& newLoc) override;
			virtual palette::PaletteInstance* ApplyAsPaletteInstance()override;
            virtual void ApplyAsPlaytimeObject() override {};

            POD_Region pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static TemplateDataManager& templateDataManager;
            static RegionData* selectedEditorRegion;
            palette::RegionEditorInstance* regionInstance{ nullptr };

            RegionData();
            RegionData(const std::string& name);
            RegionData(const RegionData& prototype);
            RegionData& operator=(const RegionData& prototype);
        };
    }
}
