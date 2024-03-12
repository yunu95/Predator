/// 2024. 03. 12 김상준
/// IEditableData 의 구체화된 클래스
/// 카메라

#pragma once
#include "IEditableData.h"
#include "Camera_TemplateData.h"

#include <memory>
#include <string>
#include "CameraEditorInstance.h"
#include "CameraPalette.h"
#include "PodStructs.h"
#include "GlobalConstant.h"

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
        class CameraData;

        struct POD_Camera
        {
            Camera_TemplateData* templateData = nullptr;
            POD_Vector3<float> position = POD_Vector3<float>();
            POD_Quaternion<double> rotation = POD_Quaternion<double>();
            POD_Vector3<float> scale = { 1,1,1 };

            /// GlobalConstant


            TO_JSON(POD_Camera)
            FROM_JSON(POD_Camera)
        };

        class CameraData
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
            virtual palette::PaletteInstance* ApplyAsPaletteInstance() override;
            virtual void ApplyAsPlaytimeObject() override;
            virtual bool EnterDataFromGlobalConstant() override { return true; };

            POD_Camera pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static TemplateDataManager& templateDataManager;
            palette::CameraEditorInstance* cameraInstance{ nullptr };

            CameraData();
            CameraData(const std::string& name);
            CameraData(const CameraData& prototype);
            CameraData& operator=(const CameraData& prototype);
        };
    }
}
