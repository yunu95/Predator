#include "Action_Unit.h"

#include "InWanderLand.h"
#include "UnitData.h"
#include "UnitEditorInstance.h"
#include "Unit.h"

#include "Application.h"
#include "EditorLayer.h"
#include "Panel_Palette.h"
#include "UnitPalette.h"

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
		if (targetUnit->inGameUnit.expired())
		{
			co_return;
		}
		auto ingameUnit = targetUnit->inGameUnit.lock();
		if (withdrawPauseRequestBefore)
			ingameUnit->pauseRequested = false;
		if (addUnpauseRequestBefore)
			ingameUnit->unpauseRequested = true;

		auto ts = targetUnit->inGameUnit.lock()->GetTransform();
		auto startPos = ts->GetWorldPosition();

		Vector3d endPos = { destinationUnit->pod.position.x, destinationUnit->pod.position.y, destinationUnit->pod.position.z };

		endPos.y = 0;

		bool moving = true;
		targetUnit->inGameUnit.lock()->OrderMove(endPos);
		targetUnit->inGameUnit.lock()->OnStateExitCallback()[UnitBehaviourTree::Move].AddVolatileCallback([&]() {moving = false; });
		while (moving) { co_await std::suspend_always(); }
		if (addPauseRequestAfter)
			ingameUnit->pauseRequested = true;
		if (withdrawUnpauseRequestAfter)
			ingameUnit->unpauseRequested = false;
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
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& up = editor::palette::UnitPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetUnit)
					{
						ImGui::Text(data->targetUnit->pod.templateData->pod.skinnedFBXName.c_str());
						pp.ChangeTab("Unit");
						up.Reset();
						up.SelectUnitInstance(data->targetUnit);
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
		if (ImGui::MenuItem("SetPauseSettings"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool addUnpauseRequestBefore{ false };
			static bool withdrawPauseRequestBefore{ false };
			static bool withdrawUnpauseRequestAfter{ false };
			static bool addPauseRequestAfter{ false };
			addUnpauseRequestBefore = data->addUnpauseRequestBefore;
			withdrawPauseRequestBefore = data->withdrawPauseRequestBefore;
			withdrawUnpauseRequestAfter = data->withdrawUnpauseRequestAfter;
			addPauseRequestAfter = data->addPauseRequestAfter;
			editor::imgui::ShowMessageBox("SetPauseSettings(UnitMoveWithRotateAndRescale)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("addUnpauseRequestBefore##addUnpauseRequestBefore_UnitMoveWithRotateAndRescale", &addUnpauseRequestBefore);
					ImGui::Checkbox("withdrawPauseRequestBefore##withdrawPauseRequestBefore_UnitMoveWithRotateAndRescale", &withdrawPauseRequestBefore);
					ImGui::Checkbox("withdrawUnpauseRequestAfter##withdrawUnpauseRequestAfter_UnitMoveWithRotateAndRescale", &withdrawUnpauseRequestAfter);
					ImGui::Checkbox("addPauseRequestAfter##addPauseRequestAfter_UnitMoveWithRotateAndRescale", &addPauseRequestAfter);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->addUnpauseRequestBefore = addUnpauseRequestBefore;
						data->withdrawPauseRequestBefore = withdrawPauseRequestBefore;
						data->withdrawUnpauseRequestAfter = withdrawUnpauseRequestAfter;
						data->addPauseRequestAfter = addPauseRequestAfter;
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetPauseSettings(UnitMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetPauseSettings(UnitMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_UnitMove::PreEncoding(json& data) const
	{
		data["addUnpauseRequestBefore"] = addUnpauseRequestBefore;
		data["withdrawPauseRequestBefore"] = withdrawPauseRequestBefore;
		data["withdrawUnpauseRequestAfter"] = withdrawUnpauseRequestAfter;
		data["addPauseRequestAfter"] = addPauseRequestAfter;
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
		if (data.contains("addUnpauseRequestBefore"))
			addUnpauseRequestBefore = data["addUnpauseRequestBefore"];
		if (data.contains("withdrawPauseRequestBefore"))
			withdrawPauseRequestBefore = data["withdrawPauseRequestBefore"];
		if (data.contains("withdrawUnpauseRequestAfter"))
			withdrawUnpauseRequestAfter = data["withdrawUnpauseRequestAfter"];
		if (data.contains("addPauseRequestAfter"))
			addPauseRequestAfter = data["addPauseRequestAfter"];
	}

	bool Action_UnitMove::PostDecoding(const json& data)
	{
		SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
		SetDestinationUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["destinationUnit"])));
		return true;
	}

	void Action_UnitMove::PostRecovery()
	{
		if (targetUnit->inGameUnit.expired())
		{
			UnitPool::SingleInstance().Borrow(targetUnit);
		}
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
		if (targetUnit->inGameUnit.expired())
		{
			co_return;
		}

		auto ingameUnit = targetUnit->inGameUnit.lock();
		if (withdrawPauseRequestBefore)
			ingameUnit->pauseRequested = false;
		if (addUnpauseRequestBefore)
			ingameUnit->unpauseRequested = true;
		// 유닛을 정지시키고
		//auto pause{ targetUnit->inGameUnit.lock()->referencePause.Acquire() };
		ingameUnit->OrderHold();
		// 유닛의 자체적인 회전을 막는다.
		//auto blockRotation{ ingameUnit->referenceBlockRotation.Acquire() };
		//auto ts = ingameUnit->GetTransform();
		//auto startRot = ts->GetWorldRotation();

		Quaternion endRot;

		if (isRelative)
		{
			//endRot = startRot * Quaternion(Vector3d(0, 90 - angle, 0));
		}
		else
		{
			//endRot = Quaternion(Vector3d(0, 90 - angle, 0));
		}

		double timer = 0;
		ingameUnit->SetRotation(isRelative ? ingameUnit->GetDesiredRotation() + angle : angle, lerpTime);
		//float factor = 0;

		//if (lerpTime == 0)
		//{
		//    ts->SetWorldRotation(endRot);
		//    co_return;
		//}

		for (double timer = 0; timer < lerpTime;)
		{
			//factor = timer / lerpTime;
			//ts->SetWorldRotation(Quaternion::Lerp(startRot, endRot, factor));
			timer += Time::GetDeltaTimeUnscaled();
			co_await std::suspend_always();
		}
		if (addPauseRequestAfter)
			ingameUnit->pauseRequested = true;
		if (withdrawUnpauseRequestAfter)
			ingameUnit->unpauseRequested = false;
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
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& up = editor::palette::UnitPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetUnit)
					{
						ImGui::Text(data->targetUnit->pod.templateData->pod.skinnedFBXName.c_str());
						pp.ChangeTab("Unit");
						up.Reset();
						up.SelectUnitInstance(data->targetUnit);
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
		if (ImGui::MenuItem("SetPauseSettings"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool addUnpauseRequestBefore{ false };
			static bool withdrawPauseRequestBefore{ false };
			static bool withdrawUnpauseRequestAfter{ false };
			static bool addPauseRequestAfter{ false };
			addUnpauseRequestBefore = data->addUnpauseRequestBefore;
			withdrawPauseRequestBefore = data->withdrawPauseRequestBefore;
			withdrawUnpauseRequestAfter = data->withdrawUnpauseRequestAfter;
			addPauseRequestAfter = data->addPauseRequestAfter;
			editor::imgui::ShowMessageBox("SetPauseSettings(UnitMoveWithRotateAndRescale)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("addUnpauseRequestBefore##addUnpauseRequestBefore_UnitMoveWithRotateAndRescale", &addUnpauseRequestBefore);
					ImGui::Checkbox("withdrawPauseRequestBefore##withdrawPauseRequestBefore_UnitMoveWithRotateAndRescale", &withdrawPauseRequestBefore);
					ImGui::Checkbox("withdrawUnpauseRequestAfter##withdrawUnpauseRequestAfter_UnitMoveWithRotateAndRescale", &withdrawUnpauseRequestAfter);
					ImGui::Checkbox("addPauseRequestAfter##addPauseRequestAfter_UnitMoveWithRotateAndRescale", &addPauseRequestAfter);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->addUnpauseRequestBefore = addUnpauseRequestBefore;
						data->withdrawPauseRequestBefore = withdrawPauseRequestBefore;
						data->withdrawUnpauseRequestAfter = withdrawUnpauseRequestAfter;
						data->addPauseRequestAfter = addPauseRequestAfter;
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetPauseSettings(UnitMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetPauseSettings(UnitMoveWithRotateAndRescale)");
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
		data["addUnpauseRequestBefore"] = addUnpauseRequestBefore;
		data["withdrawPauseRequestBefore"] = withdrawPauseRequestBefore;
		data["withdrawUnpauseRequestAfter"] = withdrawUnpauseRequestAfter;
		data["addPauseRequestAfter"] = addPauseRequestAfter;
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
		if (data.contains("addUnpauseRequestBefore"))
			addUnpauseRequestBefore = data["addUnpauseRequestBefore"];
		if (data.contains("withdrawPauseRequestBefore"))
			withdrawPauseRequestBefore = data["withdrawPauseRequestBefore"];
		if (data.contains("withdrawUnpauseRequestAfter"))
			withdrawUnpauseRequestAfter = data["withdrawUnpauseRequestAfter"];
		if (data.contains("addPauseRequestAfter"))
			addPauseRequestAfter = data["addPauseRequestAfter"];
		return true;
	}

	bool Action_UnitRotate::PostDecoding(const json& data)
	{
		SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
		return true;
	}

	void Action_UnitRotate::PostRecovery()
	{
		if (targetUnit->inGameUnit.expired())
		{
			UnitPool::SingleInstance().Borrow(targetUnit);
		}
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
		if (targetUnit->inGameUnit.expired())
		{
			co_return;
		}
		auto ts = targetUnit->inGameUnit.lock()->GetTransform();
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
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& up = editor::palette::UnitPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetUnit)
					{
						ImGui::Text(data->targetUnit->pod.templateData->pod.skinnedFBXName.c_str());
						pp.ChangeTab("Unit");
						up.Reset();
						up.SelectUnitInstance(data->targetUnit);
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

	void Action_UnitRescale::PostRecovery()
	{
		if (targetUnit->inGameUnit.expired())
		{
			UnitPool::SingleInstance().Borrow(targetUnit);
		}
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
		if (targetUnit->inGameUnit.expired())
		{
			co_return;
		}

		auto inGameUnit = targetUnit->inGameUnit.lock();

		if (withdrawPauseRequestBefore)
			inGameUnit->pauseRequested = false;
		if (addUnpauseRequestBefore)
			inGameUnit->unpauseRequested = true;
		auto ts = inGameUnit->GetTransform();
		auto startPos = ts->GetWorldPosition();

		Vector3d endPos = { destinationUnit->pod.position.x, destinationUnit->pod.position.y, destinationUnit->pod.position.z };
		Quaternion endRot = { destinationUnit->pod.rotation.w, destinationUnit->pod.rotation.x, destinationUnit->pod.rotation.y, destinationUnit->pod.rotation.z };
		Vector3d endScale = { destinationUnit->pod.scale.x, destinationUnit->pod.scale.y, destinationUnit->pod.scale.z };

		endPos.y = 0;

		bool moving = true;
		inGameUnit->OrderMove(endPos);
		inGameUnit->OnStateExitCallback()[UnitBehaviourTree::Move].AddVolatileCallback([&]() {moving = false; });
		while (moving) { co_await std::suspend_always(); }

		{
			// 유닛을 정지시키고
			//auto pause{ inGameUnit->referencePause.Acquire() };
			inGameUnit->OrderHold();
			// 유닛의 자체적인 회전을 막는다.
			//auto blockRotation{ inGameUnit->referencePause.Acquire() };
			double timer = 0;
			float factor = 0;

			//auto startRot = ts->GetWorldRotation();
			auto startScale = ts->GetWorldScale();
			inGameUnit->SetRotation(endRot, lerpTime);

			if (lerpTime == 0)
			{
				//ts->SetWorldRotation(endRot);
				ts->SetWorldScale(endScale);
				if (addPauseRequestAfter)
					inGameUnit->pauseRequested = true;
				if (withdrawUnpauseRequestAfter)
					inGameUnit->unpauseRequested = false;
				co_return;
			}

			for (double timer = 0; timer < lerpTime;)
			{
				factor = timer / lerpTime;
				//ts->SetWorldRotation(Quaternion::Lerp(startRot, endRot, factor));
				ts->SetWorldScale(Vector3d::Lerp(startScale, endScale, factor));
				timer += Time::GetDeltaTimeUnscaled();
				co_await std::suspend_always();
			}

			if (addPauseRequestAfter)
				inGameUnit->pauseRequested = true;
			if (withdrawUnpauseRequestAfter)
				inGameUnit->unpauseRequested = false;
			co_return;
		}
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
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& up = editor::palette::UnitPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetUnit)
					{
						ImGui::Text(data->targetUnit->pod.templateData->pod.skinnedFBXName.c_str());
						pp.ChangeTab("Unit");
						up.Reset();
						up.SelectUnitInstance(data->targetUnit);
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
		if (ImGui::MenuItem("SetPauseSettings"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool addUnpauseRequestBefore{ false };
			static bool withdrawPauseRequestBefore{ false };
			static bool withdrawUnpauseRequestAfter{ false };
			static bool addPauseRequestAfter{ false };
			addUnpauseRequestBefore = data->addUnpauseRequestBefore;
			withdrawPauseRequestBefore = data->withdrawPauseRequestBefore;
			withdrawUnpauseRequestAfter = data->withdrawUnpauseRequestAfter;
			addPauseRequestAfter = data->addPauseRequestAfter;
			editor::imgui::ShowMessageBox("SetPauseSettings(UnitMoveWithRotateAndRescale)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("addUnpauseRequestBefore##addUnpauseRequestBefore_UnitMoveWithRotateAndRescale", &addUnpauseRequestBefore);
					ImGui::Checkbox("withdrawPauseRequestBefore##withdrawPauseRequestBefore_UnitMoveWithRotateAndRescale", &withdrawPauseRequestBefore);
					ImGui::Checkbox("withdrawUnpauseRequestAfter##withdrawUnpauseRequestAfter_UnitMoveWithRotateAndRescale", &withdrawUnpauseRequestAfter);
					ImGui::Checkbox("addPauseRequestAfter##addPauseRequestAfter_UnitMoveWithRotateAndRescale", &addPauseRequestAfter);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->addUnpauseRequestBefore = addUnpauseRequestBefore;
						data->withdrawPauseRequestBefore = withdrawPauseRequestBefore;
						data->withdrawUnpauseRequestAfter = withdrawUnpauseRequestAfter;
						data->addPauseRequestAfter = addPauseRequestAfter;
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetPauseSettings(UnitMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetPauseSettings(UnitMoveWithRotateAndRescale)");
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

	void Action_UnitMoveWithRotateAndRescale::PostRecovery()
	{
		if (targetUnit->inGameUnit.expired())
		{
			UnitPool::SingleInstance().Borrow(targetUnit);
		}
	}

	Action_UnitPlayAnimation::~Action_UnitPlayAnimation()
	{
		if (targetUnit)
		{
			targetUnit->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_UnitPlayAnimation::DoAction()
	{
		if (targetUnit->inGameUnit.expired() || animType == UnitAnimType::None)
		{
			co_return;
		}
		auto ingameUnit = targetUnit->inGameUnit.lock();
		auto puaseRef = ingameUnit->referencePause.Acquire();
		auto anim = wanderResources::GetAnimation(ingameUnit->GetFBXName(), animType);
		auto speedRatio = 1.0f;
		if (duration > 0)
		{
			speedRatio = anim->GetDuration() / duration;
		}

		co_await std::suspend_always{};

		auto animator = ingameUnit->GetAnimator().lock();
		animator->GetGI().SetPlaySpeed(speedRatio);

		if (repeat)
		{
			ingameUnit->PlayAnimation(animType, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
			while (animator->GetCurrentAnimation() == anim)
			{
				co_await std::suspend_always{};
			}
		}
		else
		{
			auto blockAnimLoop = ingameUnit->referenceBlockAnimLoop.Acquire();
			ingameUnit->SetDefaultAnimation(UnitAnimType::None);
			ingameUnit->PlayAnimation(animType, Animation::PlayFlag_::Blending);
			while (animator->GetCurrentAnimation() == anim && !animator->IsDone())
			{
				co_await std::suspend_always{};
			}
		}

		animator->GetGI().SetPlaySpeed(1);

		co_return;
	}

	bool Action_UnitPlayAnimation::IsValid()
	{
		return (targetUnit == nullptr) ? false : true;
	}

	void Action_UnitPlayAnimation::SetTargetUnit(editor::UnitData* unit)
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

	void Action_UnitPlayAnimation::SetAnimType(UnitAnimType animType)
	{
		this->animType = animType;
	}

	void Action_UnitPlayAnimation::SetDuration(float duration)
	{
		this->duration = duration;
	}

	void Action_UnitPlayAnimation::SetRepeat(bool repeat)
	{
		this->repeat = repeat;
	}

	void Action_UnitPlayAnimation::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

	void Action_UnitPlayAnimation::ImGui_DrawDataPopup(Action_UnitPlayAnimation* data)
	{
		if (ImGui::MenuItem("SetTargetUnit(PlayAnimation)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetUnit(PlayAnimation)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& up = editor::palette::UnitPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetUnit)
					{
						ImGui::Text(data->targetUnit->pod.templateData->pod.skinnedFBXName.c_str());
						pp.ChangeTab("Unit");
						up.Reset();
						up.SelectUnitInstance(data->targetUnit);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(PlayAnimation)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitPlayAnimation>("SetTargetUnit(PlayAnimation)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(PlayAnimation)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetAnimation(PlayAnimation)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static int animType = (int)UnitAnimType::None;
			animType = (int)data->animType;
			editor::imgui::ShowMessageBox("SetAnimation(PlayAnimation)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);

					static std::map<int, std::string> nameMap = std::map<int, std::string>();
					static std::vector<const char*> nameList = std::vector<const char*>();

					if (nameMap.empty())
					{
						nameMap[(int)UnitAnimType::None] = "None";
						nameList.push_back("None");
						nameMap[(int)UnitAnimType::Idle] = "Idle";
						nameList.push_back("Idle");
						nameMap[(int)UnitAnimType::BattleIdle] = "BattleIdle";
						nameList.push_back("BattleIdle");
						nameMap[(int)UnitAnimType::Move] = "Move";
						nameList.push_back("Move");
						nameMap[(int)UnitAnimType::BattleMove] = "BattleMove";
						nameList.push_back("BattleMove");
						nameMap[(int)UnitAnimType::Attack] = "Attack";
						nameList.push_back("Attack");
						nameMap[(int)UnitAnimType::Death] = "Death";
						nameList.push_back("Death");
						nameMap[(int)UnitAnimType::Paralysis] = "Paralysis";
						nameList.push_back("Paralysis");
						nameMap[(int)UnitAnimType::Slam] = "Slam";
						nameList.push_back("Slam");
						nameMap[(int)UnitAnimType::Taunt] = "Taunt";
						nameList.push_back("Taunt");
						nameMap[(int)UnitAnimType::Skill1] = "Skill1";
						nameList.push_back("Skill1");
						nameMap[(int)UnitAnimType::Skill2] = "Skill2";
						nameList.push_back("Skill2");
						nameMap[(int)UnitAnimType::Skill3] = "Skill3";
						nameList.push_back("Skill3");
						nameMap[(int)UnitAnimType::Skill4] = "Skill4";
						nameList.push_back("Skill4");
						nameMap[(int)UnitAnimType::Birth] = "Birth";
						nameList.push_back("Birth");
						nameMap[(int)UnitAnimType::Rush] = "Rush";
						nameList.push_back("Rush");
						nameMap[(int)UnitAnimType::Throw] = "Throw";
						nameList.push_back("Throw");
						nameMap[(int)UnitAnimType::Spin] = "Spin";
						nameList.push_back("Spin");
						nameMap[(int)UnitAnimType::BattleStart] = "BattleStart";
						nameList.push_back("BattleStart");
						nameMap[(int)UnitAnimType::BattleEnd] = "BattleEnd";
						nameList.push_back("BattleEnd");
						nameMap[(int)UnitAnimType::Damaged] = "Damaged";
						nameList.push_back("Damaged");
						nameMap[(int)UnitAnimType::Airborne] = "Airborne";
						nameList.push_back("Airborne");
					}

					if (ImGui::BeginCombo("##Anim Type Combo", nameList[animType]))
					{
						for (int i = 0; i < (int)UnitAnimType::End; i++)
						{
							const bool is_selected = (nameList[animType] == nameList[i]);
							if (ImGui::Selectable(nameList[i], is_selected))
							{
								animType = i;
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetAnimType((UnitAnimType)animType);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetAnimation(PlayAnimation)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetAnimation(PlayAnimation)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetDuration(PlayAnimation)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float duration = -1;
			duration = data->duration;
			editor::imgui::ShowMessageBox("SetDuration(PlayAnimation)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##Duration_PlayAnim", &duration);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetDuration(duration);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetDuration(PlayAnimation)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetDuration(PlayAnimation)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetRepeat"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool repeat = false;
			repeat = data->repeat;
			editor::imgui::ShowMessageBox("SetRepeat(UnitAnimation)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("Repeat", &repeat);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetRepeat(repeat);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRepeat(UnitAnimation)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRepeat(UnitAnimation)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_UnitPlayAnimation::PreEncoding(json& data) const
	{
		data["duration"] = duration;
		data["animType"] = (int)animType;
		data["repeat"] = repeat;
		return true;
	}

	bool Action_UnitPlayAnimation::PostEncoding(json& data) const
	{
		data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_UnitPlayAnimation::PreDecoding(const json& data)
	{
		duration = data["duration"];
		animType = (UnitAnimType)(int)data["animType"];
		repeat = data["repeat"];
		return true;
	}

	bool Action_UnitPlayAnimation::PostDecoding(const json& data)
	{
		SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
		return true;
	}

	void Action_UnitPlayAnimation::PostRecovery()
	{
		if (targetUnit->inGameUnit.expired())
		{
			UnitPool::SingleInstance().Borrow(targetUnit);
		}
	}

	Action_UnitSetHP::~Action_UnitSetHP()
	{
		if (targetUnit)
		{
			targetUnit->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_UnitSetHP::DoAction()
	{
		if (targetUnit->inGameUnit.expired())
		{
			co_return;
		}

		if (hp >= targetUnit->inGameUnit.lock()->GetUnitCurrentHp())
		{
			targetUnit->inGameUnit.lock()->SetCurrentHp(hp);
		}
		else
		{
			targetUnit->inGameUnit.lock()->Damaged(targetUnit->inGameUnit.lock()->GetUnitCurrentHp() - hp);
		}
	}

	bool Action_UnitSetHP::IsValid()
	{
		return (targetUnit == nullptr) ? false : true;
	}

	void Action_UnitSetHP::SetTargetUnit(editor::UnitData* unit)
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

	void Action_UnitSetHP::SetHP(float hp)
	{
		this->hp = hp;
	}

	void Action_UnitSetHP::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

	void Action_UnitSetHP::ImGui_DrawDataPopup(Action_UnitSetHP* data)
	{
		if (ImGui::MenuItem("SetTargetUnit(HP)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetUnit(HP)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& up = editor::palette::UnitPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetUnit)
					{
						ImGui::Text(data->targetUnit->pod.templateData->pod.skinnedFBXName.c_str());
						pp.ChangeTab("Unit");
						up.Reset();
						up.SelectUnitInstance(data->targetUnit);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(HP)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitSetHP>("SetTargetUnit(HP)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(HP)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetUnitHP"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float hp = 0;
			hp = data->hp;
			editor::imgui::ShowMessageBox("SetUnitHP", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##UnitHP", &hp);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetHP(hp);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetUnitHP");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetUnitHP");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_UnitSetHP::PreEncoding(json& data) const
	{
		data["hp"] = hp;
		return true;
	}

	bool Action_UnitSetHP::PostEncoding(json& data) const
	{
		data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_UnitSetHP::PreDecoding(const json& data)
	{
		hp = data["hp"];
		return true;
	}

	bool Action_UnitSetHP::PostDecoding(const json& data)
	{
		SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
		return true;
	}
}