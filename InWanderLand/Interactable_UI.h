/// 2024. 05. 29
/// Interactable 중에서 Active 인 경우에 사용할 UI Component 입니다.
/// 단순히 통일된 UI Texture 를 제공하기 위해서 만든 Component 로, 특별한 기능을 수행하지 않습니다.

#pragma once

#include "YunutyEngine.h"

class Interactable_UI
	: public Component
{
	virtual void Start() override
	{
		auto rsc = GetGameObject()->AddComponent<graphics::UIImage>();
		const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		auto texture = resourceManager->GetTexture(L"Texture/Interactable/UI_Interaction.png");
		rsc->GetGI().SetImage(texture);
		rsc->GetGI().SetWidth(texture->GetWidth() / 2);
		rsc->GetGI().SetHeight(texture->GetHeight() / 2);
		rsc->GetGI().SetXPivot(0.5);
		rsc->GetGI().SetYPivot(0.5);
		GetGameObject()->SetSelfActive(false);
	}
};
