#include "CameraEditorInstance.h"

#include "InWanderLand.h"
#include "CameraData.h"

namespace application::editor::palette
{
	void CameraEditorInstance::Start()
	{
		PaletteInstance::Start();
	}

	void CameraEditorInstance::Init(const application::editor::CameraData* cameraData)
	{
		Init(cameraData->pod.templateData);
	}

	void CameraEditorInstance::Init(const application::editor::Camera_TemplateData* cameraTemplateData)
	{
		this->cameraTemplateData = cameraTemplateData;
		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Stone_001");
		obj->SetParent(GetGameObject());
	}
}

