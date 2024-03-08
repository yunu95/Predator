#include "OrnamentEditorInstance.h"

#include "InWanderLand.h"
#include "OrnamentData.h"

namespace application::editor::palette
{
	void OrnamentEditorInstance::Start()
	{
		PaletteInstance::Start();
	}

	void OrnamentEditorInstance::Init(const application::editor::OrnamentData* ornamentData)
	{
		Init(ornamentData->pod.templateData);
	}

	void OrnamentEditorInstance::Init(const application::editor::Ornament_TemplateData* ornamentTemplateData)
	{
		this->ornamentTemplateData = ornamentTemplateData;
		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(ornamentTemplateData->pod.fbxName);
		obj->SetParent(GetGameObject());
		currentFBX = ornamentTemplateData->pod.fbxName;
	}

	void OrnamentEditorInstance::ChangeTemplateData(const application::editor::OrnamentData* ornamentData)
	{
		ChangeTemplateData(ornamentData->pod.templateData);
	}

	void OrnamentEditorInstance::ChangeTemplateData(const application::editor::Ornament_TemplateData* ornamentTemplateData)
	{
		if (ornamentTemplateData == nullptr)
			return;
		this->ornamentTemplateData = ornamentTemplateData;
		ChangeResource(ornamentTemplateData->pod.fbxName);
	}

	void OrnamentEditorInstance::ChangeResource(const std::string& fbxName)
	{
		// TemplateData 를 유지하고 Resource 만 갱신함
		if (currentFBX == fbxName)
			return;

		for (auto& each : GetGameObject()->GetChildren())
		{
			if (each->getName() == currentFBX)
			{
				yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(each);
				break;
			}
		}

		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName);
		obj->SetParent(GetGameObject());
		currentFBX = fbxName;

		return;
	}
}

