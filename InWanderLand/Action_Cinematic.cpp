#include "Action_Cinematic.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "UIManager.h"
#include "UIElement.h"

#include "CinematicManager.h"

namespace application
{
	CoroutineObject<void> Action_CinematicModeChange::DoAction()
	{
		CinematicManager::Instance().SetCinematicMode(cinematicMode);
		co_return;
	}

	void Action_CinematicModeChange::SetCinematicMode(bool cinematicMode)
	{
		this->cinematicMode = cinematicMode;
	}

	void Action_CinematicModeChange::ImGui_DrawDataPopup(Action_CinematicModeChange* data)
	{
		if (ImGui::MenuItem("SetCinematicMode"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool cinematicMode = false;
			cinematicMode = data->cinematicMode;
			editor::imgui::ShowMessageBox("SetCinematicMode", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("Cinematic Mode", &cinematicMode);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetCinematicMode(cinematicMode);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetCinematicMode");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetCinematicMode");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_CinematicModeChange::PreEncoding(json& data) const
	{
		data["cinematicMode"] = cinematicMode;
		return true;
	}

	bool Action_CinematicModeChange::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_CinematicModeChange::PreDecoding(const json& data)
	{
		cinematicMode = data["cinematicMode"];
		return true;
	}

	bool Action_CinematicModeChange::PostDecoding(const json& data)
	{
		return true;
	}

	CoroutineObject<void> Action_CinematicFadeIn::DoAction()
	{
		UIManager::Instance().FadeIn();
		co_return;
	}

	void Action_CinematicFadeIn::ImGui_DrawDataPopup(Action_CinematicFadeIn* data)
	{
		
	}

	bool Action_CinematicFadeIn::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_CinematicFadeIn::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_CinematicFadeIn::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_CinematicFadeIn::PostDecoding(const json& data)
	{
		return true;
	}

	CoroutineObject<void> Action_CinematicFadeOut::DoAction()
	{
		UIElement* fadingElement{ nullptr };
		/// FadeOut
		switch (direction)
		{
			case application::FadeDirection::RIGHT:
				fadingElement = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_RightToLeft);
				break;
			case application::FadeDirection::LEFT:
				fadingElement = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_LeftToRight);
				break;
			case application::FadeDirection::UP:
				fadingElement = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_TopToBottom);
				break;
			case application::FadeDirection::DOWN:
				fadingElement = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_BottomToTop);
				break;
			default:
				break;
		}
		fadingElement->EnableElement();
		co_return;
	}

	void Action_CinematicFadeOut::SetFadeDirection(FadeDirection direction)
	{
		this->direction = direction;
	}

	void Action_CinematicFadeOut::ImGui_DrawDataPopup(Action_CinematicFadeOut* data)
	{
		if (ImGui::MenuItem("SetFadeOutDirection"))
		{
			editor::EditorLayer::SetInputControl(false);
			static FadeDirection direction = FadeDirection::RIGHT;
			direction = data->direction;
			static std::string dirName = "Right";
			switch (direction)
			{
				case FadeDirection::RIGHT:
				{
					dirName = "Right";
					break;
				}
				case FadeDirection::LEFT:
				{
					dirName = "Left";
					break;
				}
				case FadeDirection::UP:
				{
					dirName = "Up";
					break;
				}
				case FadeDirection::DOWN:
				{
					dirName = "Down";
					break;
				}
				default:
					break;
			}

			editor::imgui::ShowMessageBox("SetFadeOutDirection", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);

					static std::vector<std::string> dirList = { "Right", "Left", "Up", "Down" };

					if (ImGui::BeginCombo("##FadeOutDirection", dirName.c_str()))
					{
						for (auto& str : dirList)
						{
							const bool is_selected = (dirName == str);
							if (ImGui::Selectable(str.c_str(), is_selected))
							{
								dirName = str;
							}

							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						if (dirName == "Right")
						{
							direction = FadeDirection::RIGHT;
						}
						else if (dirName == "Left")
						{
							direction = FadeDirection::LEFT;
						}
						else if (dirName == "Up")
						{
							direction = FadeDirection::UP;
						}
						else if (dirName == "Down")
						{
							direction = FadeDirection::DOWN;
						}

						data->SetFadeDirection(direction);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetFadeOutDirection");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetFadeOutDirection");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_CinematicFadeOut::PreEncoding(json& data) const
	{
		data["direction"] = direction;
		return true;
	}

	bool Action_CinematicFadeOut::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_CinematicFadeOut::PreDecoding(const json& data)
	{
		direction = data["direction"];
		return true;
	}

	bool Action_CinematicFadeOut::PostDecoding(const json& data)
	{
		return true;
	}
}
