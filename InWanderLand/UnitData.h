/// 2023. 11. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 유닛

#pragma once

#include "IEditableData.h"
#include "Unit_TemplateData.h"
#include "PodStructs.h"
#include "UnitEditorInstance.h"
#include "UnitPalette.h"
#include "GlobalConstant.h"

#include <memory>
#include <string>

class Unit;
class UnitProductor;

namespace application
{
    namespace editor
    {
        class TemplateDataManager;
        class WaveData;
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
            int stage = 1;
            bool isGuide = false;
            bool isBossSummon = false;
            bool summonLeftFrame = false;

            /// Global Constant



            TO_JSON(POD_Unit)
                FROM_JSON(POD_Unit)
        };

        class UnitData
            : public IEditableData
        {
            friend class InstanceManager;

        public:
            virtual bool EnterDataFromTemplate() override;
            virtual Unit_TemplateData* GetTemplateData()override;
            virtual Unit_TemplateData* GetUnitTemplateData()const;
            virtual bool SetTemplateData(const std::string& dataName) override;
            virtual IEditableData* Clone() const override;
            virtual void OnRelocate(const Vector3d& newLoc) override;
            virtual void OnRerotate(const Quaternion& newRot) override;
            virtual void OnRescale(const Vector3d& newScale) override;
            virtual void OnDataResourceChange(std::string newName) override;
            virtual palette::PaletteInstance* ApplyAsPaletteInstance()override;
            virtual void ApplyAsPlaytimeObject() override;
            virtual void PostApplyAsPlaytimeObject() override;
            virtual bool EnterDataFromGlobalConstant() override;

            POD_Unit pod;

            // 내가 공격할 때, 매개변수는 내가 공격하는 상대
            DelegateCallback<std::weak_ptr<Unit>> onAttack;
            // 내가 때린 공격이 적에게 맞았을 때, 근거리 공격인 경우라면 onAttack과 호출시점이 같겠으나 원거리 공격인 경우에는 시간차가 있을 수 있다. 
            // 매개변수는 피해를 받은 상대
            DelegateCallback<std::weak_ptr<Unit>> onAttackHit;
            // 내가 피해를 입었을 때, 매개변수는 피해를 준 상대
            DelegateCallback<void> onDamaged;
            // 유닛이 새로 생성될 때
            DelegateCallback<void> onCreated;
            // 유닛이 회전을 끝냈을 때
            DelegateCallback<void> onRotationFinish;

            std::array<DelegateCallback<void>, UnitBehaviourTree::Keywords::KeywordNum> onStateEngage;
            std::array<DelegateCallback<void>, UnitBehaviourTree::Keywords::KeywordNum> onStateExit;
            mutable std::weak_ptr<Unit> inGameUnit;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static TemplateDataManager& templateDataManager;
            palette::UnitEditorInstance* unitInstance{ nullptr };

            std::vector<UnitProductor*> productorSelector;
            bool isSelectorInitialized{ false };

            UnitData();
            UnitData(const std::string& name);
            UnitData(const UnitData& prototype);
            UnitData& operator=(const UnitData& prototype);
        };
    }
}
