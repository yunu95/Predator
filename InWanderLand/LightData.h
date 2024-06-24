/// 2024. 03. 13 김상준
/// IEditableData 의 구체화된 클래스
/// Light

#pragma once
#include "IEditableData.h"
#include "Light_TemplateData.h"
#include "StaticInstanceRegistry.h"

#include <memory>
#include <string>
#include "LightEditorInstance.h"
#include "LightPalette.h"
#include "PodStructs.h"
#include "GlobalConstant.h"

namespace yunutyEngine
{
    namespace graphics
    {
        class DirectionalLight;
    }
}

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
        class LightData;

        struct LightColor
        {
            float r = 1;
            float g = 1;
            float b = 1;
            float a = 1;

            TO_JSON(LightColor)
            FROM_JSON(LightColor)
        };

        struct POD_Light
        {
            Light_TemplateData* templateData = nullptr;
            std::string name = "Light";
            POD_Vector3<float> position = POD_Vector3<float>();
            POD_Quaternion<double> rotation = POD_Quaternion<double>();
            POD_Vector3<float> scale = { 1,1,1 };
            LightColor color = { 1,1,1,1 };
            float intensity = 1;
            float range = 1;
            bool isCast = true;

            /// GlobalConstant


            TO_JSON(POD_Light)
            FROM_JSON(POD_Light)
        };

        class LightData
            : public IEditableData, public StaticInstanceRegistry<LightData>
        {
            friend class InstanceManager;

        public:
            static yunutyEngine::graphics::DirectionalLight* GetPlaytimeDirectionalLight();

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
            virtual bool EnterDataFromGlobalConstant() override { return true; };

            void OnLightTypeChange(LightType type);

            POD_Light pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static TemplateDataManager& templateDataManager;
            palette::LightEditorInstance* lightInstance{ nullptr };

            static yunutyEngine::graphics::DirectionalLight* directionalLight;

            LightData();
            LightData(const std::string& name);
            LightData(const LightData& prototype);
            LightData& operator=(const LightData& prototype);
        };
    }
}
