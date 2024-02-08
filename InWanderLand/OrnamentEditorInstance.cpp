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
		obj->setName("FBX");
		obj->SetParent(GetGameObject());
	}

	void OrnamentEditorInstance::ChangeTemplateData(const application::editor::OrnamentData* ornamentData)
	{
		ChangeTemplateData(ornamentData->pod.templateData);
	}

	void OrnamentEditorInstance::ChangeTemplateData(const application::editor::Ornament_TemplateData* ornamentTemplateData)
	{
		if (this->ornamentTemplateData == ornamentTemplateData)
			return;

		this->ornamentTemplateData = ornamentTemplateData;
		for (auto& each : GetGameObject()->GetChildren())
		{
			if (each->getName() == "FBX")
			{
				yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(each);
				break;
			}
		}
		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(ornamentTemplateData->pod.fbxName);
		obj->setName("FBX");
		obj->SetParent(GetGameObject());
	}
}

