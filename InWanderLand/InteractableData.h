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

namespace yunutyEnigne
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
            std::vector<std::string> targetInteractables = std::vector<std::string>();
            POD_Vector3<float> position = POD_Vector3<float>();
            POD_Quaternion<float> rotation = POD_Quaternion<float>();
            POD_Vector3<float> scale = { 1,1,1 };
            int stage = 1;

            /// Global Constant

            TO_JSON(POD_Interactable)
            FROM_JSON(POD_Interactable)
        };

        class InteractableData
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
            virtual bool EnterDataFromGlobalConstant() override;

            POD_Interactable pod;

            GameObject* inGameInteractable{ nullptr };

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
