#include "ParticleEditorInstance.h"

#include "InWanderLand.h"
#include "ParticleData.h"
#include "EditorResourceManager.h"
#include "TemplateDataManager.h"
#include "Application.h"
#include "ContentsLayer.h"

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

		if (particleData->pod.isGuide)
		{
			ChangeGuideInstance();
		}
	}

	void ParticleEditorInstance::Init(const application::editor::Particle_TemplateData* particleTemplateData)
	{
		this->particleTemplateData = particleTemplateData;
		yunuGI::Vector3 boundingMin, boundingMax;
		fbxObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Cylinder", &boundingMin, &boundingMax);
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
					comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 0, 0.5, 1, 0.5 });
				}
			}
		}

		particleObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		particleObj->AddComponent<graphics::ParticleRenderer>();
	}

	void ParticleEditorInstance::ApplyParticleComponent(ParticleData* data)
	{
		if (particleObj)
		{
			particleObj->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
			particleObj->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
			particleObj->GetTransform()->SetLocalScale(GetTransform()->GetWorldScale());

			auto pComp = particleObj->GetComponent<graphics::ParticleRenderer>();

			if (pComp)
			{
				pComp->SetParticleShape((yunutyEngine::graphics::ParticleShape)data->pod.particleData.shape);
				pComp->SetParticleMode((yunutyEngine::graphics::ParticleMode)data->pod.particleData.particleMode);
				pComp->SetLoop(data->pod.particleData.isLoop);
				pComp->SetDuration(data->pod.particleData.duration);
				pComp->SetLifeTime(data->pod.particleData.lifeTime);
				pComp->SetSpeed(data->pod.particleData.speed);
				pComp->SetStartScale(data->pod.particleData.startScale);
				pComp->SetEndScale(data->pod.particleData.endScale);
				pComp->SetMaxParticle(data->pod.particleData.maxParticle);
				pComp->SetPlayAwake(data->pod.particleData.playAwake);
				pComp->SetIsApplyRoot(data->pod.particleData.isApplyRoot);
				pComp->SetRadius(data->pod.particleData.radius);
				pComp->SetAngle(data->pod.particleData.angle);

				pComp->SetRateOverTime(data->pod.particleData.rateOverTime);

				pComp->SetBurstsCount(data->pod.particleData.burstsCount);
				pComp->SetInterval(data->pod.particleData.interval);

				pComp->SetStartAngle(data->pod.particleData.startAngle);
				pComp->SetEndAngle(data->pod.particleData.endAngle);
				pComp->SetIsRandomScale(data->pod.particleData.isRandomScale);
				pComp->SetIsRandomAngle(data->pod.particleData.isRandomAngle);
				pComp->SetIsAlphaDiminish(data->pod.particleData.isAlphaDiminish);

				static const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

				std::wstring texturePath;
				texturePath.assign(data->pod.particleData.texturePath.begin(), data->pod.particleData.texturePath.end());
				auto texturePtr = resourceManager->GetTexture(texturePath);
				if (texturePtr)
				{
					pComp->SetTexture(texturePtr);
				}
			}
		}
	}

	void ParticleEditorInstance::ShowEditorInstance()
	{
		if (isGuide)
		{
			return;
		}

		GetGameObject()->SetSelfActive(true);
		if (!Application::GetInstance().IsContentsPlaying())
		{
			if (particleObj)
			{
				auto ts = particleObj->GetTransform();
				ts->SetWorldPosition(GetTransform()->GetWorldPosition());
				ts->SetWorldRotation(GetTransform()->GetWorldRotation());
				ts->SetWorldScale(GetTransform()->GetWorldScale());
				particleObj->SetSelfActive(true);
				particleObj->GetComponent<graphics::ParticleRenderer>()->Play();
			}
		}
	}

	void ParticleEditorInstance::HideEditorInstance()
	{
		if (isGuide)
		{
			return;
		}

		GetGameObject()->SetSelfActive(false);	
	}

	void ParticleEditorInstance::ChangeGuideInstance()
	{
		GetGameObject()->SetSelfActive(false);
		if (particleObj)
		{
			particleObj->SetSelfActive(false);
		}

		isGuide = true;
	}

	void ParticleEditorInstance::ShowParticleObject()
	{
		if (particleObj)
		{
			particleObj->SetSelfActive(true);
			particleObj->GetComponent<graphics::ParticleRenderer>()->Play();
		}
	}

	void ParticleEditorInstance::HideParticleObject()
	{
		if (particleObj)
		{
			particleObj->SetSelfActive(false);
		}
	}

	GameObject* ParticleEditorInstance::GetParticleObject()
	{
		return particleObj;
	}
}

