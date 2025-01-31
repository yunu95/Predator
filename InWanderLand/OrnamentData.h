/// 2023. 11. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 장식물

#pragma once
#include "IEditableData.h"
#include "Ornament_TemplateData.h"

#include <memory>
#include <string>
#include "OrnamentEditorInstance.h"
#include "OrnamentPalette.h"
#include "PodStructs.h"
#include "GlobalConstant.h"
#include "ShortcutSystem.h"

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
        class OrnamentData;

        struct POD_Ornament
        {
            Ornament_TemplateData* templateData = nullptr;
            POD_Vector3<float> position = POD_Vector3<float>();
            POD_Quaternion<double> rotation = POD_Quaternion<double>();
            POD_Vector3<float> scale = { 1,1,1 };
            int LightMapIndex{ -1 };
            std::vector<float> LightMapScaleOffset{ 0,0,0,0 };
            int stage = 1;
            bool isGuide = false;
            bool madeEditor = false;
            bool isBillBoard = false;
            std::string punrealGUID = "";

            TO_JSON(POD_Ornament)
                FROM_JSON(POD_Ornament)
        };

        class OrnamentData
            : public IEditableData
        {
            friend class InstanceManager;

        public:
            class DisablingReference
            {
            private:
                OrnamentData* ornamentData;
                std::weak_ptr<Transform> ornamentTransform;
            public:
                DisablingReference(application::editor::OrnamentData*);
                ~DisablingReference();
            };
            virtual ~OrnamentData();
            virtual bool EnterDataFromTemplate() override;
            virtual ITemplateData* GetTemplateData() override;
            virtual bool SetTemplateData(const std::string& dataName) override;
            virtual IEditableData* Clone() const override;
            virtual void OnRelocate(const Vector3d& newLoc) override;
            virtual void OnRerotate(const Quaternion& newRot) override;
            virtual void OnRescale(const Vector3d& newScale) override;
            virtual void OnDataResourceChange(std::string newName) override;
            virtual palette::PaletteInstance* ApplyAsPaletteInstance() override;
            virtual void ApplyAsPlaytimeObject() override;
            virtual bool EnterDataFromGlobalConstant() override;
            std::shared_ptr<DisablingReference> AcquireDisablingReference();

            POD_Ornament pod;

            bool tookAction = false;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            std::weak_ptr<DisablingReference> disablingReference;
            static TemplateDataManager& templateDataManager;
            palette::OrnamentEditorInstance* ornamentInstance{ nullptr };

            OrnamentData();
            OrnamentData(const std::string& name);
            OrnamentData(const OrnamentData& prototype);
            OrnamentData& operator=(const OrnamentData& prototype);
        };
    }
}
