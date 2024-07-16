#include "InWanderLand.h"
#include "ParticleData.h"
#include "RegionData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "EditorResourceManager.h"

namespace application
{
    namespace editor
    {
        TemplateDataManager& ParticleData::templateDataManager = TemplateDataManager::GetSingletonInstance();

        bool ParticleData::EnterDataFromTemplate()
        {
            // 템플릿으로부터 초기화되는 데이터들 처리 영역	

            return true;
        }

        ITemplateData* ParticleData::GetTemplateData()
        {
            return pod.templateData;
        }

        bool ParticleData::SetTemplateData(const std::string& dataName)
        {
            auto ptr = templateDataManager.GetTemplateData(dataName);
            if (ptr == nullptr)
            {
                return false;
            }

            pod.templateData = static_cast<Particle_TemplateData*>(ptr);

            return true;
        }

        IEditableData* ParticleData::Clone() const
        {
            auto& imanager = InstanceManager::GetSingletonInstance();
            auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

            if (instance != nullptr)
            {
                static_cast<ParticleData*>(instance)->pod = pod;
            }

            return instance;
        }

        void ParticleData::OnRelocate(const Vector3d& newLoc)
        {
            pod.position.x = newLoc.x;
            pod.position.y = newLoc.y;
            pod.position.z = newLoc.z;
        }

        void ParticleData::OnRerotate(const Quaternion& newRot)
        {
            pod.rotation.x = newRot.x;
            pod.rotation.y = newRot.y;
            pod.rotation.z = newRot.z;
            pod.rotation.w = newRot.w;
        }

        void ParticleData::OnRescale(const Vector3d& newScale)
        {
            pod.scale.x = newScale.x;
            pod.scale.y = newScale.y;
            pod.scale.z = newScale.z;
        }

        palette::PaletteInstance* ParticleData::ApplyAsPaletteInstance()
        {
            if (GetPaletteInstance() == nullptr)
            {
                particleInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::ParticleEditorInstance>();
                SetPaletteInstance(particleInstance);
                particleInstance->SetEditableData(this);
                particleInstance->Init(this);
            }
            particleInstance->GetTransform()->SetWorldPosition({ pod.position.x,pod.position.y,pod.position.z });
            particleInstance->GetTransform()->SetWorldRotation({ pod.rotation.w, pod.rotation.x, pod.rotation.y, pod.rotation.z });
            particleInstance->GetTransform()->SetLocalScale({ pod.scale.x,pod.scale.y,pod.scale.z });
            particleInstance->ApplyParticleComponent(this);
            return particleInstance;
        }

        void ParticleData::ApplyAsPlaytimeObject()
        {
            ApplyAsPaletteInstance();
            particleInstance->GetGameObject()->SetSelfActive(false);
            if (!pod.isGuide && tookAction)
            {
                particleInstance->ShowParticleObject();
                auto pt = particleInstance->GetParticleObject()->GetTransform();
                pt->SetWorldPosition({ pod.position.x,pod.position.y,pod.position.z });
                pt->SetWorldRotation({ pod.rotation.w, pod.rotation.x, pod.rotation.y, pod.rotation.z });
                pt->SetLocalScale({ pod.scale.x,pod.scale.y,pod.scale.z });
                tookAction = false;
            }
        }

        bool ParticleData::EnterDataFromGlobalConstant()
        {
            auto& data = GlobalConstant::GetSingletonInstance().pod;
            return true;
        }

        bool ParticleData::PreEncoding(json& data) const
        {
            FieldPreEncoding<boost::pfr::tuple_size_v<POD_Particle>>(pod, data["POD"]);

            return true;
        }

        bool ParticleData::PostEncoding(json& data) const
        {
            FieldPostEncoding<boost::pfr::tuple_size_v<POD_Particle>>(pod, data["POD"]);

            return true;
        }

        bool ParticleData::PreDecoding(const json& data)
        {
            FieldPreDecoding<boost::pfr::tuple_size_v<POD_Particle>>(pod, data["POD"]);

            return true;
        }

        bool ParticleData::PostDecoding(const json& data)
        {
            FieldPostDecoding<boost::pfr::tuple_size_v<POD_Particle>>(pod, data["POD"]);
            EnterDataFromGlobalConstant();
#ifdef EDITOR
            ApplyAsPaletteInstance();
#endif
            return true;
        }

        ParticleData::ParticleData()
            : pod()
        {

        }

        ParticleData::ParticleData(const std::string& name)
            : pod()
        {
            pod.templateData = static_cast<Particle_TemplateData*>(templateDataManager.GetTemplateData(name));
            EnterDataFromTemplate();
            EnterDataFromGlobalConstant();
        }

        ParticleData::ParticleData(const ParticleData& prototype)
            : pod(prototype.pod)
        {

        }

        ParticleData& ParticleData::operator=(const ParticleData& prototype)
        {
            IEditableData::operator=(prototype);
            pod = prototype.pod;
            return *this;
        }

        ParticleData::~ParticleData()
        {

        }
    }
}
