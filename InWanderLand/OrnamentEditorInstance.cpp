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
	}
}

