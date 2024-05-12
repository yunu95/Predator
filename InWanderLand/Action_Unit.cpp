#include "Action_Unit.h"

#include "InWanderLand.h"
#include "UnitData.h"
#include "UnitEditorInstance.h"

#include "Application.h"
#include "EditorLayer.h"

#include "EditorPopupManager.h"
#include "InstanceManager.h"

#include "YunutyEngine.h"

namespace application
{
	Action_UnitMove::~Action_UnitMove()
	{
		if (targetUnit)
		{
			targetUnit->RemoveObserver(this);
		}

		if (destinationUnit)
		{
			destinationUnit->RemoveObserver(this);
			editor::InstanceManager::GetSingletonInstance().DeleteInstance(destinationUnit->GetUUID());
		}
	}

	CoroutineObject<void> Action_UnitMove::DoAction()
	{
		auto ts = targetUnit->inGameUnit->GetTransform();
		auto startPos = ts->GetWorldPosition();
		auto startRot = ts->GetWorldRotation();
		auto startScale = ts->GetWorldScale();

		Vector3d endPos = { destinationUnit->pod.position.x, destinationUnit->pod.position.y, destinationUnit->pod.position.z };
		Quaternion endRot = { destinationUnit->pod.rotation.w, destinationUnit->pod.rotation.x, destinationUnit->pod.rotation.y, destinationUnit->pod.rotation.z };
		Vector3d endScale = { destinationUnit->pod.scale.x, destinationUnit->pod.scale.y, destinationUnit->pod.scale.z };

		endPos.y = 0;

		targetUnit->inGameUnit->OrderMove(endPos);
		co_return;

		/*double timer = 0;
		float factor = 0;

		if (lerpTime == 0)
		{
			ts->SetWorldPosition(endPos);
			ts->SetWorldRotation(endRot);
			ts->SetWorldScale(endScale);
			co_return;
		}

		for (double timer = 0; timer < lerpTime;)
		{
			factor = timer / lerpTime;
			ts->SetWorldPosition(Vector3d::Lerp(startPos, endPos, factor));
			ts->SetWorldRotation(Quaternion::Lerp(startRot, endRot, factor));
			ts->SetWorldScale(Vector3d::Lerp(startScale, endScale, factor));
			timer += Time::GetDeltaTimeUnscaled();
			co_await std::suspend_always();
		}*/
	}

	bool Action_UnitMove::IsValid()
	{
		return (targetUnit == nullptr) ? false : (destinationUnit == nullptr) ? false : true;
	}

	void Action_UnitMove::SetTargetUnit(editor::UnitData* unit)
	{
		if (targetUnit)
		{
			targetUnit->RemoveObserver(this);
		}

		targetUnit = unit;
		if (unit)
		{
			unit->RegisterObserver(this);
		}

		if (destinationUnit)
		{
			editor::InstanceManager::GetSingletonInstance().DeleteInstance(destinationUnit->GetUUID());
		}
	}

	void Action_UnitMove::SetDestinationUnit(editor::UnitData* unit)
	{
		if (destinationUnit)
		{
			destinationUnit->RemoveObserver(this);
		}

		destinationUnit = unit;
		if (unit)
		{
			unit->RegisterObserver(this);
			if (destinationUnit->GetPaletteInstance())
			{
				unit->pod.isGuide = true;
				static_cast<editor::palette::UnitEditorInstance*>(destinationUnit->GetPaletteInstance())->ChangeGuideInstance();
			}
		}
	}

	void Action_UnitMove::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetUnit == static_cast<editor::UnitData*>(target))
				{
					targetUnit = nullptr;
				}
				if (destinationUnit == static_cast<editor::UnitData*>(target))
				{
					destinationUnit = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Action_UnitMove::ImGui_DrawDataPopup(Action_UnitMove* data)
	{
		if (ImGui::MenuItem("SetTargetUnit(Move)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetUnit(Move)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetUnit)
					{
						ImGui::Text(data->targetUnit->pod.templateData->pod.skinnedFBXName.c_str());
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(Move)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitMove>("SetTargetUnit(Move)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(Move)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (data->destinationUnit)
		{
			if (ImGui::MenuItem("EditDestinationUnit(Move)"))
			{
				editor::EditorLayer::SetInputControl(false);
				editor::imgui::ShowMessageBox("EditDestinationUnit(Move)", [data]()
					{
						editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

						ImGui::Separator();

						ImGui::SetNextItemWidth(-1);

						ImGui::Text(data->destinationUnit->pod.templateData->pod.skinnedFBXName.c_str());

						ImGui::Separator();

						if (ImGui::Button("Edit"))
						{
							data->destinationUnit->GetPaletteInstance()->GetGameObject()->SetSelfActive(true);
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("EditDestinationUnit(Move)");
							editor::EditorLayer::SetInputControl(true);
							editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitMove>("EditDestinationUnit(Move)", data);
						}
						ImGui::SameLine();

						if (ImGui::Button("Cancel"))
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("EditDestinationUnit(Move)");
							editor::EditorLayer::SetInputControl(true);
						}
					}, 300);
			}
		}
		else
		{
			if (data->targetUnit && ImGui::MenuItem("SetDestinationUnit(Move)"))
			{
				editor::EditorLayer::SetInputControl(false);
				editor::imgui::ShowMessageBox("SetDestinationUnit(Move)", [data]()
					{
						if (data->destinationUnit)
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("SetDestinationUnit(Move)");
							editor::EditorLayer::SetInputControl(true);
							return;
						}

						editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

						ImGui::Separator();

						ImGui::SetNextItemWidth(-1);

						ImGui::Text("------");

						ImGui::Separator();

						if (ImGui::Button("Edit"))
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("SetDestinationUnit(Move)");
							editor::EditorLayer::SetInputControl(true);
							editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitMove>("SetDestinationUnit(Move)", data);
						}
						ImGui::SameLine();

						if (ImGui::Button("Cancel"))
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("SetDestinationUnit(Move)");
							editor::EditorLayer::SetInputControl(true);
						}
					}, 300);
			}
		}
	}

	bool Action_UnitMove::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_UnitMove::PostEncoding(json& data) const
	{
		data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
		data["destinationUnit"] = destinationUnit ? UUID_To_String(destinationUnit->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_UnitMove::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_UnitMove::PostDecoding(const json& data)
	{
		SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
		SetDestinationUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["destinationUnit"])));
		return true;
	}
}