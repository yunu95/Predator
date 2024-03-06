/// 2024. 01. 30. 이윤우
/// IEditableData 의 구체화된 클래스
/// 지역

#pragma once

#include "IEditableData.h"
#include "YunutyEngine.h"
#include "StaticInstanceRegistry.h"
#include "RegionData.h"
#include <memory>
#include <string>

namespace application
{
    namespace editor
    {
        class TemplateDataManager;
        namespace palette
        {
            class WaveEditorInstance;
        }
    }
}

namespace application
{
    namespace editor
    {
        class UnitData;
        class WaveData;
        class RegionData;
        class Wave_TemplateData;

        struct POD_Wave
        {
            std::wstring name;
            // 웨이브가 진행되면서 소환되는 유닛들의 고유번호를 저장합니다.
            std::vector<string> waveUnitUUIDS;
            // 유닛들의 소환시점을 저장합니다.
            std::vector<float> waveDelays;
            // waveSizes가 {3, 5, 2}라면 첫번째 웨이브는 waveUnitUUIDS의 0, 1, 2번째 유닛을, 두번째 웨이브는 3, 4, 5, 6, 7번째 유닛을, 세번째 웨이브는 8, 9번째 유닛을 소환합니다.
            // 각 웨이브는 이전 웨이브에서 소환된 유닛들이 모두 사망하면 시작됩니다.
            std::vector<int> waveSizes;
            // 웨이브를 발동시키는 지역
            RegionData* triggerRegion{ nullptr };
            // 웨이브가 발동되었을 때 카메라의 이동을 제한하는 지역
            RegionData* contraintRegion{ nullptr };
            Wave_TemplateData* templateData{ nullptr };

            TO_JSON(POD_Wave)
                FROM_JSON(POD_Wave)
        };

        class WaveData : public IEditableData, public StaticInstanceRegistry<WaveData>
        {
            friend class InstanceManager;
        public:
            struct WaveUnitData
            {
                UnitData* unitData;
                int waveIdx;
                float delay;
            };
            // 에디터에서 특정 웨이브에 종속적인 액션을 취하고 싶을 때 부르는 함수입니다.
            // 웨이브에서 소환되는 유닛들을 배치하고 싶을 때 사용됩니다.
            // 웨이브에 종속적이지 않은 동작을 하고 싶다면 이 함수의 매개변수로 nullptr을 넘겨주십시오.
            static void SelectEditingWave(WaveData* waveData);
            virtual bool EnterDataFromTemplate() override;
            virtual ITemplateData* GetTemplateData() override;
            virtual bool SetTemplateData(const std::string& dataName) override;
            virtual IEditableData* Clone() const override;
            virtual void OnRelocate(const Vector3d& newLoc) override;
            virtual palette::PaletteInstance* ApplyAsPaletteInstance()override;
            virtual void ApplyAsPlaytimeObject() override;
            void InsertUnitData(WaveUnitData waveUnitData);
            void DeleteUnitData(UnitData* unitData);
            void HideWaveUnitsVisibility();
            unordered_map<UnitData*, WaveUnitData>& GetWaveUnitDataMap() { return waveUnitDataMap; }

            POD_Wave pod;
            void ApplyPodAsMap();
            void ApplyMapAsPod();

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;
            virtual bool PostLoadCallback() override;
            virtual bool PreSaveCallback() override;

        private:
            static TemplateDataManager& templateDataManager;
            static WaveData* selectedEditorWave;
            palette::WaveEditorInstance* waveInstance{ nullptr };
            unordered_map<UnitData*, WaveUnitData> waveUnitDataMap;

            WaveData();
            WaveData(const std::string& name);
            WaveData(const WaveData& prototype);
            std::wstring MakeUpName();
            WaveData& operator=(const WaveData& prototype);
        };
    }
}

