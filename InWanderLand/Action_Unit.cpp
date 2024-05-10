#include "Action_Unit.h"

#include "InWanderLand.h"
#include "UnitData.h"

#include "Application.h"
#include "EditorLayer.h"

#include "EditorPopupManager.h"

#include "YunutyEngine.h"

namespace application
{
	Action_UnitMove::~Action_UnitMove()
	{
		if (targetUnit)
		{
			targetUnit->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_UnitMove::DoAction()
	{
		auto ts = targetUnit->inGameUnit->GetTransform();
		auto startPos = ts->GetWorldPosition();
		auto startRot = ts->GetWorldRotation();
		auto startScale = ts->GetWorldScale();

		Vector3d endPos = { position.x, position.y, position.z };
		Quaternion endRot = { rotation.w, rotation.x, rotation.y, rotation.z };
		Vector3d endScale = { scale.x, scale.y, scale.z };

		double timer = 0;
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
		}
	}

	bool Action_UnitMove::IsValid()
	{
		return (targetUnit == nullptr) ? false : destinationSetting;
	}

	void Action_UnitMove::SetUnit(editor::UnitData* unit)
	{
		if (targetUnit)
		{
			targetUnit->RemoveObserver(this);
		}

		targetUnit = unit;
		unit->RegisterObserver(this);
		destinationSetting = false;
	}

	void Action_UnitMove::SetDestinationUnit(editor::UnitData* unit)
	{
		container_pos = unit->pod.position;
		yunutyEngine::Quaternion quat;
		quat.x = unit->pod.rotation.x;
		quat.y = unit->pod.rotation.y;
		quat.z = unit->pod.rotation.z;
		quat.w = unit->pod.rotation.w;
		container_rot.x = quat.Euler().x;
		container_rot.y = quat.Euler().y;
		container_rot.z = quat.Euler().z;
		container_scal = unit->pod.scale;
	}

	void Action_UnitMove::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
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
				break;
			}
			default:
				break;
		}
	}

	void Action_UnitMove::ImGui_DrawDataPopup(Action_UnitMove* data)
	{
		if (ImGui::MenuItem("SetTargetUnit"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetUnit", [data]()
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
						editor::imgui::CloseMessageBox("SetTargetUnit");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitMove>("SetTargetUnit", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetDestination(Unit)"))
		{
			editor::EditorLayer::SetInputControl(false);
			data->container_pos = data->position;
			yunutyEngine::Quaternion quat;
			quat.x = data->rotation.x;
			quat.y = data->rotation.y;
			quat.z = data->rotation.z;
			quat.w = data->rotation.w;
			data->container_rot.x = quat.Euler().x;
			data->container_rot.y = quat.Euler().y;
			data->container_rot.z = quat.Euler().z;
			data->container_scal = data->scale;
			editor::imgui::ShowMessageBox("SetDestination(Unit)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					int idx = 0;

					if (editor::imgui::BeginSection_1Col(idx, "TransForm", ImGui::GetContentRegionAvail().x))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						bool reset[9] = { false };

						auto resetPosition = editor::imgui::Vector3Control("Position", data->container_pos.x, data->container_pos.y, data->container_pos.z);
						auto resetRotation = editor::imgui::Vector3Control("Rotation", data->container_rot.x, data->container_rot.y, data->container_rot.z);
						auto resetScale = editor::imgui::Vector3Control("Scale", data->container_scal.x, data->container_scal.y, data->container_scal.z);

						switch (resetPosition)
						{
							case application::editor::imgui::Vector3Flags::ResetX:
								reset[0] = true;
								break;
							case application::editor::imgui::Vector3Flags::ResetY:
								reset[1] = true;
								break;
							case application::editor::imgui::Vector3Flags::ResetZ:
								reset[2] = true;
								break;
							default:
								break;
						}

						switch (resetRotation)
						{
							case application::editor::imgui::Vector3Flags::ResetX:
								reset[3] = true;
								break;
							case application::editor::imgui::Vector3Flags::ResetY:
								reset[4] = true;
								break;
							case application::editor::imgui::Vector3Flags::ResetZ:
								reset[5] = true;
								break;
							default:
								break;
						}

						switch (resetScale)
						{
							case application::editor::imgui::Vector3Flags::ResetX:
								reset[6] = true;
								break;
							case application::editor::imgui::Vector3Flags::ResetY:
								reset[7] = true;
								break;
							case application::editor::imgui::Vector3Flags::ResetZ:
								reset[8] = true;
								break;
							default:
								break;
						}

						if (reset[0])
						{
							data->container_pos.x = 0;
						}
						else if (reset[1])
						{
							data->container_pos.y = 0;
						}
						else if (reset[2])
						{
							data->container_pos.z = 0;
						}
						else if (reset[3])
						{
							data->container_rot.x = 0;
						}
						else if (reset[4])
						{
							data->container_rot.y = 0;
						}
						else if (reset[5])
						{
							data->container_rot.z = 0;
						}
						else if (reset[6])
						{
							data->container_scal.x = 1;
						}
						else if (reset[7])
						{
							data->container_scal.y = 1;
						}
						else if (reset[8])
						{
							data->container_scal.z = 1;
						}
					}

					ImGui::SetNextItemWidth(-1);

					if (ImGui::Button("SetFromUnit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetDestination(Unit)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitMove>("SetDestination(Unit)", data);
					}

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->UpdateDestinationFromContainer();
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetDestination(Unit)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetDestination(Unit)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetLerpTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float lerpTime = 0;
			lerpTime = data->lerpTime;
			editor::imgui::ShowMessageBox("SetLerpTime(UnitMove)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTimeUnitMove", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(UnitMove)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(UnitMove)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_UnitMove::PreEncoding(json& data) const
	{
		data["lerpTime"] = lerpTime;
		data["position"]["x"] = position.x;
		data["position"]["y"] = position.y;
		data["position"]["z"] = position.z;
		data["rotation"]["x"] = rotation.x;
		data["rotation"]["y"] = rotation.y;
		data["rotation"]["z"] = rotation.z;
		data["rotation"]["w"] = rotation.w;
		data["scale"]["x"] = scale.x;
		data["scale"]["y"] = scale.y;
		data["scale"]["z"] = scale.z;
		return true;
	}

	bool Action_UnitMove::PostEncoding(json& data) const
	{
		data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_UnitMove::PreDecoding(const json& data)
	{
		lerpTime = data["lerpTime"];
		position.x = data["position"]["x"];
		position.y = data["position"]["y"];
		position.z = data["position"]["z"];
		rotation.x = data["rotation"]["x"];
		rotation.y = data["rotation"]["y"];
		rotation.z = data["rotation"]["z"];
		rotation.w = data["rotation"]["w"];
		scale.x = data["scale"]["x"];
		scale.y = data["scale"]["y"];
		scale.z = data["scale"]["z"];
		return true;
	}

	bool Action_UnitMove::PostDecoding(const json& data)
	{
		SetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
		destinationSetting = true;
		return true;
	}

	void Action_UnitMove::UpdateDestinationFromContainer()
	{
		UpdatePosition(container_pos);
		yunutyEngine::Quaternion quat = Vector3d(container_rot.x, container_rot.y, container_rot.z);
		yunuGI::Quaternion gi_quat;
		gi_quat.x = quat.x;
		gi_quat.y = quat.y;
		gi_quat.z = quat.z;
		gi_quat.w = quat.w;
		UpdateRotation(gi_quat);
		UpdateScale(container_scal);
		destinationSetting = true;
	}

	void Action_UnitMove::UpdatePosition(const yunuGI::Vector3& pos)
	{
		position.x = pos.x;
		position.y = pos.y;
		position.z = pos.z;
	}

	void Action_UnitMove::UpdateRotation(const yunuGI::Quaternion& rot)
	{
		rotation.x = rot.x;
		rotation.y = rot.y;
		rotation.z = rot.z;
		rotation.w = rot.w;
	}

	void Action_UnitMove::UpdateScale(const yunuGI::Vector3& scal)
	{
		scale.x = scal.x;
		scale.y = scal.y;
		scale.z = scal.z;
	}
}