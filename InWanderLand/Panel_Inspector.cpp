#include "InWanderLand.h"
#include "Panel_Inspector.h"
#include "Application.h"
#include "PaletteList.h"
#include "CommandManager.h"
#include "TransformEditCommand.h"
#include "IEditableData.h"
#include "EditorCommonEvents.h"

#include "imgui.h"
#include "imgui_Utility.h"


namespace application
{
	namespace editor
	{
		InspectorPanel::~InspectorPanel()
		{

		}

		void InspectorPanel::Initialize()
		{

		}

		void InspectorPanel::Update(float ts)
		{

		}

		void InspectorPanel::GUIProgress()
		{
			ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoBringToFrontOnFocus);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역
			if (pm.GetCurrentPalette() && pm.GetCurrentPalette()->AreThereAnyObjectSelected())
			{
				int idx = 0;
				ImGui_DrawTransform(idx);
				imgui::draw::Underline();
				imgui::ShiftCursorY(10);
				ImGui_DrawTemplateData(idx);
				imgui::draw::Underline();
				imgui::ShiftCursorY(20);
				ImGui_DrawRestPOD(idx);
			}

			ImGui::End();
		}

		void InspectorPanel::Finalize()
		{

		}

		void InspectorPanel::OnEvent(EditorEvents& event)
		{
			editor::EventDispatcher dispatcher(event);
			dispatcher.Dispatch<editor::LoadEvent>([this](editor::LoadEvent& e)
				{
					saved = false;
					savedPosition = POD_Vector3<float>();
					savedQuaternion = POD_Quaternion<float>();
					savedScale.x = 1;
					savedScale.y = 1;
					savedScale.z = 1;
					return true;
				});
		}

		InspectorPanel::InspectorPanel()
		{
			savedScale.x = 1;
			savedScale.y = 1;
			savedScale.z = 1;
		}

