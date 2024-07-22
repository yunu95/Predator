/// 2023. 11. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 유닛

#pragma once

#include "IEditableData.h"
#include "Interactable_TemplateData.h"
#include "PodStructs.h"
#include "InteractableEditorInstance.h"
#include "InteractablePalette.h"
#include "GlobalConstant.h"
#include "IObserver.h"
#include "ProgressTracker.h"

#include <memory>
#include <string>

class Unit;
class UnitProductor;
class IInteractableComponent;

namespace application
{
    namespace editor
    {
        class TemplateDataManager;
        class WaveData;
    }
}

namespace yunutyEngine
{
    class GameObject;
}

namespace application
{
    namespace editor
    {
        class InteractableData;

        struct POD_Interactable
        {
            Interactable_TemplateData* templateData = nullptr;
            /// UUID string
            std::unordered_set<std::string> targetInteractables = std::unordered_set<std::string>();
            POD_Vector3<float> position = POD_Vector3<float>();
            POD_Quaternion<float> rotation = POD_Quaternion<float>();
            POD_Vector3<float> scale = { 1,1,1 };
            POD_Vector2<float> uiOffset = POD_Vector2<float>();
            std::string guideUI = "Texture/Interactable/UI_Interaction.png";
            float ui_Width = -1;
            float ui_Height = -1;
            int stage = 1;

            /// Global Constant

            TO_JSON(POD_Interactable)
            FROM_JSON(POD_Interactable)
        };

        class InteractableData
            : public IEditableData, public IObserver, public application::ProgressTracker
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
            virtual void PostApplyAsPlaytimeObject() override;
            virtual bool EnterDataFromGlobalConstant() override;
            const std::unordered_set<std::string>& GetTargetInteractables() { return pod.targetInteractables; }
            bool AddTargetInteractables(InteractableData* target);
            bool EraseTargetInteractables(InteractableData* target);

            virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

            virtual void CurrentProgressSave();
            virtual void Recovery();

            POD_Interactable pod;

            IInteractableComponent* inGameInteractable{ nullptr };

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static TemplateDataManager& templateDataManager;
            palette::InteractableEditorInstance* interactableInstance{ nullptr };

            bool hasAnimation = false;

            InteractableData();
            InteractableData(const std::string& name);
            InteractableData(const InteractableData& prototype);
            InteractableData& operator=(const InteractableData& prototype);
        };
    }
}
