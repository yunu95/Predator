#include "Action_Unit.h"

#include "InWanderLand.h"
#include "UnitData.h"
#include "UnitEditorInstance.h"
#include "Unit.h"

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

		Vector3d endPos = { destinationUnit->pod.position.x, destinationUnit->pod.position.y, destinationUnit->pod.position.z };

		endPos.y = 0;

		targetUnit->inGameUnit->OrderMove(endPos);
		while(true)
		{
			auto curPos = ts->GetWorldPosition();
			if ((endPos - curPos).MagnitudeSqr() <= 0.04)
			{
				break;
			}
			co_await std::suspend_always();
		}

		co_return;
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

	Action_UnitRotate::~Action_UnitRotate()
	{
		if (targetUnit)
		{
			targetUnit->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_UnitRotate::DoAction()
	{
		auto ts = targetUnit->inGameUnit->GetTransform();
		auto startRot = ts->GetWorldRotation();

		Quaternion endRot;
		
		if (isRelative)
		{
			endRot = startRot * Quaternion(Vector3d(0, angle, 0));
		}
		else
		{
			endRot = Quaternion(Vector3d(0, angle, 0));
		}

		double timer = 0;
		float factor = 0;

		if (lerpTime == 0)
		{
			ts->SetWorldRotation(endRot);
			co_return;
		}

		for (double timer = 0; timer < lerpTime;)
		{
			factor = timer / lerpTime;
			ts->SetWorldRotation(Quaternion::Lerp(startRot, endRot, factor));
			timer += Time::GetDeltaTimeUnscaled();
			co_await std::suspend_always();
		}
	}

	bool Action_UnitRotate::IsValid()
	{
		return (targetUnit == nullptr) ? false : true;
	}

	void Action_UnitRotate::SetTargetUnit(editor::UnitData* unit)
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
	}

	void Action_UnitRotate::SetRelative(bool isRelative)
	{
		this->isRelative = isRelative;
	}

	void Action_UnitRotate::SetRotation(float angle)
	{
		this->angle = angle;
	}

	void Action_UnitRotate::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
	}

	void Action_UnitRotate::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

	void Action_UnitRotate::ImGui_DrawDataPopup(Action_UnitRotate* data)
	{
		if (ImGui::MenuItem("SetTargetUnit(Rotate)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetUnit(Rotate)", [data]()
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
						editor::imgui::CloseMessageBox("SetTargetUnit(Rotate)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitRotate>("SetTargetUnit(Rotate)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(Rotate)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetRelative"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool relative = false;
			relative = data->isRelative;
			editor::imgui::ShowMessageBox("SetRelative(UnitRotate)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("Relative", &relative);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetRelative(relative);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRelative(UnitRotate)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRelative(UnitRotate)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetAngle"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float angle = 0;
			angle = data->angle;
			editor::imgui::ShowMessageBox("SetAngle(UnitRotate)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##AngleUnitRotate", &angle);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetRotation(angle);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetAngle(UnitRotate)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetAngle(UnitRotate)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetLerpTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float lerpTime = 0;
			lerpTime = data->lerpTime;
			editor::imgui::ShowMessageBox("SetLerpTime(UnitRotate)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTimeUnitRotate", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(UnitRotate)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(UnitRotate)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_UnitRotate::PreEncoding(json& data) const
	{
		data["relative"] = isRelative;
		data["angle"] = angle;
		data["lerpTime"] = lerpTime;
		return true;
	}

	bool Action_UnitRotate::PostEncoding(json& data) const
	{
		data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_UnitRotate::PreDecoding(const json& data)
	{
		isRelative = data["relative"];
		angle = data["angle"];
		lerpTime = data["lerpTime"];
		return true;
	}

	bool Action_UnitRotate::PostDecoding(const json& data)
	{
		SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
		return true;
	}

	Action_UnitRescale::~Action_UnitRescale()
	{
		if (targetUnit)
		{
			targetUnit->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_UnitRescale::DoAction()
	{
		auto ts = targetUnit->inGameUnit->GetTransform();
		auto startScale = ts->GetWorldScale();

		Vector3d endScale = { scale.x, scale.y, scale.z };

		double timer = 0;
		float factor = 0;

		if (lerpTime == 0)
		{
			ts->SetWorldScale(endScale);
			co_return;
		}

		for (double timer = 0; timer < lerpTime;)
		{
			factor = timer / lerpTime;
			ts->SetWorldScale(Vector3d::Lerp(startScale, endScale, factor));
			timer += Time::GetDeltaTimeUnscaled();
			co_await std::suspend_always();
		}
	}

	bool Action_UnitRescale::IsValid()
	{
		return (targetUnit == nullptr) ? false : true;
	}

	void Action_UnitRescale::SetTargetUnit(editor::UnitData* unit)
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
	}

	void Action_UnitRescale::SetFinalScale(const yunuGI::Vector3& scale)
	{
		this->scale = scale;
	}

	void Action_UnitRescale::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
	}

	void Action_UnitRescale::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

	void Action_UnitRescale::ImGui_DrawDataPopup(Action_UnitRescale* data)
	{
		if (ImGui::MenuItem("SetTargetUnit(Rescale)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetUnit(Rescale)", [data]()
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
						editor::imgui::CloseMessageBox("SetTargetUnit(Rescale)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitRescale>("SetTargetUnit(Rescale)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(Rescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetFinalScale"))
		{
			editor::EditorLayer::SetInputControl(false);
			static yunuGI::Vector3 fScale = yunuGI::Vector3();
			fScale = data->scale;
			editor::imgui::ShowMessageBox("SetRelative(UnitRescale)", [data]()
				{
					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);

					auto resetScale = editor::imgui::Vector3Control("Scale", fScale.x, fScale.y, fScale.z);

					switch (resetScale)
					{
						case application::editor::imgui::Vector3Flags::ResetX:
							fScale.x = 1;
							break;
						case application::editor::imgui::Vector3Flags::ResetY:
							fScale.y = 1;
							break;
						case application::editor::imgui::Vector3Flags::ResetZ:
							fScale.z = 1;
							break;
						default:
							break;
					}

					if (fScale.x == 0)
					{
						fScale.x = 0.000001;
					}
					if (fScale.y == 0)
					{
						fScale.y = 0.000001;
					}
					if (fScale.z == 0)
					{
						fScale.z = 0.000001;
					}

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetFinalScale(fScale);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRelative(UnitRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRelative(UnitRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetLerpTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float lerpTime = 0;
			lerpTime = data->lerpTime;
			editor::imgui::ShowMessageBox("SetLerpTime(UnitRescale)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTimeUnitRescale", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(UnitRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(UnitRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_UnitRescale::PreEncoding(json& data) const
	{
		data["scale"]["x"] = scale.x;
		data["scale"]["y"] = scale.y;
		data["scale"]["z"] = scale.z;
		data["lerpTime"] = lerpTime;
		return true;
	}

	bool Action_UnitRescale::PostEncoding(json& data) const
	{
		data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_UnitRescale::PreDecoding(const json& data)
	{
		scale.x = data["scale"]["x"];
		scale.y = data["scale"]["y"];
		scale.z = data["scale"]["z"];
		lerpTime = data["lerpTime"];
		return true;
	}

	bool Action_UnitRescale::PostDecoding(const json& data)
	{
		SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
		return true;
	}

	Action_UnitMoveWithRotateAndRescale::~Action_UnitMoveWithRotateAndRescale()
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

	CoroutineObject<void> Action_UnitMoveWithRotateAndRescale::DoAction()
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
		while (true)
		{
			auto curPos = ts->GetWorldPosition();
			if ((endPos - curPos).MagnitudeSqr() <= 0.04)
			{
				break;
			}
			co_await std::suspend_always();
		}

		double timer = 0;
		float factor = 0;

		if (lerpTime == 0)
		{
			ts->SetWorldRotation(endRot);
			ts->SetWorldScale(endScale);
			co_return;
		}

		for (double timer = 0; timer < lerpTime;)
		{
			factor = timer / lerpTime;
			ts->SetWorldRotation(Quaternion::Lerp(startRot, endRot, factor));
			ts->SetWorldScale(Vector3d::Lerp(startScale, endScale, factor));
			timer += Time::GetDeltaTimeUnscaled();
			co_await std::suspend_always();
		}

		co_return;
	}

	bool Action_UnitMoveWithRotateAndRescale::IsValid()
	{
		return (targetUnit == nullptr) ? false : (destinationUnit == nullptr) ? false : true;
	}

	void Action_UnitMoveWithRotateAndRescale::SetTargetUnit(editor::UnitData* unit)
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

	void Action_UnitMoveWithRotateAndRescale::SetDestinationUnit(editor::UnitData* unit)
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

	void Action_UnitMoveWithRotateAndRescale::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
	}

	void Action_UnitMoveWithRotateAndRescale::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

	void Action_UnitMoveWithRotateAndRescale::ImGui_DrawDataPopup(Action_UnitMoveWithRotateAndRescale* data)
	{
		if (ImGui::MenuItem("SetTargetUnit(MoveWithRotateAndRescale)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetUnit(MoveWithRotateAndRescale)", [data]()
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
						editor::imgui::CloseMessageBox("SetTargetUnit(MoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitMoveWithRotateAndRescale>("SetTargetUnit(MoveWithRotateAndRescale)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(MoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (data->destinationUnit)
		{
			if (ImGui::MenuItem("EditDestinationUnit(MoveWithRotateAndRescale)"))
			{
				editor::EditorLayer::SetInputControl(false);
				editor::imgui::ShowMessageBox("EditDestinationUnit(MoveWithRotateAndRescale)", [data]()
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
							editor::imgui::CloseMessageBox("EditDestinationUnit(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
							editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitMoveWithRotateAndRescale>("EditDestinationUnit(MoveWithRotateAndRescale)", data);
						}
						ImGui::SameLine();

						if (ImGui::Button("Cancel"))
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("EditDestinationUnit(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
						}
					}, 300);
			}
		}
		else
		{
			if (data->targetUnit && ImGui::MenuItem("SetDestinationUnit(MoveWithRotateAndRescale)"))
			{
				editor::EditorLayer::SetInputControl(false);
				editor::imgui::ShowMessageBox("SetDestinationUnit(MoveWithRotateAndRescale)", [data]()
					{
						if (data->destinationUnit)
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("SetDestinationUnit(MoveWithRotateAndRescale)");
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
							editor::imgui::CloseMessageBox("SetDestinationUnit(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
							editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitMoveWithRotateAndRescale>("SetDestinationUnit(MoveWithRotateAndRescale)", data);
						}
						ImGui::SameLine();

						if (ImGui::Button("Cancel"))
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("SetDestinationUnit(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
						}
					}, 300);
			}
		}

		if (ImGui::MenuItem("SetLerpTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float lerpTime = 0;
			lerpTime = data->lerpTime;
			editor::imgui::ShowMessageBox("SetLerpTime(UnitMoveWithRotateAndRescale)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTimeUnitMoveWithRotateAndRescale", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(UnitMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(UnitMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_UnitMoveWithRotateAndRescale::PreEncoding(json& data) const
	{
		data["lerpTime"] = lerpTime;
		return true;
	}

	bool Action_UnitMoveWithRotateAndRescale::PostEncoding(json& data) const
	{
		data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
		data["destinationUnit"] = destinationUnit ? UUID_To_String(destinationUnit->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_UnitMoveWithRotateAndRescale::PreDecoding(const json& data)
	{
		lerpTime = data["lerpTime"];
		return true;
	}

	bool Action_UnitMoveWithRotateAndRescale::PostDecoding(const json& data)
	{
		SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
		SetDestinationUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["destinationUnit"])));
		return true;
	}
}