		void InspectorPanel::ImGui_DrawTransform(int& idx)
		{
			auto& selections = pm.GetCurrentPalette()->GetSelections();
			static unsigned int containerSize = 30;
			static vector<TransformData> beforeTS(containerSize);
			static vector<bool> isEditing(containerSize);
			static std::vector<std::tuple<IEditableData*, TransformData, TransformData>> container(containerSize);
			
			if (idx > 0)
				imgui::ShiftCursorY(5.5f);

			imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32_WHITE);
			ImGui::Text("TransForm");
			ImGui::SameLine();
			auto buttonSize = ImGui::CalcTextSize("+");
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonSize.y - 10);
			
			{
				imgui::SmartStyleVar textAlign(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
				if (ImGui::ButtonEx("+", ImVec2(buttonSize.y + 5, buttonSize.y + 5), ImGuiButtonFlags_AlignTextBaseLine))
				{
					ImGui::OpenPopup("TransformEdit");
				}
			}

			if (ImGui::BeginPopup("TransformEdit"))
			{
				if (ImGui::MenuItem("Copy"))
				{
					if (selections.size() == 1)
					{
						auto wp = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldPosition();
						auto wr = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldRotation();
						auto ws = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldScale();
						savedPosition.x = wp.x;
						savedPosition.y = wp.y;
						savedPosition.z = wp.z;
						savedQuaternion.w = wr.w;
						savedQuaternion.x = wr.x;
						savedQuaternion.y = wr.y;
						savedQuaternion.z = wr.z;
						savedScale.x = ws.x;
						savedScale.y = ws.y;
						savedScale.z = ws.z;

						saved = true;
					}
				}
				if (ImGui::MenuItem("Paste"))
				{
					if (saved)
					{
						int containerIdx = 0;
						container.resize(0);
						beforeTS.resize(selections.size());
						Vector3d fpos = Vector3d(savedPosition.x, savedPosition.y, savedPosition.z);
						Quaternion fquat = Quaternion(savedQuaternion.w, savedQuaternion.x, savedQuaternion.y, savedQuaternion.z);
						Vector3d fscal = Vector3d(savedScale.x, savedScale.y, savedScale.z);
						for (auto& each : selections)
						{
							beforeTS[containerIdx] = each->GetPaletteInstance()->GetTransform();
							each->OnRelocate(fpos);
							each->OnRerotate(fquat);
							each->OnRescale(fscal);
							each->ApplyAsPaletteInstance();
							container.push_back({ each, beforeTS[containerIdx], each->GetPaletteInstance()->GetTransform() });
							containerIdx++;
						}
						CommandManager::GetSingletonInstance().AddQueue(std::make_shared<TransformEditCommand>(container));
					}
				}
				if (ImGui::MenuItem("Reset"))
				{
					int containerIdx = 0;
					container.resize(0);
					beforeTS.resize(selections.size());
					for (auto& each : selections)
					{
						beforeTS[containerIdx] = each->GetPaletteInstance()->GetTransform();
						each->OnRelocate(Vector3d());
						each->OnRerotate(Quaternion());
						each->OnRescale(Vector3d(1, 1, 1));
						each->ApplyAsPaletteInstance();
						container.push_back({ each, beforeTS[containerIdx], each->GetPaletteInstance()->GetTransform() });
						containerIdx++;
					}
					CommandManager::GetSingletonInstance().AddQueue(std::make_shared<TransformEditCommand>(container));
				}
				ImGui::EndPopup();
			}

			imgui::draw::Underline(IM_COL32(90, 90, 90, 200));
			imgui::ShiftCursorY(3.5f);

			bool result = ImGui::BeginTable("##section_table", 1, ImGuiTableFlags_SizingStretchSame);
			if (result)
			{
				ImGui::TableSetupColumn("Labels && Widgets", ImGuiTableColumnFlags_WidthFixed, ImGui::GetContentRegionAvail().x);
			}
			idx++;
			
			if (result)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				Vector3f position = Vector3f();
				Vector3f rotation = Vector3f();
				Quaternion rotQ = Quaternion();
				Vector3f scale = Vector3f(1, 1, 1);

				bool reset[9] = { false };
				bool endEdit = false;
				if (selections.size() == 1)
				{
					position = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldPosition();
					rotation = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldRotation().Euler();
					rotQ = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldRotation();
					auto prevRot = rotation;
					scale = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetLocalScale();

					auto resetPosition = imgui::Vector3Control("Position", position.x, position.y, position.z);
					auto resetRotation = imgui::Vector3Control("Rotation", rotation.x, rotation.y, rotation.z);
					auto resetScale = imgui::Vector3Control("Scale", scale.x, scale.y, scale.z);

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
						case application::editor::imgui::Vector3Flags::EndEditX:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditY:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditZ:
							endEdit = true;
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
						case application::editor::imgui::Vector3Flags::EndEditX:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditY:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditZ:
							endEdit = true;
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
						case application::editor::imgui::Vector3Flags::EndEditX:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditY:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditZ:
							endEdit = true;
							break;
						default:
							break;
					}

					if (reset[0])
					{
						position.x = 0;
					}
					else if (reset[1])
					{
						position.y = 0;
					}
					else if (reset[2])
					{
						position.z = 0;
					}
					else if (reset[3])
					{
						rotation.x = 0;
					}
					else if (reset[4])
					{
						rotation.y = 0;
					}
					else if (reset[5])
					{
						rotation.z = 0;
					}
					else if (reset[6])
					{
						scale.x = 1;
					}
					else if (reset[7])
					{
						scale.y = 1;
					}
					else if (reset[8])
					{
						scale.z = 1;
					}

					auto obj = (*selections.begin())->GetPaletteInstance()->GetGameObject();
					if (isEditing[0] == false &&
						(position != Vector3f(obj->GetTransform()->GetWorldPosition()) ||
							rotation != Vector3f(obj->GetTransform()->GetWorldRotation().Euler()) ||
							scale != Vector3f(obj->GetTransform()->GetLocalScale())))
					{
						beforeTS[0] = obj->GetTransform();
						isEditing[0] = true;
					}

					(*selections.begin())->OnRelocate(position);
					(*selections.begin())->OnRerotate(rotQ * Quaternion(rotation - prevRot));

					if (scale.x == 0)
					{
						scale.x = 0.000001;
					}
					if (scale.y == 0)
					{
						scale.y = 0.000001;
					}
					if (scale.z == 0)
					{
						scale.z = 0.000001;
					}

					(*selections.begin())->OnRescale(scale);
					(*selections.begin())->ApplyAsPaletteInstance();

					for (auto each : reset)
					{
						endEdit |= each;
					}

					if (endEdit)
					{
						auto obj = (*selections.begin())->GetPaletteInstance()->GetGameObject();
						CommandManager::GetSingletonInstance().AddQueue(std::make_shared<TransformEditCommand>(std::vector<std::tuple<IEditableData*, TransformData, TransformData>>(1, { *selections.begin(), beforeTS[0], obj->GetTransform() })));
						isEditing[0] = false;
					}
				}
				else
				{
					bool valDiff[9] = { false };
					bool reset[9] = { false };
					bool type[9] = { false };
					float errorRange = 0.0001;
					bool endEdit = false;

					Vector3f fposition = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldPosition();
					Vector3f frotation = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldRotation().Euler();
					Vector3f fscale = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetLocalScale();

					if (selections.size() > containerSize)
					{
						containerSize = selections.size();
						beforeTS.resize(containerSize);
						isEditing.resize(containerSize);
						container.reserve(containerSize);
					}

					for (auto each : selections)
					{
						position = each->GetPaletteInstance()->GetTransform()->GetWorldPosition();
						rotation = each->GetPaletteInstance()->GetTransform()->GetWorldRotation().Euler();
						rotQ = each->GetPaletteInstance()->GetTransform()->GetWorldRotation();
						scale = each->GetPaletteInstance()->GetTransform()->GetLocalScale();

						if (abs(fposition.x - position.x) > errorRange)
						{
							valDiff[0] |= true;
						}
						if (abs(fposition.y - position.y) > errorRange)
						{
							valDiff[1] |= true;
						}
						if (abs(fposition.z - position.z) > errorRange)
						{
							valDiff[2] |= true;
						}
						if (abs(frotation.x - rotation.x) > errorRange)
						{
							valDiff[3] |= true;
						}
						if (abs(frotation.y - rotation.y) > errorRange)
						{
							valDiff[4] |= true;
						}
						if (abs(frotation.z - rotation.z) > errorRange)
						{
							valDiff[5] |= true;
						}
						if (abs(fscale.x - scale.x) > errorRange)
						{
							valDiff[6] |= true;
						}
						if (abs(fscale.y - scale.y) > errorRange)
						{
							valDiff[7] |= true;
						}
						if (abs(fscale.z - scale.z) > errorRange)
						{
							valDiff[8] |= true;
						}
					}

					if (valDiff[0])
					{
						position.x = fposition.x;
					}
					if (valDiff[1])
					{
						position.y = fposition.y;
					}
					if (valDiff[2])
					{
						position.z = fposition.z;
					}
					if (valDiff[3])
					{
						rotation.x = frotation.x;
					}
					if (valDiff[4])
					{
						rotation.y = frotation.y;
					}
					if (valDiff[5])
					{
						rotation.z = frotation.z;
					}
					if (valDiff[6])
					{
						scale.x = fscale.x;
					}
					if (valDiff[7])
					{
						scale.y = fscale.y;
					}
					if (valDiff[8])
					{
						scale.z = fscale.z;
					}

					auto resetPosition = imgui::Vector3Control("Position", position.x, position.y, position.z, valDiff[0], valDiff[1], valDiff[2]);
					auto resetRotation = imgui::Vector3Control("Rotation", rotation.x, rotation.y, rotation.z, valDiff[3], valDiff[4], valDiff[5]);
					auto resetScale = imgui::Vector3Control("Scale", scale.x, scale.y, scale.z, valDiff[6], valDiff[7], valDiff[8]);

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
						case application::editor::imgui::Vector3Flags::TypeX:
							type[0] = true;
							break;
						case application::editor::imgui::Vector3Flags::TypeY:
							type[1] = true;
							break;
						case application::editor::imgui::Vector3Flags::TypeZ:
							type[2] = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditX:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditY:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditZ:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndTypeX:
						{
							type[0] = true;
							endEdit = true;
							break;
						}
						case application::editor::imgui::Vector3Flags::EndTypeY:
						{
							type[1] = true;
							endEdit = true;
							break;
						}
						case application::editor::imgui::Vector3Flags::EndTypeZ:
						{
							type[2] = true;
							endEdit = true;
							break;
						}
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
						case application::editor::imgui::Vector3Flags::TypeX:
							type[3] = true;
							break;
						case application::editor::imgui::Vector3Flags::TypeY:
							type[4] = true;
							break;
						case application::editor::imgui::Vector3Flags::TypeZ:
							type[5] = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditX:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditY:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditZ:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndTypeX:
						{
							type[3] = true;
							endEdit = true;
							break;
						}
						case application::editor::imgui::Vector3Flags::EndTypeY:
						{
							type[4] = true;
							endEdit = true;
							break;
						}
						case application::editor::imgui::Vector3Flags::EndTypeZ:
						{
							type[5] = true;
							endEdit = true;
							break;
						}
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
						case application::editor::imgui::Vector3Flags::TypeX:
							type[6] = true;
							break;
						case application::editor::imgui::Vector3Flags::TypeY:
							type[7] = true;
							break;
						case application::editor::imgui::Vector3Flags::TypeZ:
							type[8] = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditX:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditY:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndEditZ:
							endEdit = true;
							break;
						case application::editor::imgui::Vector3Flags::EndTypeX:
						{
							type[6] = true;
							endEdit = true;
							break;
						}
						case application::editor::imgui::Vector3Flags::EndTypeY:
						{
							type[7] = true;
							endEdit = true;
							break;
						}
						case application::editor::imgui::Vector3Flags::EndTypeZ:
						{
							type[8] = true;
							endEdit = true;
							break;
						}
						default:
							break;
					}

					Vector3d finalPosition = Vector3d();
					Vector3d finalRotation = Vector3d();
					Vector3d finalScale = Vector3d(1, 1, 1);

					Vector3f deltaPos = Vector3f();
					Vector3f deltaRot = Vector3f();
					Vector3f deltaScl = Vector3f(1, 1, 1);

					int idx = 0;

					for (auto each : selections)
					{
						deltaPos = position - fposition;
						deltaRot = rotation - frotation;
						deltaScl = scale - fscale;

						if (abs(deltaPos.x) <= errorRange)
						{
							deltaPos.x = 0;
						}
						if (abs(deltaPos.y) <= errorRange)
						{
							deltaPos.y = 0;
						}
						if (abs(deltaPos.z) <= errorRange)
						{
							deltaPos.z = 0;
						}
						if (abs(deltaRot.x) <= errorRange)
						{
							deltaRot.x = 0;
						}
						if (abs(deltaRot.y) <= errorRange)
						{
							deltaRot.y = 0;
						}
						if (abs(deltaRot.z) <= errorRange)
						{
							deltaRot.z = 0;
						}
						if (abs(deltaScl.x) <= errorRange)
						{
							deltaScl.x = 0;
						}
						if (abs(deltaScl.y) <= errorRange)
						{
							deltaScl.y = 0;
						}
						if (abs(deltaScl.z) <= errorRange)
						{
							deltaScl.z = 0;
						}

						finalPosition = each->GetPaletteInstance()->GetTransform()->GetWorldPosition() + deltaPos;
						finalRotation = each->GetPaletteInstance()->GetTransform()->GetWorldRotation().Euler() + deltaRot;
						auto prevRotQ = each->GetPaletteInstance()->GetTransform()->GetWorldRotation();
						finalScale = each->GetPaletteInstance()->GetTransform()->GetLocalScale() + deltaScl;

						if (reset[0])
						{
							finalPosition.x = 0;
						}
						else if (reset[1])
						{
							finalPosition.y = 0;
						}
						else if (reset[2])
						{
							finalPosition.z = 0;
						}
						else if (reset[3])
						{
							finalRotation.x = 0;
							deltaRot.x = -prevRotQ.Euler().x;
						}
						else if (reset[4])
						{
							finalRotation.y = 0;
							deltaRot.y = -prevRotQ.Euler().y;
						}
						else if (reset[5])
						{
							finalRotation.z = 0;
							deltaRot.z = -prevRotQ.Euler().z;
						}
						else if (reset[6])
						{
							finalScale.x = 1;
						}
						else if (reset[7])
						{
							finalScale.y = 1;
						}
						else if (reset[8])
						{
							finalScale.z = 1;
						}
						else if (type[0])
						{
							finalPosition.x = fposition.x;
						}
						else if (type[1])
						{
							finalPosition.y = fposition.y;
						}
						else if (type[2])
						{
							finalPosition.z = fposition.z;
						}
						else if (type[3])
						{
							finalRotation.x = frotation.x;
							deltaRot.z = frotation.x - prevRotQ.Euler().x;
						}
						else if (type[4])
						{
							finalRotation.y = frotation.y;
							deltaRot.y = frotation.y - prevRotQ.Euler().y;
						}
						else if (type[5])
						{
							finalRotation.z = frotation.z;
							deltaRot.z = frotation.z - prevRotQ.Euler().z;
						}
						else if (type[6])
						{
							finalScale.x = fscale.x;
						}
						else if (type[7])
						{
							finalScale.y = fscale.y;
						}
						else if (type[8])
						{
							finalScale.z = fscale.z;
						}

						auto objTS = each->GetPaletteInstance()->GetGameObject()->GetTransform();
						if ((isEditing[idx] == false) &&
							(finalPosition != objTS->GetWorldPosition() ||
								finalRotation != objTS->GetWorldRotation().Euler() ||
								finalScale != objTS->GetLocalScale()))
						{
							if (isEditing[0] == false)
							{
								beforeTS[0] = (*selections.begin())->GetPaletteInstance()->GetGameObject()->GetTransform();
								isEditing[0] = true;
							}

							beforeTS[idx] = objTS;
							isEditing[idx] = true;
						}

						each->OnRelocate(finalPosition);
						each->OnRerotate(prevRotQ * Quaternion(deltaRot));

						if (finalScale.x == 0)
						{
							finalScale.x = 0.000001;
						}
						if (finalScale.y == 0)
						{
							finalScale.y = 0.000001;
						}
						if (finalScale.z == 0)
						{
							finalScale.z = 0.000001;
						}

						each->OnRescale(finalScale);
						each->ApplyAsPaletteInstance();

						idx++;
					}

					for (auto& each : reset)
					{
						endEdit |= each;
					}

					if (endEdit)
					{
						if (type[0])
						{
							for (auto& each : selections)
							{
								auto pos = each->GetPaletteInstance()->GetTransform()->GetWorldPosition();
								pos.x = finalPosition.x;
								each->OnRelocate(pos);
								each->ApplyAsPaletteInstance();
							}
						}
						else if (type[1])
						{
							for (auto& each : selections)
							{
								auto pos = each->GetPaletteInstance()->GetTransform()->GetWorldPosition();
								pos.y = finalPosition.y;
								each->OnRelocate(finalPosition);
								each->ApplyAsPaletteInstance();
							}
						}
						else if (type[2])
						{
							for (auto& each : selections)
							{
								auto pos = each->GetPaletteInstance()->GetTransform()->GetWorldPosition();
								pos.z = finalPosition.z;
								each->OnRelocate(finalPosition);
								each->ApplyAsPaletteInstance();
							}
						}
						else if (type[3])
						{
							for (auto& each : selections)
							{
								auto rot = each->GetPaletteInstance()->GetTransform()->GetWorldRotation();
								each->OnRerotate(rot * Quaternion(Vector3d(finalRotation.x - rot.Euler().x, 0, 0)));
								each->ApplyAsPaletteInstance();
							}
						}
						else if (type[4])
						{
							for (auto& each : selections)
							{
								auto rot = each->GetPaletteInstance()->GetTransform()->GetWorldRotation();
								each->OnRerotate(rot* Quaternion(Vector3d(finalRotation.y - rot.Euler().y, 0, 0)));
								each->ApplyAsPaletteInstance();
							}
						}
						else if (type[5])
						{
							for (auto& each : selections)
							{
								auto rot = each->GetPaletteInstance()->GetTransform()->GetWorldRotation();
								each->OnRerotate(rot * Quaternion(Vector3d(finalRotation.z - rot.Euler().z, 0, 0)));
								each->ApplyAsPaletteInstance();
							}
						}
						else if (type[6])
						{
							for (auto& each : selections)
							{
								auto scl = each->GetPaletteInstance()->GetTransform()->GetWorldScale();
								scl.x = finalScale.x;
								each->OnRescale(scl);
								each->ApplyAsPaletteInstance();
							}
						}
						else if (type[7])
						{
							for (auto& each : selections)
							{
								auto scl = each->GetPaletteInstance()->GetTransform()->GetWorldScale();
								scl.y = finalScale.y;
								each->OnRescale(scl);
								each->ApplyAsPaletteInstance();
							}
						}
						else if (type[8])
						{
							for (auto& each : selections)
							{
								auto scl = each->GetPaletteInstance()->GetTransform()->GetWorldScale();
								scl.z = finalScale.z;
								each->OnRescale(scl);
								each->ApplyAsPaletteInstance();
							}
						}

						container.resize(0);
						int idx = 0;
						for (auto& each : selections)
						{
							container.push_back({ each, beforeTS[idx], each->GetPaletteInstance()->GetTransform() });
							isEditing[idx] = false;
							idx++;
						}
						CommandManager::GetSingletonInstance().AddQueue(std::make_shared<TransformEditCommand>(container));
					}
				}

				imgui::EndSection();
			}
		}

		void InspectorPanel::ImGui_DrawTemplateData(int& idx)
		{
			auto& tdm = TemplateDataManager::GetSingletonInstance();
			auto& selections = pm.GetCurrentPalette()->GetSelections();
			if (imgui::BeginSection_1Col(idx, "Template", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				std::vector<ITemplateData*> templateList;
				std::vector<std::string> list;
				static int selected = -1;
				static int count = 0;

				const char* current = nullptr;

				auto type = pm.GetCurrentPaletteType();
				switch (type)
				{
					case application::editor::palette::Palette_List::Terrain:
					{
						imgui::EndSection();
						return;
					}
					case application::editor::palette::Palette_List::Unit:
					{
						templateList = tdm.GetDataList(DataType::UnitData);
						break;
					}
					case application::editor::palette::Palette_List::Ornament:
					{
						templateList = tdm.GetDataList(DataType::OrnamentData);
						break;
					}
					case application::editor::palette::Palette_List::Region:
					{
						imgui::EndSection();
						return;
					}
					case application::editor::palette::Palette_List::Wave:
					{
						imgui::EndSection();
						return;
					}
					case application::editor::palette::Palette_List::Cam:
					{
						templateList = tdm.GetDataList(DataType::CameraData);
						break;
					}
					case application::editor::palette::Palette_List::Light:
					{
						templateList = tdm.GetDataList(DataType::LightData);
						break;
					}
					case application::editor::palette::Palette_List::Particle:
					{
						templateList = tdm.GetDataList(DataType::ParticleData);
						break;
					}
					case application::editor::palette::Palette_List::Interactable:
					{
						if (selections.size() == 1)
						{
							ImGui::Text((*selections.begin())->GetTemplateData()->GetDataKey().c_str());
						}
						else
						{
							ImGui::Text("-------------");
						}
						imgui::EndSection();
						return;
					}
					default:
						break;
				}

				for (auto& each : templateList)
				{
					list.push_back(tdm.GetDataKey(each));
				}

				count = list.size();

				if (selections.size() == 1)
				{
					int fbxIdx = 0;
					for (auto& each : list)
					{
						if (each == (*selections.begin())->GetTemplateData()->GetDataKey())
						{
							selected = fbxIdx;
						}
						fbxIdx++;
					}
					current = list[selected].c_str();
				}
				else
				{
					bool diff = false;
					auto firstName = (*selections.begin())->GetTemplateData()->GetDataKey();
					for (auto& each : selections)
					{
						if (each->GetTemplateData()->GetDataKey() != firstName)
						{
							diff = true;
							break;
						}
					}

					if (diff)
					{
						selected = -1;
						current = "-----------";
					}
					else
					{
						int fbxIdx = 0;
						for (auto& each : list)
						{
							if (each == firstName)
							{
								selected = fbxIdx;
							}
							fbxIdx++;
						}
						current = list[selected].c_str();
					}
				}

				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32_WHITE);
				imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(5, 10.f));

				ImGui::PushItemWidth(-1);
				if (ImGui::BeginCombo("##Combo", current))
				{
					for (int i = 0; i < count; i++)
					{
						const bool is_selected = (current == list[i]);

						if (ImGui::Selectable(list[i].c_str(), is_selected))
						{
							current = list[i].c_str();
							selected = i;

							for (auto each : selections)
							{
								each->SetTemplateData(list[i]);
							}
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopItemWidth();

				imgui::EndSection();
			}
		}

		void InspectorPanel::ImGui_DrawRestPOD(int& idx)
		{
			auto& selections = pm.GetCurrentPalette()->GetSelections();
			if (imgui::BeginSection_2Col(idx, "Rest POD Data", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				if (selections.size() == 1)
				{
					auto type = TemplateDataManager::GetSingletonInstance().GetDataType((*selections.begin())->GetTemplateData());
					switch (type)
					{
						case application::editor::DataType::TerrainData:
						{
							break;
						}
						case application::editor::DataType::UnitData:
						{
							DrawRestPOD<POD_Unit>(static_cast<UnitData*>(*selections.begin())->pod);
							break;
						}
						case application::editor::DataType::OrnamentData:
						{
							DrawRestPOD<POD_Ornament>(static_cast<OrnamentData*>(*selections.begin())->pod);
							break;
						}
						case application::editor::DataType::RegionData:
						{
							break;
						}
						case application::editor::DataType::WaveData:
						{
							break;
						}
						case application::editor::DataType::CameraData:
						{
							break;
						}
						case application::editor::DataType::LightData:
						{
							break;
						}
						case application::editor::DataType::ParticleData:
						{
							break;
						}
						case application::editor::DataType::InteractableData:
						{
							break;
						}
						default:
							break;
					}
				}

				imgui::EndSection();
			}
		}
	}
}
