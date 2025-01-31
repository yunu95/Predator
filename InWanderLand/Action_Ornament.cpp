#include "Action_Ornament.h"

#include "InWanderLand.h"
#include "OrnamentData.h"
#include "OrnamentEditorInstance.h"

#include "Application.h"
#include "EditorLayer.h"

#include "EditorPopupManager.h"
#include "InstanceManager.h"
#include "Panel_Palette.h"
#include "OrnamentPalette.h"

#include "YunutyEngine.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace application
{
	Action_OrnamentMoveWithRotateAndRescale::~Action_OrnamentMoveWithRotateAndRescale()
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}

		if (destinationOrnament)
		{
			destinationOrnament->RemoveObserver(this);
			editor::InstanceManager::GetSingletonInstance().DeleteInstance(destinationOrnament->GetUUID());
		}
	}

	CoroutineObject<void> Action_OrnamentMoveWithRotateAndRescale::DoAction()
	{
		auto ts = targetOrnament->GetPaletteInstance()->GetTransform();

		Vector3d endPos = { destinationOrnament->pod.position.x, destinationOrnament->pod.position.y, destinationOrnament->pod.position.z };
		Quaternion endRot = { destinationOrnament->pod.rotation.w, destinationOrnament->pod.rotation.x, destinationOrnament->pod.rotation.y, destinationOrnament->pod.rotation.z };
		Vector3d endScale = { destinationOrnament->pod.scale.x, destinationOrnament->pod.scale.y, destinationOrnament->pod.scale.z };

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

	bool Action_OrnamentMoveWithRotateAndRescale::IsValid()
	{
		return (targetOrnament == nullptr) ? false : (destinationOrnament == nullptr) ? false : true;
	}

	void Action_OrnamentMoveWithRotateAndRescale::SetTargetOrnament(editor::OrnamentData* ornament)
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}

		targetOrnament = ornament;
		if (ornament)
		{
			ornament->RegisterObserver(this);
		}

		if (destinationOrnament)
		{
			editor::InstanceManager::GetSingletonInstance().DeleteInstance(destinationOrnament->GetUUID());
		}
	}

	void Action_OrnamentMoveWithRotateAndRescale::SetDestinationOrnament(editor::OrnamentData* ornament)
	{
		if (destinationOrnament)
		{
			destinationOrnament->RemoveObserver(this);
		}

		destinationOrnament = ornament;
		if (ornament)
		{
			ornament->RegisterObserver(this);
			if (destinationOrnament->GetPaletteInstance())
			{
				ornament->pod.isGuide = true;
				static_cast<editor::palette::OrnamentEditorInstance*>(destinationOrnament->GetPaletteInstance())->ChangeGuideInstance();
			}
		}
	}

	void Action_OrnamentMoveWithRotateAndRescale::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
	}

	void Action_OrnamentMoveWithRotateAndRescale::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetOrnament == static_cast<editor::OrnamentData*>(target))
				{
					targetOrnament = nullptr;
				}
				if (destinationOrnament == static_cast<editor::OrnamentData*>(target))
				{
					destinationOrnament = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Action_OrnamentMoveWithRotateAndRescale::ImGui_DrawDataPopup(Action_OrnamentMoveWithRotateAndRescale* data)
	{
		if (ImGui::MenuItem("SetTargetOrnament(MoveWithRotateAndRescale)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetOrnament(MoveWithRotateAndRescale)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& op = editor::palette::OrnamentPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetOrnament)
					{
						ImGui::Text(data->targetOrnament->pod.templateData->pod.staticFBXName.c_str());
						pp.ChangeTab("Ornament");
						op.Reset();
						op.SelectOrnamentInstance(data->targetOrnament);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(MoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_OrnamentMoveWithRotateAndRescale>("SetTargetOrnament(MoveWithRotateAndRescale)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(MoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (data->destinationOrnament)
		{
			if (ImGui::MenuItem("EditDestinationOrnament(MoveWithRotateAndRescale)"))
			{
				editor::EditorLayer::SetInputControl(false);
				editor::imgui::ShowMessageBox("EditDestinationOrnament(MoveWithRotateAndRescale)", [data]()
					{
						editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

						ImGui::Separator();

						ImGui::SetNextItemWidth(-1);

						ImGui::Text(data->destinationOrnament->pod.templateData->pod.staticFBXName.c_str());

						ImGui::Separator();

						if (ImGui::Button("Edit"))
						{
							data->destinationOrnament->GetPaletteInstance()->GetGameObject()->SetSelfActive(true);
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("EditDestinationOrnament(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
							editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_OrnamentMoveWithRotateAndRescale>("EditDestinationOrnament(MoveWithRotateAndRescale)", data);
						}
						ImGui::SameLine();

						if (ImGui::Button("Cancel"))
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("EditDestinationOrnament(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
						}
					}, 300);
			}
		}
		else
		{
			if (data->targetOrnament && ImGui::MenuItem("SetDestinationOrnament(MoveWithRotateAndRescale)"))
			{
				editor::EditorLayer::SetInputControl(false);
				editor::imgui::ShowMessageBox("SetDestinationOrnament(MoveWithRotateAndRescale)", [data]()
					{
						if (data->destinationOrnament)
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("SetDestinationOrnament(MoveWithRotateAndRescale)");
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
							editor::imgui::CloseMessageBox("SetDestinationOrnament(MoveWithRotateAndRescale)");
							editor::EditorLayer::SetInputControl(true);
							editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_OrnamentMoveWithRotateAndRescale>("SetDestinationOrnament(MoveWithRotateAndRescale)", data);
						}
						ImGui::SameLine();

						if (ImGui::Button("Cancel"))
						{
							ImGui::CloseCurrentPopup();
							editor::imgui::CloseMessageBox("SetDestinationOrnament(MoveWithRotateAndRescale)");
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
			editor::imgui::ShowMessageBox("SetLerpTime(OrnamentMoveWithRotateAndRescale)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTimeOrnamentMoveWithRotateAndRescale", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(OrnamentMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(OrnamentMoveWithRotateAndRescale)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_OrnamentMoveWithRotateAndRescale::PreEncoding(json& data) const
	{
		data["lerpTime"] = lerpTime;
		return true;
	}

	bool Action_OrnamentMoveWithRotateAndRescale::PostEncoding(json& data) const
	{
		data["targetOrnament"] = targetOrnament ? UUID_To_String(targetOrnament->GetUUID()) : "nullptr";
		data["destinationOrnament"] = destinationOrnament ? UUID_To_String(destinationOrnament->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_OrnamentMoveWithRotateAndRescale::PreDecoding(const json& data)
	{
		lerpTime = data["lerpTime"];
		return true;
	}

	bool Action_OrnamentMoveWithRotateAndRescale::PostDecoding(const json& data)
	{
		SetTargetOrnament(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::OrnamentData*>(String_To_UUID(data["targetOrnament"])));
		SetDestinationOrnament(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::OrnamentData*>(String_To_UUID(data["destinationOrnament"])));
		return true;
	}

	void Action_OrnamentMoveWithRotateAndRescale::Recovery()
	{
		IAction::Recovery();
		if (!doAction)
		{
			auto ornamentInstance = targetOrnament->GetPaletteInstance();
			ornamentInstance->GetTransform()->SetWorldPosition({ targetOrnament->pod.position.x,targetOrnament->pod.position.y,targetOrnament->pod.position.z });
			ornamentInstance->GetTransform()->SetWorldRotation({ targetOrnament->pod.rotation.w, targetOrnament->pod.rotation.x, targetOrnament->pod.rotation.y, targetOrnament->pod.rotation.z });
			ornamentInstance->GetTransform()->SetLocalScale({ targetOrnament->pod.scale.x,targetOrnament->pod.scale.y,targetOrnament->pod.scale.z });
		}
	}

	Action_OrnamentShow::~Action_OrnamentShow()
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_OrnamentShow::DoAction()
	{
		/// 성찬아 여기봐
		targetOrnament->tookAction = true;

		targetOrnament->GetPaletteInstance()->GetGameObject()->SetSelfActive(true);

		if (lerpTime == 0)
		{
			co_return;
		}

		GameObject* targetObj = nullptr;

		for (auto each : targetOrnament->GetPaletteInstance()->GetGameObject()->GetChildren())
		{
			if (each->getName() != targetOrnament->pod.templateData->pod.staticFBXName)
			{
				continue;
			}

			targetObj = each;
		}

		yunutyEngine::graphics::StaticMeshRenderer* renderer = nullptr;
		for (auto each : targetObj->GetChildren())
		{
			renderer = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			if (renderer)
			{
				break;
			}
		}

		if (renderer)
		{
			float localTimer = 0;
			float ratio = 0;
			bool isFirst = false;
			bool isDeferred = (renderer->GetGI().GetMaterial()->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred);

			while (ratio < 1)
			{
				localTimer += yunutyEngine::Time::GetDeltaTime();
				ratio = localTimer / lerpTime;

				if (ratio > 1)
				{
					ratio = 1;
				}

				for (int i = 0; i < renderer->GetGI().GetMaterialCount(); ++i)
				{
					/// 추후 셰이더 교체까지 해줘야 함
					auto shaderName = renderer->GetGI().GetMaterial(i)->GetPixelShader()->GetName();

					size_t pos = shaderName.find(L"PS");
					if (pos != std::wstring::npos && !isFirst && isDeferred)
					{
						isFirst = true;
						const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
						shaderName = shaderName.substr(0, pos);
						shaderName += L"_AlphaPS.cso";
						renderer->GetGI().GetMaterial(i)->SetPixelShader(_resourceManager->GetShader(shaderName));
					}
					renderer->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,1,1, ratio });
				}
				co_await std::suspend_always();
			}
		}
		static const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		for (int i = 0; i < renderer->GetGI().GetMaterialCount(); ++i)
		{
			renderer->GetGI().SetMaterial(i, resourceManager->GetMaterial(renderer->GetGI().GetMaterial(i)->GetName(true)), true);
		}
		co_return;
	}

	bool Action_OrnamentShow::IsValid()
	{
		return (targetOrnament == nullptr) ? false : true;
	}

	void Action_OrnamentShow::SetTargetOrnament(editor::OrnamentData* ornament)
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}

		targetOrnament = ornament;
		if (ornament)
		{
			ornament->RegisterObserver(this);
		}
	}

	void Action_OrnamentShow::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
	}

	void Action_OrnamentShow::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetOrnament == static_cast<editor::OrnamentData*>(target))
				{
					targetOrnament = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Action_OrnamentShow::ImGui_DrawDataPopup(Action_OrnamentShow* data)
	{
		if (ImGui::MenuItem("SetTargetOrnament(Show)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetOrnament(Show)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& op = editor::palette::OrnamentPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetOrnament)
					{
						ImGui::Text(data->targetOrnament->pod.templateData->pod.staticFBXName.c_str());
						pp.ChangeTab("Ornament");
						op.Reset();
						op.SelectOrnamentInstance(data->targetOrnament);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Show)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_OrnamentShow>("SetTargetOrnament(Show)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Show)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetLerpTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float lerpTime = 0;
			lerpTime = data->lerpTime;
			editor::imgui::ShowMessageBox("SetLerpTime(OrnamentShow)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTimeOrnamentShow", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(OrnamentShow)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(OrnamentShow)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_OrnamentShow::PreEncoding(json& data) const
	{
		data["lerpTime"] = lerpTime;
		return true;
	}

	bool Action_OrnamentShow::PostEncoding(json& data) const
	{
		data["targetOrnament"] = targetOrnament ? UUID_To_String(targetOrnament->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_OrnamentShow::PreDecoding(const json& data)
	{
		lerpTime = data["lerpTime"];
		return true;
	}

	bool Action_OrnamentShow::PostDecoding(const json& data)
	{
		SetTargetOrnament(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::OrnamentData*>(String_To_UUID(data["targetOrnament"])));
		return true;
	}

	void Action_OrnamentShow::ProgressInitialize()
	{
		IAction::ProgressInitialize();
		savedActive = true;
	}

	void Action_OrnamentShow::CurrentProgressSave()
	{
		IAction::CurrentProgressSave();
		savedActive = targetOrnament->GetPaletteInstance()->GetGameObject()->GetSelfActive();
	}

	void Action_OrnamentShow::Recovery()
	{
		IAction::Recovery();
		if (!doAction && !savedActive)
		{
			auto ornamentInstance = targetOrnament->GetPaletteInstance();
			ornamentInstance->GetGameObject()->SetSelfActive(false);

			GameObject* targetObj = nullptr;

			for (auto each : ornamentInstance->GetGameObject()->GetChildren())
			{
				if (each->getName() != targetOrnament->pod.templateData->pod.staticFBXName)
				{
					continue;
				}

				targetObj = each;
			}

			yunutyEngine::graphics::StaticMeshRenderer* renderer = nullptr;
			for (auto each : targetObj->GetChildren())
			{
				renderer = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
				if (renderer)
				{
					break;
				}
			}

			if (renderer)
			{
				for (int i = 0; i < renderer->GetGI().GetMaterialCount(); ++i)
				{
					renderer->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,1,1,0 });
				}
			}

			targetOrnament->tookAction = false;
		}
	}

	Action_OrnamentHide::~Action_OrnamentHide()
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_OrnamentHide::DoAction()
	{
		targetOrnament->tookAction = true;

		if (lerpTime == 0 || !targetOrnament->GetPaletteInstance()->GetGameObject()->GetSelfActive())
		{
			targetOrnament->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
			co_return;
		}

		GameObject* targetObj = nullptr;

		for (auto each : targetOrnament->GetPaletteInstance()->GetGameObject()->GetChildren())
		{
			if (each->getName() != targetOrnament->pod.templateData->pod.staticFBXName)
			{
				continue;
			}

			targetObj = each;
		}

		yunutyEngine::graphics::StaticMeshRenderer* renderer = nullptr;
		for (auto each : targetObj->GetChildren())
		{
			renderer = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			if (renderer)
			{
				break;
			}
		}

		if (renderer)
		{
			float localTimer = 0;
			float ratio = 0;
			bool isFirst = false;

			bool isDeferred = (renderer->GetGI().GetMaterial()->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred);

			while (ratio < 1)
			{
				localTimer += yunutyEngine::Time::GetDeltaTime();
				ratio = localTimer / lerpTime;

				if (ratio > 1)
				{
					ratio = 1;
				}

				for (int i = 0; i < renderer->GetGI().GetMaterialCount(); ++i)
				{
					/// 추후 셰이더 교체까지 해줘야 함
					auto shaderName = renderer->GetGI().GetMaterial(i)->GetPixelShader()->GetName();

					size_t pos = shaderName.find(L"PS");
					if (pos != std::wstring::npos && !isFirst && isDeferred)
					{
						isFirst = true;
						const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
						shaderName = shaderName.substr(0, pos);
						shaderName += L"_AlphaPS.cso";
						renderer->GetGI().GetMaterial(i)->SetPixelShader(_resourceManager->GetShader(shaderName));
					}
					renderer->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,1,1, 1 - ratio });
				}
				co_await std::suspend_always();
			}
		}
		static const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		for (int i = 0; i < renderer->GetGI().GetMaterialCount(); ++i)
		{
			renderer->GetGI().SetMaterial(i, resourceManager->GetMaterial(renderer->GetGI().GetMaterial(i)->GetName(true)), true);
		}
		targetOrnament->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
		co_return;
	}

	bool Action_OrnamentHide::IsValid()
	{
		return (targetOrnament == nullptr) ? false : true;
	}

	void Action_OrnamentHide::SetTargetOrnament(editor::OrnamentData* ornament)
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}

		targetOrnament = ornament;
		if (ornament)
		{
			ornament->RegisterObserver(this);
		}
	}

	void Action_OrnamentHide::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
	}

	void Action_OrnamentHide::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetOrnament == static_cast<editor::OrnamentData*>(target))
				{
					targetOrnament = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Action_OrnamentHide::ImGui_DrawDataPopup(Action_OrnamentHide* data)
	{
		if (ImGui::MenuItem("SetTargetOrnament(Hide)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetOrnament(Hide)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& op = editor::palette::OrnamentPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetOrnament)
					{
						ImGui::Text(data->targetOrnament->pod.templateData->pod.staticFBXName.c_str());
						pp.ChangeTab("Ornament");
						op.Reset();
						op.SelectOrnamentInstance(data->targetOrnament);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Hide)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_OrnamentHide>("SetTargetOrnament(Hide)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Hide)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetLerpTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float lerpTime = 0;
			lerpTime = data->lerpTime;
			editor::imgui::ShowMessageBox("SetLerpTime(OrnamentHide)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTimeOrnamentHide", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(OrnamentHide)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(OrnamentHide)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_OrnamentHide::PreEncoding(json& data) const
	{
		data["lerpTime"] = lerpTime;
		return true;
	}

	bool Action_OrnamentHide::PostEncoding(json& data) const
	{
		data["targetOrnament"] = targetOrnament ? UUID_To_String(targetOrnament->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_OrnamentHide::PreDecoding(const json& data)
	{
		lerpTime = data["lerpTime"];
		return true;
	}

	bool Action_OrnamentHide::PostDecoding(const json& data)
	{
		SetTargetOrnament(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::OrnamentData*>(String_To_UUID(data["targetOrnament"])));
		return true;
	}

	void Action_OrnamentHide::ProgressInitialize()
	{
		IAction::ProgressInitialize();
		savedActive = false;
	}

	void Action_OrnamentHide::CurrentProgressSave()
	{
		IAction::CurrentProgressSave();
		savedActive = targetOrnament->GetPaletteInstance()->GetGameObject()->GetSelfActive();
	}

	void Action_OrnamentHide::Recovery()
	{
		IAction::Recovery();
		if (!doAction && savedActive)
		{
			auto ornamentInstance = targetOrnament->GetPaletteInstance();
			ornamentInstance->GetGameObject()->SetSelfActive(true);

			GameObject* targetObj = nullptr;

			for (auto each : ornamentInstance->GetGameObject()->GetChildren())
			{
				if (each->getName() != targetOrnament->pod.templateData->pod.staticFBXName)
				{
					continue;
				}

				targetObj = each;
			}

			yunutyEngine::graphics::StaticMeshRenderer* renderer = nullptr;
			for (auto each : targetObj->GetChildren())
			{
				renderer = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
				if (renderer)
				{
					break;
				}
			}

			if (renderer)
			{
				for (int i = 0; i < renderer->GetGI().GetMaterialCount(); ++i)
				{
					renderer->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,1,1,1 });
				}
			}

			targetOrnament->tookAction = false;
		}
	}

	Action_OrnamentFloating::~Action_OrnamentFloating()
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_OrnamentFloating::DoAction()
	{
		if (distance == 0 || roundTripTime == 0)
		{
			co_return;
		}

		auto ts = targetOrnament->GetPaletteInstance()->GetTransform();
		auto startPos = ts->GetWorldPosition();
		float localTimer = 0;
		float ratio = 0;
		float direction = 1.0f;
		if (!isUpDirection)
		{
			direction = -1.0f;
		}
		while (!prevCoroBreak)
		{
			localTimer += yunutyEngine::Time::GetDeltaTime();
			ratio = localTimer / roundTripTime;
			if (ratio > 1)
			{
				localTimer = 0;
				ratio = 0;
			}

			ts->SetWorldPosition(Vector3d(startPos.x, startPos.y + distance * direction * ((-std::cos(ratio * 2 * M_PI) + 1) * 0.5), startPos.z));
			co_await std::suspend_always();
		}

		prevCoroBreak = false;
	}

	bool Action_OrnamentFloating::IsValid()
	{
		return (targetOrnament == nullptr) ? false : true;
	}

	void Action_OrnamentFloating::SetTargetOrnament(editor::OrnamentData* ornament)
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}

		targetOrnament = ornament;
		if (ornament)
		{
			ornament->RegisterObserver(this);
		}
	}

	void Action_OrnamentFloating::SetDistance(float distance)
	{
		this->distance = distance;
	}

	void Action_OrnamentFloating::SetRoundTripTime(float roundTripTime)
	{
		this->roundTripTime = roundTripTime;
	}

	void Action_OrnamentFloating::SetDirection(bool isUp)
	{
		isUpDirection = isUp;
	}

	void Action_OrnamentFloating::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetOrnament == static_cast<editor::OrnamentData*>(target))
				{
					targetOrnament = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Action_OrnamentFloating::ImGui_DrawDataPopup(Action_OrnamentFloating* data)
	{
		if (ImGui::MenuItem("SetTargetOrnament(Floating)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetOrnament(Floating)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& op = editor::palette::OrnamentPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetOrnament)
					{
						ImGui::Text(data->targetOrnament->pod.templateData->pod.staticFBXName.c_str());
						pp.ChangeTab("Ornament");
						op.Reset();
						op.SelectOrnamentInstance(data->targetOrnament);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Floating)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_OrnamentFloating>("SetTargetOrnament(Floating)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Floating)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetFloatingDistance"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float distance = 0;
			distance = data->distance;
			editor::imgui::ShowMessageBox("SetFloatingDistance", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##OrnamentFloatingDistance", &distance);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetDistance(distance);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetFloatingDistance");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetFloatingDistance");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetRoundTripTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float roundTripTime = 0;
			roundTripTime = data->roundTripTime;
			editor::imgui::ShowMessageBox("SetRoundTripTime", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##OrnamentRoundTripTime", &roundTripTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetRoundTripTime(roundTripTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRoundTripTime");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRoundTripTime");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetDirection(OrnamentFloat)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool isUp = true;
			isUp = data->isUpDirection;
			editor::imgui::ShowMessageBox("SetDirection(OrnamentFloat)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("Up Direction", &isUp);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetDirection(isUp);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetDirection(OrnamentFloat)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetDirection(OrnamentFloat)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_OrnamentFloating::PreEncoding(json& data) const
	{
		data["distance"] = distance;
		data["roundTripTime"] = roundTripTime;
		data["isUpDirection"] = isUpDirection;
		return true;
	}

	bool Action_OrnamentFloating::PostEncoding(json& data) const
	{
		data["targetOrnament"] = targetOrnament ? UUID_To_String(targetOrnament->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_OrnamentFloating::PreDecoding(const json& data)
	{
		distance = data["distance"];
		roundTripTime = data["roundTripTime"];
		if (data.contains("isUpDirection"))
		{
			isUpDirection = data["isUpDirection"];
		}
		return true;
	}

	bool Action_OrnamentFloating::PostDecoding(const json& data)
	{
		SetTargetOrnament(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::OrnamentData*>(String_To_UUID(data["targetOrnament"])));
		return true;
	}

	void Action_OrnamentFloating::ProgressInitialize()
	{
		IAction::ProgressInitialize();
		prevCoroBreak = false;
	}

	void Action_OrnamentFloating::CurrentProgressSave()
	{
		IAction::CurrentProgressSave();
	}

	void Action_OrnamentFloating::Recovery()
	{
		if (!savedAction && doAction)
		{
			prevCoroBreak = true;
			targetOrnament->GetPaletteInstance()->GetTransform()->SetWorldPosition({ targetOrnament->pod.position.x,targetOrnament->pod.position.y,targetOrnament->pod.position.z });
		}

		IAction::Recovery();
	}
}