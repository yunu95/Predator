/// 2024. 04. 11 김상준
/// Script 편집을 위한 Editor Module

#pragma once

#include "Singleton.h"
#include "EditorModule.h"

#include "ScriptSystem.h"
#include "TriggerList.h"
#include "ConditionList.h"
#include "ActionList.h"

#include "imgui.h"
#include "imgui_ModuleData.h"

#include <memory>

namespace application
{
	class Script;
}

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor
			: public EditorModule, public Singleton<Module_ScriptEditor>
		{
			friend class Singleton<Module_ScriptEditor>;

		public:
			virtual ~Module_ScriptEditor();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

			void Clear();

			Script* GetSelectedScript();

		private:
			Module_ScriptEditor();

			// Script 리스트와 선택한 리스트에 대한 내용을 출력하는 두 패널을 그려주는 함수
			void DrawLayout();
			// Script 리스트를 그리는 함수
			void DrawList(const ImVec2& region);
			// 선택한 Script 의 내용을 출력하는 패널을 그리는 함수
			void DrawScriptPanel(const ImVec2& region);
			// Script 내용을 출력하는 함수
			void DrawScript(Script* data);

			void ImGui_AddTriggerPopup();
			void ImGui_AddConditionPopup();
			void ImGui_AddActionPopup();

			void DrawTrigger(std::shared_ptr<ITrigger> data);
			void DrawCondition(std::shared_ptr<ICondition> data);
			void DrawAction(std::shared_ptr<IAction> data);

			template <typename T> requires std::is_same_v<ITrigger, T> || std::is_same_v<ICondition, T> || std::is_same_v<IAction, T>
			void PopUpDataEdit(T* data)
			{

			}

			template <>
			void PopUpDataEdit(ITrigger* data)
			{
				auto type = data->GetType();
				auto& str = ScriptSystem::triggerList[type];
				if (ImGui::BeginPopup(str.c_str()))
				{
					switch (type)
					{
						case application::TriggerType::GameStart:
						{
							Trigger_GameStart::ImGui_DrawDataPopup(static_cast<Trigger_GameStart*>(data));
							break;
						}
						case application::TriggerType::EnterRegion:
						{
							Trigger_EnterRegion::ImGui_DrawDataPopup(static_cast<Trigger_EnterRegion*>(data));
							break;
						}
						case application::TriggerType::LeaveRegion:
						{
							Trigger_LeaveRegion::ImGui_DrawDataPopup(static_cast<Trigger_LeaveRegion*>(data));
							break;
						}
						default:
							break;
					}
					ImGui::EndPopup();
				}
			}

			template <>
			void PopUpDataEdit(ICondition* data)
			{
				auto type = data->GetType();
				auto& str = ScriptSystem::conditionList[type];
				if (ImGui::BeginPopup(str.c_str()))
				{
					switch (type)
					{
						default:
							break;
					}
					ImGui::EndPopup();
				}
			}

			template <>
			void PopUpDataEdit(IAction* data)
			{
				auto type = data->GetType();
				auto& str = ScriptSystem::actionList[type];
				if (ImGui::BeginPopup(str.c_str()))
				{
					switch (type)
					{
						case application::ActionType::WaitForSeconds:
						{
							Action_WaitForSeconds::ImGui_DrawDataPopup(static_cast<Action_WaitForSeconds*>(data));
							break;
						}
						case application::ActionType::WaitForRealSeconds:
						{
							Action_WaitForRealSeconds::ImGui_DrawDataPopup(static_cast<Action_WaitForRealSeconds*>(data));
							break;
						}
						case application::ActionType::CinematicModeChange:
						{
							Action_CinematicModeChange::ImGui_DrawDataPopup(static_cast<Action_CinematicModeChange*>(data));
							break;
						}
						default:
							break;
					}
					ImGui::EndPopup();
				}
			}

			Script* selectedScript = nullptr;
			std::shared_ptr<ITrigger> selectedTrigger = nullptr;
			std::shared_ptr<ICondition> selectedCondition = nullptr;
			std::shared_ptr<IAction> selectedAction = nullptr;
		};
	}
}
