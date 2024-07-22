#include "Action_Particle.h"

#include "InWanderLand.h"
#include "ParticleData.h"
#include "ParticleEditorInstance.h"

#include "Application.h"
#include "EditorLayer.h"

#include "EditorPopupManager.h"
#include "InstanceManager.h"
#include "Panel_Palette.h"
#include "ParticlePalette.h"

#include "YunutyEngine.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace application
{
	Action_ParticleMoveWithRotateAndRescale::~Action_ParticleMoveWithRotateAndRescale()
	{
		if (targetParticle)
		{
			targetParticle->RemoveObserver(this);
		}

		if (destinationParticle)
		{
			destinationParticle->RemoveObserver(this);
			editor::InstanceManager::GetSingletonInstance().DeleteInstance(destinationParticle->GetUUID());
		}
	}

	CoroutineObject<void> Action_ParticleMoveWithRotateAndRescale::DoAction()
	{
		targetParticle->tookAction = true;
		auto ts = static_cast<editor::palette::ParticleEditorInstance*>(targetParticle->GetPaletteInstance())->GetParticleObject()->GetTransform();

		Vector3d endPos = { destinationParticle->pod.position.x, destinationParticle->pod.position.y, destinationParticle->pod.position.z };
		Quaternion endRot = { destinationParticle->pod.rotation.w, destinationParticle->pod.rotation.x, destinationParticle->pod.rotation.y, destinationParticle->pod.rotation.z };
		Vector3d endScale = { destinationParticle->pod.scale.x, destinationParticle->pod.scale.y, destinationParticle->pod.scale.z };

		endPos.y = 0;

		double timer = 0;
		float factor = 0;

		auto startPos = ts->GetWorldPosition();
		auto startRot = ts->GetWorldRotation();
		auto startScale = ts->GetWorldScale();

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

		co_return;
	}

	bool Action_ParticleMoveWithRotateAndRescale::IsValid()
	{
		return (targetParticle == nullptr) ? false : (destinationParticle == nullptr) ? false : true;
	}

	void Action_ParticleMoveWithRotateAndRescale::SetTargetParticle(editor::ParticleData* particle)
	{
		if (targetParticle)
		{
			targetParticle->RemoveObserver(this);
		}

		targetParticle = particle;
		if (particle)
		{
			particle->RegisterObserver(this);
		}

		if (destinationParticle)
		{
			editor::InstanceManager::GetSingletonInstance().DeleteInstance(destinationParticle->GetUUID());
		}
	}

	void Action_ParticleMoveWithRotateAndRescale::SetDestinationParticle(editor::ParticleData* particle)
	{
		if (destinationParticle)
		{
			destinationParticle->RemoveObserver(this);
		}

		destinationParticle = particle;
		if (particle)
		{
			particle->RegisterObserver(this);
			if (destinationParticle->GetPaletteInstance())
			{
				particle->pod.isGuide = true;
				static_cast<editor::palette::ParticleEditorInstance*>(destinationParticle->GetPaletteInstance())->ChangeGuideInstance();
			}
		}
	}

	void Action_ParticleMoveWithRotateAndRescale::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
	}

	void Action_ParticleMoveWithRotateAndRescale::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetParticle == static_cast<editor::ParticleData*>(target))
				{
					targetParticle = nullptr;
				}
				if (destinationParticle == static_cast<editor::ParticleData*>(target))
				{
					destinationParticle = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Action_ParticleMoveWithRotateAndRescale::ImGui_DrawDataPopup(Action_ParticleMoveWithRotateAndRescale* data)
	{
		if (ImGui::MenuItem("SetTargetParticle(MoveWithRotateAndRescale)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetParticle(MoveWithRotateAndRescale)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& ptp = editor::palette::ParticlePalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetParticle)
					{
						ImGui::Text(data->targetParticle->pod.name.c_str());
						pp.ChangeTab("Particle");
						ptp.Reset();
						ptp.SelectParticle(data->targetParticle);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetParticle(MoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_ParticleMoveWithRotateAndRescale>("SetTargetParticle(MoveWithRotateAndRescale)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetParticle(MoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (data->destinationParticle)
		{
			if (ImGui::MenuItem("EditDestinationParticle(MoveWithRotateAndRescale)"))
			{
				editor::EditorLayer::SetInputControl(false);
				editor::imgui::ShowMessageBox("EditDestinationParticle(MoveWithRotateAndRescale)", [data]()
					{
						editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

						ImGui::Separator();

						ImGui::SetNextItemWidth(-1);

						ImGui::Text(data->destinationParticle->pod.name.c_str());

						ImGui::Separator();

						if (ImGui::Button("Edit"))
						{
							data->destinationParticle->GetPaletteInstance()->GetGameObject()->SetSelfActive(true);
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("EditDestinationParticle(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
							editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_ParticleMoveWithRotateAndRescale>("EditDestinationParticle(MoveWithRotateAndRescale)", data);
						}
						ImGui::SameLine();

						if (ImGui::Button("Cancel"))
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("EditDestinationParticle(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
						}
					}, 300);
			}
		}
		else
		{
			if (data->targetParticle && ImGui::MenuItem("SetDestinationParticle(MoveWithRotateAndRescale)"))
			{
				editor::EditorLayer::SetInputControl(false);
				editor::imgui::ShowMessageBox("SetDestinationParticle(MoveWithRotateAndRescale)", [data]()
					{
						if (data->destinationParticle)
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("SetDestinationParticle(MoveWithRotateAndRescale)");
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
							editor::imgui::CloseMessageBox("SetDestinationParticle(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
							editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_ParticleMoveWithRotateAndRescale>("SetDestinationParticle(MoveWithRotateAndRescale)", data);
						}
						ImGui::SameLine();

						if (ImGui::Button("Cancel"))
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("SetDestinationParticle(MoveWithRotateAndRescale)");
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
			editor::imgui::ShowMessageBox("SetLerpTime(ParticleMoveWithRotateAndRescale)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTimeParticleMoveWithRotateAndRescale", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(ParticleMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(ParticleMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_ParticleMoveWithRotateAndRescale::PreEncoding(json& data) const
	{
		data["lerpTime"] = lerpTime;
		return true;
	}

	bool Action_ParticleMoveWithRotateAndRescale::PostEncoding(json& data) const
	{
		data["targetParticle"] = targetParticle ? UUID_To_String(targetParticle->GetUUID()) : "nullptr";
		data["destinationParticle"] = destinationParticle ? UUID_To_String(destinationParticle->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_ParticleMoveWithRotateAndRescale::PreDecoding(const json& data)
	{
		lerpTime = data["lerpTime"];
		return true;
	}

	bool Action_ParticleMoveWithRotateAndRescale::PostDecoding(const json& data)
	{
		SetTargetParticle(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::ParticleData*>(String_To_UUID(data["targetParticle"])));
		SetDestinationParticle(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::ParticleData*>(String_To_UUID(data["destinationParticle"])));
		return true;
	}

	void Action_ParticleMoveWithRotateAndRescale::PostRecovery()
	{
		if (!doAction && targetParticle->tookAction)
		{
			auto particleInstance = targetParticle->GetPaletteInstance();
			particleInstance->GetTransform()->SetWorldPosition({ targetParticle->pod.position.x,targetParticle->pod.position.y,targetParticle->pod.position.z });
			particleInstance->GetTransform()->SetWorldRotation({ targetParticle->pod.rotation.w, targetParticle->pod.rotation.x, targetParticle->pod.rotation.y, targetParticle->pod.rotation.z });
			particleInstance->GetTransform()->SetLocalScale({ targetParticle->pod.scale.x,targetParticle->pod.scale.y,targetParticle->pod.scale.z });
			targetParticle->tookAction = false;
		}
	}

	Action_ParticleShow::~Action_ParticleShow()
	{
		if (targetParticle)
		{
			targetParticle->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_ParticleShow::DoAction()
	{
		static_cast<editor::palette::ParticleEditorInstance*>(targetParticle->GetPaletteInstance())->ShowParticleObject();
		targetParticle->tookAction = true;
		co_return;
	}

	bool Action_ParticleShow::IsValid()
	{
		return (targetParticle == nullptr) ? false : true;
	}

	void Action_ParticleShow::SetTargetParticle(editor::ParticleData* particle)
	{
		if (targetParticle)
		{
			targetParticle->RemoveObserver(this);
		}

		targetParticle = particle;
		if (particle)
		{
			particle->RegisterObserver(this);
		}
	}

	void Action_ParticleShow::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetParticle == static_cast<editor::ParticleData*>(target))
				{
					targetParticle = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Action_ParticleShow::ImGui_DrawDataPopup(Action_ParticleShow* data)
	{
		if (ImGui::MenuItem("SetTargetParticle(Show)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetParticle(Show)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& ptp = editor::palette::ParticlePalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetParticle)
					{
						ImGui::Text(data->targetParticle->pod.name.c_str());
						pp.ChangeTab("Particle");
						ptp.Reset();
						ptp.SelectParticle(data->targetParticle);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetParticle(Show)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_ParticleShow>("SetTargetParticle(Show)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetParticle(Show)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_ParticleShow::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_ParticleShow::PostEncoding(json& data) const
	{
		data["targetParticle"] = targetParticle ? UUID_To_String(targetParticle->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_ParticleShow::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_ParticleShow::PostDecoding(const json& data)
	{
		SetTargetParticle(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::ParticleData*>(String_To_UUID(data["targetParticle"])));
		return true;
	}

	void Action_ParticleShow::ProgressInitialize()
	{
		IAction::ProgressInitialize();
		savedActive = true;
	}

	void Action_ParticleShow::CurrentProgressSave()
	{
		IAction::CurrentProgressSave();
		savedActive = static_cast<editor::palette::ParticleEditorInstance*>(targetParticle->GetPaletteInstance())->GetParticleObject()->GetSelfActive();
	}

	void Action_ParticleShow::Recovery()
	{
		IAction::Recovery();
		if (!doAction && !savedActive)
		{
			static_cast<editor::palette::ParticleEditorInstance*>(targetParticle->GetPaletteInstance())->HideParticleObject();
			targetParticle->tookAction = false;
		}
	}

	Action_ParticleHide::~Action_ParticleHide()
	{
		if (targetParticle)
		{
			targetParticle->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_ParticleHide::DoAction()
	{
		static_cast<editor::palette::ParticleEditorInstance*>(targetParticle->GetPaletteInstance())->HideParticleObject();
		targetParticle->tookAction = true;
		co_return;
	}

	bool Action_ParticleHide::IsValid()
	{
		return (targetParticle == nullptr) ? false : true;
	}

	void Action_ParticleHide::SetTargetParticle(editor::ParticleData* particle)
	{
		if (targetParticle)
		{
			targetParticle->RemoveObserver(this);
		}

		targetParticle = particle;
		if (particle)
		{
			particle->RegisterObserver(this);
		}
	}
	
	void Action_ParticleHide::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetParticle == static_cast<editor::ParticleData*>(target))
				{
					targetParticle = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Action_ParticleHide::ImGui_DrawDataPopup(Action_ParticleHide* data)
	{
		if (ImGui::MenuItem("SetTargetParticle(Hide)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetParticle(Hide)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& ptp = editor::palette::ParticlePalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetParticle)
					{
						ImGui::Text(data->targetParticle->pod.name.c_str());
						pp.ChangeTab("Particle");
						ptp.Reset();
						ptp.SelectParticle(data->targetParticle);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetParticle(Hide)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_ParticleHide>("SetTargetParticle(Hide)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetParticle(Hide)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_ParticleHide::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_ParticleHide::PostEncoding(json& data) const
	{
		data["targetParticle"] = targetParticle ? UUID_To_String(targetParticle->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_ParticleHide::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_ParticleHide::PostDecoding(const json& data)
	{
		SetTargetParticle(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::ParticleData*>(String_To_UUID(data["targetParticle"])));
		return true;
	}

	void Action_ParticleHide::ProgressInitialize()
	{
		IAction::ProgressInitialize();
		savedActive = false;
	}

	void Action_ParticleHide::CurrentProgressSave()
	{
		IAction::CurrentProgressSave();
		savedActive = static_cast<editor::palette::ParticleEditorInstance*>(targetParticle->GetPaletteInstance())->GetParticleObject()->GetSelfActive();
	}

	void Action_ParticleHide::Recovery()
	{
		IAction::Recovery();
		if (!doAction && savedActive)
		{
			static_cast<editor::palette::ParticleEditorInstance*>(targetParticle->GetPaletteInstance())->ShowParticleObject();
			targetParticle->tookAction = false;
		}
	}
}