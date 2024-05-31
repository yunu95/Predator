/// 2024. 05. 29
/// Interactable 중에서 Active 인 경우에 사용할 UI Component 입니다.

#pragma once

#include "YunutyEngine.h"

class Interactable_UI
	: public Component
{
public:
	virtual void Start() override
	{
		auto rsc = GetGameObject()->AddComponent<graphics::UIImage>();
		const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		std::wstring texturePath;
		texturePath.assign(guideUI.begin(), guideUI.end());
		auto texture = resourceManager->GetTexture(texturePath);
		rsc->GetGI().SetImage(texture);

		if (ui_Width < 0)
		{
			rsc->GetGI().SetWidth(texture->GetWidth());
		}
		else
		{
			rsc->GetGI().SetWidth(ui_Width);
		}

		if (ui_Height < 0)
		{
			rsc->GetGI().SetHeight(texture->GetHeight());
		}
		else
		{
			rsc->GetGI().SetHeight(ui_Height);
		}

		rsc->GetGI().SetXPivot(0.5);
		rsc->GetGI().SetYPivot(0.5);
		GetGameObject()->SetSelfActive(false);
	}

	void UpdateUI()
	{
		auto rsc = GetGameObject()->GetComponent<graphics::UIImage>();
		if (rsc == nullptr)
		{
			return;
		}

		const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		std::wstring texturePath;
		texturePath.assign(guideUI.begin(), guideUI.end());
		auto texture = resourceManager->GetTexture(texturePath);
		rsc->GetGI().SetImage(texture);

		if (ui_Width < 0)
		{
			rsc->GetGI().SetWidth(texture->GetWidth());
		}
		else
		{
			rsc->GetGI().SetWidth(ui_Width);
		}

		if (ui_Height < 0)
		{
			rsc->GetGI().SetHeight(texture->GetHeight());
		}
		else
		{
			rsc->GetGI().SetHeight(ui_Height);
		}
	}

	void SetUI(const std::string& ui)
	{
		guideUI = ui;
	}

	void SetUIWidth(float width)
	{
		ui_Width = width;
	}

	void SetUIHeight(float height)
	{
		ui_Height = height;
	}

private:
	std::string guideUI = "Texture/Interactable/UI_Interaction.png";
	float ui_Width = -1;
	float ui_Height = -1;
};
