/// 2024. 04. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 파티클

#pragma once
#include "IEditableData.h"
#include "Particle_TemplateData.h"
#include "StaticInstanceRegistry.h"

#include <memory>
#include <string>
#include "ParticleEditorInstance.h"
#include "ParticlePalette.h"
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
        class ParticleData;

        struct POD_Particle
        {
            Particle_TemplateData* templateData = nullptr;
            std::string name = "World Particle";
            POD_Vector3<float> position = POD_Vector3<float>();
            POD_Quaternion<double> rotation = POD_Quaternion<double>();
            POD_Vector3<float> scale = { 1,1,1 };

            TO_JSON(POD_Particle)
            FROM_JSON(POD_Particle)
        };

        class ParticleData
            : public IEditableData, public StaticInstanceRegistry<ParticleData>
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
            virtual palette::PaletteInstance* ApplyAsPaletteInstance() override;
            virtual void ApplyAsPlaytimeObject() override;
            virtual bool EnterDataFromGlobalConstant() override;

            POD_Particle pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static TemplateDataManager& templateDataManager;
            palette::ParticleEditorInstance* particleInstance{ nullptr };

            ParticleData();
            ParticleData(const std::string& name);
            ParticleData(const ParticleData& prototype);
            ParticleData& operator=(const ParticleData& prototype);
            virtual ~ParticleData();
        };
    }
}
