#include "ParticleEditorInstance.h"

#include "InWanderLand.h"
#include "ParticleData.h"
#include "EditorResourceManager.h"
#include "TemplateDataManager.h"

namespace application::editor::palette
{
	ParticleEditorInstance::~ParticleEditorInstance()
	{
		if (particleObj)
		{
			Scene::getCurrentScene()->DestroyGameObject(particleObj);
			fbxObj = nullptr;
			particleObj = nullptr;
		}
	}

	void ParticleEditorInstance::Start()
	{
		PaletteInstance::Start();
	}

	void ParticleEditorInstance::Init(const application::editor::ParticleData* particleData)
	{
		Init(particleData->pod.templateData);
	}

	void ParticleEditorInstance::Init(const application::editor::Particle_TemplateData* particleTemplateData)
	{
		this->particleTemplateData = particleTemplateData;

		/// TemplateData 의 Key 로 particleObj 생성
		//particleObj

		yunuGI::Vector3 boundingMin, boundingMax;
		fbxObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Cube", &boundingMin, &boundingMax);
		AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
		fbxObj->SetParent(GetGameObject());

		auto& erm = ResourceManager::GetSingletonInstance();

		for (auto each : fbxObj->GetChildren())
		{
			auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

			if (comp)
			{
				for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
				{
					comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
					comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0.3,1,0.1 });
				}
			}
		}
	}

	void ParticleEditorInstance::ChangeTemplateData(const application::editor::ParticleData* particleData)
	{
		ChangeTemplateData(particleData->pod.templateData);
	}

	void ParticleEditorInstance::ChangeTemplateData(const application::editor::Particle_TemplateData* particleTemplateData)
	{
		if (this->particleTemplateData == particleTemplateData)
			return;
		this->particleTemplateData = particleTemplateData;

		yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(particleObj);

		/// TemplateData 의 Key 로 particleObj 생성
		//particleObj
	}

	void ParticleEditorInstance::ApplyParticleComponent(ParticleData* data)
	{
		if (particleObj)
		{
			particleObj->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
			particleObj->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
			particleObj->GetTransform()->SetLocalScale(GetTransform()->GetWorldScale());

			/// particle 관련 설정들 적용
		}
	}

	void ParticleEditorInstance::ShowEditorInstance()
	{
		GetGameObject()->SetSelfActive(true);
		if (particleObj)
		{
			particleObj->SetSelfActive(true);
		}
	}

	void ParticleEditorInstance::HideEditorInstance()
	{
		GetGameObject()->SetSelfActive(false);
		if (particleObj)
		{
			particleObj->SetSelfActive(false);
		}
	}
}

