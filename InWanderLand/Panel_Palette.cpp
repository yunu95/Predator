#include "InWanderLand.h"
#include "Panel_Palette.h"
#include "Application.h"
#include "imgui_Utility.h"
#include "EditorCommonEvents.h"
#include "Wave_TemplateData.h"
#include "Region_TemplateData.h"
#include "SpecialEvent.h"
#include "WavePalette.h"
#include "RegionPalette.h"
#include "EditorLayer.h"
#include "InstanceManager.h"
#include "EditorCameraManager.h"
#include "EditorCamera.h"
#include "CommandManager.h"
#include "TransformEditCommand.h"
#include "EditableDataList.h"
#include "ParticleTool_Manager.h"
#include "BrushList.h"

#include "YunutyEngine.h"
#include "WanderUtils.h"

#include "imgui.h"

const int bufferSize = 255;
static char* unitNameBuffer = new char[bufferSize];
static char* interatableNameBuffer = new char[bufferSize];

namespace application
{
	namespace editor
	{
		PalettePanel::~PalettePanel()
		{

		}

		void PalettePanel::Initialize()
		{
			memset(unitNameBuffer, 0, bufferSize);
			memset(interatableNameBuffer, 0, bufferSize);

			currentPalette = pm.GetCurrentPalette();

			auto uSize = tdm.GetDataList(DataType::UnitData).size();
		}

		void PalettePanel::Update(float ts)
		{
			static Vector3d offset{ 0.1, 0.072, 0.2 };
			if (lightGizmo)
			{
				yunuGI::Vector3 camPos = ec.GetPosition();
				Vector3f pos = *reinterpret_cast<Vector3f*>(&camPos);
				yunuGI::Vector3 front = ec.GetForwardDirection();
				yunuGI::Vector3 right = ec.GetRightDirection();
				yunuGI::Vector3 up = ec.GetUpDirection();
				Vector3f finalPos = pos + (*reinterpret_cast<Vector3f*>(&front) * offset.z)
					+ (*reinterpret_cast<Vector3f*>(&right) * offset.x)
					+ (*reinterpret_cast<Vector3f*>(&up) * offset.y);
				lightGizmo->GetTransform()->SetWorldPosition(finalPos);

				if (!Application::GetInstance().IsContentsPlaying())
				{
					if (ec.GetCameraTypeState() == CameraTypeState::Editor
						&& ec.GetCameraPerspectiveState() == CameraPerspectiveState::Free)
					{
						lightGizmo->SetSelfActive(true);
					}
					else
					{
						lightGizmo->SetSelfActive(false);
					}
				}
			}
		}

		void PalettePanel::GUIProgress()
		{
			ImGui::Begin("Palette");

			ImGui_Update();

			/// 실제 패널에 그리는 영역
			if (ImGui::BeginTabBar("PaletteTabBar", ImGuiTabBarFlags_None))
			{
				/// 플래그를 사용하여 정렬 상태를 조정해야 하지만,
				/// 그냥 간편하게 기본적으로 Terrain 으로 시작된다고 가정함
				bool tabJustOpened = false;
				if (ImGui::BeginTabItem("Terrain"))
				{
					ChangePalette(&tp);
					ImGui_BeginTerrainPalette();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Unit"))
				{
					ChangePalette(&up);
					ImGui_BeginUnitPalette();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Ornament"))
				{
					ChangePalette(&op);
					ImGui_BeginOrnamentPalette();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Region"))
				{
					ChangePalette(&rp);
					ImGui_BeginRegionPalette();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Wave"))
				{
					ChangePalette(&wp);
					ImGui_BeginWavePalette();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Light"))
				{
					ChangePalette(&lp);
					ImGui_BeginLightPalette();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Cam"))
				{
					ChangePalette(&cp);
					ImGui_BeginCameraPalette();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Particle"))
				{
					ChangePalette(&pp);
					ImGui_BeginParticlePalette();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Interactable"))
				{
					ChangePalette(&ip);
					ImGui_BeginInteractablePalette();
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		void PalettePanel::Finalize()
		{

		}

		void PalettePanel::OnEvent(EditorEvents& event)
		{
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<LoadEvent>([this](LoadEvent& e)
				{
					LoadCallback();
					return true;
				});
		}

		void PalettePanel::OnPlayContents()
		{
			lightGizmo->SetSelfActive(false);
		}

		void PalettePanel::OnPauseContents()
		{
			lightGizmo->SetSelfActive(true);
		}

		void PalettePanel::OnResumeContents()
		{
			lightGizmo->SetSelfActive(false);
		}

		void PalettePanel::OnStopContents()
		{
			lightGizmo->SetSelfActive(true);
		}

		void PalettePanel::ChangeTab(const std::string& tabName)
		{
			ImGuiContext* g = ImGui::GetCurrentContext();
			ImGuiWindow* window = ImGui::FindWindowByName("Palette");
			ImGuiID id = window->GetID("PaletteTabBar");
			ImGuiTabBar* tab_bar = g->TabBars.GetOrAddByKey(id);

			if (tabName == "Terrain")
			{
				ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[0]);
			}
			else if (tabName == "Unit")
			{
				ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[1]);
			}
			else if (tabName == "Ornament")
			{
				ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[2]);
			}
			else if (tabName == "Region")
			{
				ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[3]);
			}
			else if (tabName == "Wave")
			{
				ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[4]);
			}
			else if (tabName == "Light")
			{
				ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[5]);
			}
			else if (tabName == "Cam")
			{
				ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[6]);
			}
			else if (tabName == "Particle")
			{
				ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[7]);
			}
			else if (tabName == "Interactable")
			{
				ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[8]);
			}
		}

		void PalettePanel::ChangeDirectionalLight(LightData* light)
		{
			if (directionalLight == light)
			{
				return;
			}

			directionalLight = light;
			directionalLight->GetPaletteInstance()->SetActive(false);

			if (lightGizmo == nullptr)
			{
				lightGizmo = Scene::getCurrentScene()->AddGameObject();
				lightGizmo->GetTransform()->SetLocalScale(Vector3d(0.0016, 0.0016, 0.0016));
				auto mesh = Scene::getCurrentScene()->AddGameObjectFromFBX("Directional");
				for (auto each : mesh->GetChildren())
				{
					auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

					if (comp)
					{
						for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
						{
							comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
							comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0.9,0,0.5 });
						}
					}
				}
				mesh->SetParent(lightGizmo);
			}
			UpdataLightGizmo();
		}

		PalettePanel::PalettePanel()
		{

		}

		void PalettePanel::ChangePalette(palette::Palette* palette)
		{
			if (currentPalette != palette)
			{
				pm.SetCurrentPalette(palette);
				currentPalette = palette;
			}
		}

		void PalettePanel::ImGui_Update()
		{
			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			if (!Application::IsFocusGameWindow() && EditorLayer::GetInputControl())
			{
				// 마우스 입력에 대한 처리
				if (isMouseOver)
				{

				}

				// 키 입력에 대한 처리
				if (eim.IsKeyboardUp(KeyCode::ESC))
				{
					if (currentPalette && !currentPalette->IsSelectMode())
					{
						currentPalette->SetAsSelectMode(true);

						if (currentPalette == &up)
						{	
							unitCurrentButton = -1;
							up.SelectUnitTemplateData(nullptr);
						}
						else if (currentPalette == &op)
						{
							ornamentCurrentButton = -1;
							op.SelectOrnamentTemplateData(nullptr);
						}
						else if (currentPalette == &lp)
						{
							lightCurrentButton = -1;
							lp.SelectLightTemplateData(nullptr);
						}
						else if (currentPalette == &pp)
						{
							pp.InitParticleData();
						}
						else if (currentPalette == &ip)
						{
							interactableCurrentButton = -1;
							ip.SelectInteractableTemplateData(nullptr);
						}
					}
				}

				if (eim.IsKeyboardUp(KeyCode::Delete))
				{
					if (currentPalette && currentPalette->IsSelectMode())
					{
						currentPalette->OnDeletion();
					}
					else if (currentPalette == &up && up.GetSelections().empty() && unitCurrentButton != -1)
					{
						editor::EditorLayer::SetInputControl(false);
						editor::imgui::ShowMessageBox("Delete Unit Template", [&]()
							{
								editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

								ImGui::Separator();

								ImGui::SetNextItemWidth(-1);
								ImGui::Text(("Are you sure you want to delete the \"" + tdm.GetDataList(DataType::UnitData)[unitCurrentButton]->GetDataKey() + "\"?").c_str());

								ImGui::Separator();

								if (ImGui::Button("Delete"))
								{
									auto tKey = tdm.GetDataList(DataType::UnitData)[unitCurrentButton]->GetDataKey();
									for (auto& unitData : InstanceManager::GetSingletonInstance().GetList<UnitData>())
									{
										if (unitData->pod.templateData->GetDataKey() == tKey)
										{
											for (auto& waveData : InstanceManager::GetSingletonInstance().GetList<WaveData>())
											{
												/// 해당 Wave 에서 unitData 를 가지고 있었을 경우에 삭제하는 로직이 필요합니다.
												std::erase_if(waveData->GetWaveUnitDataMap(), [=](const auto& each) {return each.first == unitData; });
												waveData->ApplyMapAsPod();
											}

											auto editorInstance = unitData->ApplyAsPaletteInstance();
											if (editorInstance)
											{
												Scene::getCurrentScene()->DestroyGameObject(editorInstance->GetGameObject());
											}

											InstanceManager::GetSingletonInstance().DeleteInstance(unitData->GetUUID());
										}
									}

									up.SetAsSelectMode(true);
									tdm.DeleteTemplateData(tKey);
									unitCurrentButton = -1;
									up.SelectUnitTemplateData(nullptr);

									ImGui::CloseCurrentPopup();
									editor::imgui::CloseMessageBox("Delete Unit Template");
									editor::EditorLayer::SetInputControl(true);
								}
								ImGui::SameLine();

								if (ImGui::Button("Cancel"))
								{
									ImGui::CloseCurrentPopup();
									editor::imgui::CloseMessageBox("Delete Unit Template");
									editor::EditorLayer::SetInputControl(true);
								}
							}, 500);
					}
					else if (currentPalette == &ip && ip.GetSelections().empty() && interactableCurrentButton != -1)
					{
						editor::EditorLayer::SetInputControl(false);
						editor::imgui::ShowMessageBox("Delete Interactalbe Template", [&]()
							{
								editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

								ImGui::Separator();

								ImGui::SetNextItemWidth(-1);
								ImGui::Text(("Are you sure you want to delete the \"" + tdm.GetDataList(DataType::InteractableData)[interactableCurrentButton]->GetDataKey() + "\"?").c_str());

								ImGui::Separator();

								if (ImGui::Button("Delete"))
								{
									auto tKey = tdm.GetDataList(DataType::InteractableData)[interactableCurrentButton]->GetDataKey();
									for (auto& interactable : InstanceManager::GetSingletonInstance().GetList<InteractableData>())
									{
										if (interactable->pod.templateData->GetDataKey() == tKey)
										{
											auto editorInstance = interactable->ApplyAsPaletteInstance();
											if (editorInstance)
											{
												Scene::getCurrentScene()->DestroyGameObject(editorInstance->GetGameObject());
											}

											InstanceManager::GetSingletonInstance().DeleteInstance(interactable->GetUUID());
										}
									}

									ip.SetAsSelectMode(true);
									tdm.DeleteTemplateData(tKey);
									interactableCurrentButton = -1;
									ip.SelectInteractableTemplateData(nullptr);

									ImGui::CloseCurrentPopup();
									editor::imgui::CloseMessageBox("Delete Interactalbe Template");
									editor::EditorLayer::SetInputControl(true);
								}
								ImGui::SameLine();

								if (ImGui::Button("Cancel"))
								{
									ImGui::CloseCurrentPopup();
									editor::imgui::CloseMessageBox("Delete Interactalbe Template");
									editor::EditorLayer::SetInputControl(true);
								}
							}, 500);
					}
				}
			}
		}

		void PalettePanel::ImGui_BeginTerrainPalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

			int countIdx = 0;

			if (imgui::BeginSection_1Col(countIdx, "Place Mode", ImGui::GetContentRegionAvail().x))
			{
				bool pButton = !tp.IsSelectMode() && tp.IsMarking();
				bool eButton = !tp.IsSelectMode() && !tp.IsMarking();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				bool pFlag = imgui::SelectableImageButton("Terrain_PlaceButton", "ImageButtons/Terrain_PlaceButton.png", pButton, ImVec2(50, 50));
				ImGui::SameLine();
				bool eFlag = imgui::SelectableImageButton("Terrain_EraseButton", "ImageButtons/Terrain_EraseButton.png", eButton, ImVec2(50, 50));

				if (pFlag)
				{
					if (pButton)
					{
						tp.SetAsSelectMode(true);
					}
					else
					{
						tp.SetAsSelectMode(false);
						tp.SetIsMarking(true);
					}
				}

				if (eFlag)
				{
					if (eButton)
					{
						tp.SetAsSelectMode(true);
					}
					else
					{
						tp.SetAsSelectMode(false);
						tp.SetIsMarking(false);
					}
				}

				imgui::EndSection();
			}

			if (imgui::BeginSection_2Col(countIdx, "Brush Options", ImGui::GetContentRegionAvail().x, 0.3f))
			{
				int bSize = tp.GetBrushSize();

				if (imgui::SliderInt_2Col("Size", bSize, 0, 15))
				{
					tp.SetBrushSize(bSize);
				}

				imgui::EndSection();
			}

			if (imgui::BeginSection_1Col(countIdx, "Navigation Build", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Button("Build", ImVec2(ImGui::GetContentRegionAvail().x, 50)))
				{
					tp.ApplyAsPlaytimeObjects();
				}

				imgui::EndSection();
			}
		}

		void PalettePanel::ImGui_BeginUnitPalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

			int countIdx = 0;
			bool isPlacingWaveUnit = palette::WavePalette::SingleInstance().currentWaveData && palette::WavePalette::SingleInstance().currentSelectedWaveIndex >= 0;
			if (isPlacingWaveUnit)
			{
				//stringstream ss;
				//ss << "Wave is selected. Please select time offset and unit type to place.";
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.04f, 0.0f, 0.96f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
				//ImGui::TextColored({ 1,0,0,1 }, "Wave is selected. Please select time offset and unit type to place.");
				ImGui::TextWrapped("Wave is selected. Please select time offset and unit type to place.");
				ImGui::PopStyleColor();
				float timeOffset = palette::WavePalette::SingleInstance().GetCurrentSelectedWaveTimeOffset();
				ImGui::DragFloat("time offset", &timeOffset, 0.005, 0, 10000);
				palette::WavePalette::SingleInstance().SetCurrentSelectedWaveTimeOffset(timeOffset);
			}

			if (imgui::BeginSection_1Col(countIdx, "Unit List", ImGui::GetContentRegionAvail().x))
			{
				auto uSize = tdm.GetDataList(DataType::UnitData).size();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				for (int i = 0; i < uSize + 1; i++)
				{
					imgui::SmartStyleVar textAlign(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
					if (i == uSize)
					{
						if (ImGui::Selectable("+", false))
						{
							if (unitCurrentButton != -1)
							{
								unitCurrentButton = -1;
								up.SelectUnitTemplateData(nullptr);
								up.SetAsSelectMode(true);
							}
							ImGui_CreateUnitPopup();
							EditorLayer::SetInputControl(false);
						}
					}
					else
					{
						if (ImGui::Selectable(static_cast<Unit_TemplateData*>(tdm.GetDataList(DataType::UnitData)[i])->GetDataKey().c_str(), unitCurrentButton == i))
						{
							if (unitCurrentButton != -1)
							{
								if (unitCurrentButton == i)
								{
									unitCurrentButton = -1;
									up.SelectUnitTemplateData(nullptr);
									up.SetAsSelectMode(true);
								}
								else
								{
									unitCurrentButton = i;
									up.SelectUnitTemplateData(static_cast<Unit_TemplateData*>(tdm.GetDataList(DataType::UnitData)[i]));
									up.SetAsSelectMode(false);
								}
							}
							else
							{
								unitCurrentButton = i;
								up.SelectUnitTemplateData(static_cast<Unit_TemplateData*>(tdm.GetDataList(DataType::UnitData)[i]));
								up.SetAsSelectMode(false);
							}
						}
					}
				}
				imgui::EndSection();
			}
		}

		void PalettePanel::ImGui_BeginOrnamentPalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

			int countIdx = 0;

			imgui::ShiftCursorY(5);
			{
				imgui::SmartStyleVar textAlign(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
				if (ImGui::Button("Clear Ornaments", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
				{
					for (auto& each : InstanceManager::GetSingletonInstance().GetList<OrnamentData>())
					{
						op.Delete(each);
					}
				}
			}
			imgui::ShiftCursorY(5);

			if (imgui::BeginSection_1Col(countIdx, "Ornament List", ImGui::GetContentRegionAvail().x))
			{
				auto oSize = tdm.GetDataList(DataType::OrnamentData).size();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				for (int i = 0; i < oSize; i++)
				{
					imgui::SmartStyleVar textAlign(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
					if (ImGui::Selectable(static_cast<Ornament_TemplateData*>(tdm.GetDataList(DataType::OrnamentData)[i])->GetDataKey().c_str(), ornamentCurrentButton == i))
					{
						if (ornamentCurrentButton != -1)
						{
							if (ornamentCurrentButton == i)
							{
								ornamentCurrentButton = -1;
								op.SelectOrnamentTemplateData(nullptr);
								op.SetAsSelectMode(true);
							}
							else
							{
								ornamentCurrentButton = i;
								op.SelectOrnamentTemplateData(static_cast<Ornament_TemplateData*>(tdm.GetDataList(DataType::OrnamentData)[i]));
								op.SetAsSelectMode(false);
							}
						}
						else
						{
							ornamentCurrentButton = i;
							op.SelectOrnamentTemplateData(static_cast<Ornament_TemplateData*>(tdm.GetDataList(DataType::OrnamentData)[i]));
							op.SetAsSelectMode(false);
						}
					}
				}
				imgui::EndSection();
			}
		}

		void PalettePanel::ImGui_BeginRegionPalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

			//static int selectedRegionIdx = 0;
			//RegionData* selectedRegion = nullptr;
			if (ImGui::Button("Make new region"))
			{
				auto newInstance = InstanceManager::GetSingletonInstance().CreateInstance<RegionData>(Region_TemplateData::GetInstance().GetDataKey());
				auto createPoint = graphics::Camera::GetMainCamera()->GetProjectedPoint({ 0.0,0.0 }, graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition().Magnitude(), { 0,1,0 });
				newInstance->pod.angle = 0;
				newInstance->pod.width = 5;
				newInstance->pod.height = 5;
				newInstance->pod.x = createPoint.x;
				newInstance->pod.z = createPoint.z;
				newInstance->ApplyAsPaletteInstance();
			}

			int countIdx{ 0 };
			if (imgui::BeginSection_1Col(countIdx, "Region List", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x * 0.8f,400 }))
				{
					int countIdx = 0;
					for (auto each : RegionData::GetInstances())
					{
						stringstream ss;
						ss << yutility::GetString(each->pod.name).c_str() << " ##RegionSelectable" << countIdx;
						if (ImGui::Selectable(ss.str().c_str(), palette::RegionPalette::SingleInstance().GetSingleSelectedRegion() == each))
						{
							palette::RegionPalette::SingleInstance().SelectRegion(each);
						}
						if (palette::RegionPalette::SingleInstance().GetSingleSelectedRegion() == each)
						{
							ImGui::SetItemDefaultFocus();
						}
						countIdx++;
					}
					ImGui::EndListBox();
				}
				imgui::EndSection();
			}
			if (palette::RegionPalette::SingleInstance().GetSingleSelectedRegion())
			{
				if (imgui::BeginSection_2Col(countIdx, "Region info", ImGui::GetContentRegionAvail().x))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Region name");
					ImGui::TableNextColumn();
					string charBuffer = yutility::GetString(palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.name);
					charBuffer.reserve(64);
					ImGui::InputText("##RegionNameInputText", &charBuffer[0], 64);

					static bool beforeFocus = false;
					if (ImGui::IsItemFocused())
					{
						beforeFocus = true;
						EditorLayer::SetInputControl(false);
						ec.SetInputUpdate(false);
					}
					else if (beforeFocus)
					{
						beforeFocus = false;
						EditorLayer::SetInputControl(true);
						ec.SetInputUpdate(true);
					}

					palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.name = yutility::GetWString(charBuffer);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("width");
					ImGui::TableNextColumn();
					if (ImGui::DragFloat("##RegionWidthInputText", &palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.width, 0.005f, 0.0f, FLT_MAX, "%.2f", 0))
						palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->ApplyAsPaletteInstance();
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("height");
					ImGui::TableNextColumn();
					if (ImGui::DragFloat("##RegionHeightInputText", &palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.height, 0.005f, 0.0f, FLT_MAX, "%.2f", 0))
						palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->ApplyAsPaletteInstance();
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("angle");
					ImGui::TableNextColumn();
					if (ImGui::DragFloat("##RegionAngleInputText", &palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.angle, 0.05f, 0.0f, 360, "%.2f", 0))
						palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->ApplyAsPaletteInstance();
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("special event");
					ImGui::TableNextColumn();
					if (ImGui::BeginCombo("##RegionSpecialEvent", SpecialEventTypeToString(static_cast<SpecialEventType>(palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.specialEvent)).c_str()))
					{
						for (int n = 0; n < SpecialEventTypes().size(); n++)
						{
							const bool is_selected = (palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.specialEvent == n);
							if (ImGui::Selectable((SpecialEventTypeStrings()[n] + "##SpecialEventSelectable").c_str(), is_selected))
								palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.specialEvent = n;

							// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("is obstacle?");
					ImGui::TableNextColumn();
					ImGui::Checkbox("##RegionIsObstacle", &palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.isObstacle);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (!palette::RegionPalette::SingleInstance().GetIsSelectingDisablingOrnaments())
					{
						if (ImGui::Button("Select disabling ornaments"))
						{
							palette::RegionPalette::SingleInstance().SetAsSelectingDisablingOrnaments(true);
						}
					}
					else
					{
						if (ImGui::Button("Select regions"))
						{
							palette::RegionPalette::SingleInstance().SetAsSelectingDisablingOrnaments(false);
						}
					}

					if (ImGui::Button("Delete Region"))
					{
						auto region{ palette::RegionPalette::SingleInstance().GetSingleSelectedRegion() };
						for (auto each : WaveData::GetInstances())
						{
							if (each->pod.contraintRegion == region)
							{
								each->pod.contraintRegion = nullptr;
							}
							if (each->pod.triggerRegion == region)
							{
								each->pod.triggerRegion = nullptr;
							}
						}
						palette::RegionPalette::SingleInstance().OnDeletion();
					}
					imgui::EndSection();
				}
			}
		}

		void PalettePanel::ImGui_BeginWavePalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
			WaveData*& selectedWave = palette::WavePalette::SingleInstance().currentWaveData;
			if (ImGui::Button("Make new wave"))
				InstanceManager::GetSingletonInstance().CreateInstance<WaveData>(Wave_TemplateData::GetInstance().GetDataKey());

			int countIdx{ 0 };
			if (imgui::BeginSection_1Col(countIdx, "Wave List", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x * 0.8f,400 }))
				{
					int countIdx{ 0 };
					for (auto each : WaveData::GetInstances())
					{
						countIdx++;
						stringstream ss;
						ss << yutility::GetString(each->pod.name).c_str() << " ##WaveSelectable" << countIdx;
						if (ImGui::Selectable(ss.str().c_str(), selectedWave == each))
						{
							selectedWave = each;
						}
						if (selectedWave == each)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndListBox();
				}
				int countIdx = 0;
				imgui::EndSection();
			}
			if (selectedWave)
			{
				if (imgui::BeginSection_2Col(countIdx, "Wave info", ImGui::GetContentRegionAvail().x))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Wave name");
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					string charBuffer = yutility::GetString(selectedWave->pod.name);
					charBuffer.reserve(64);
					ImGui::InputText("##RegionNameInputText", &charBuffer[0], 64);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Wave combo objective 1");
					ImGui::TableSetColumnIndex(1);
					ImGui::InputInt("##WaveComboObjective1", &selectedWave->pod.comboObjective1);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Wave combo objective 2");
					ImGui::TableSetColumnIndex(1);
					ImGui::InputInt("##WaveComboObjective2", &selectedWave->pod.comboObjective2);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Wave combo objective 3");
					ImGui::TableSetColumnIndex(1);
					ImGui::InputInt("##WaveComboObjective3", &selectedWave->pod.comboObjective3);

					static bool beforeFocus = false;
					if (ImGui::IsItemFocused())
					{
						beforeFocus = true;
						EditorLayer::SetInputControl(false);
						ec.SetInputUpdate(false);
					}
					else if (beforeFocus)
					{
						beforeFocus = false;
						EditorLayer::SetInputControl(true);
						ec.SetInputUpdate(true);
					}

					selectedWave->pod.name = yutility::GetWString(charBuffer);

					imgui::EndSection();
				}
				if (imgui::BeginSection_1Col(countIdx, "Region info", ImGui::GetContentRegionAvail().x))
				{
					auto triggerRegion{ selectedWave->pod.triggerRegion };
					auto constraintRegion{ selectedWave->pod.contraintRegion };

					/// 접근시 웨이브를 촉발시키는 지역을 지정하는 부분
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Triggering region");
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (!triggerRegion)
						ImGui::BeginDisabled();
					// 지역 이름을 클릭하면 카메라가 지역의 위치로 이동
					if (ImGui::Button(triggerRegion ? yutility::GetString(triggerRegion->pod.name.c_str()).c_str() : "Region not selected"));
					if (!triggerRegion)
						ImGui::EndDisabled();
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					auto currentPaletteState = palette::WavePalette::SingleInstance().GetWavePaletteState();
					if (currentPaletteState == palette::WavePalette::WavePaletteState::SelectingTriggerRegion)
						ImGui::BeginDisabled();
					if (ImGui::Button("Select region##Trigger"))
						palette::WavePalette::SingleInstance().SetWavePaletteState(palette::WavePalette::WavePaletteState::SelectingTriggerRegion);
					if (currentPaletteState == palette::WavePalette::WavePaletteState::SelectingTriggerRegion)
						ImGui::EndDisabled();

					/// 웨이브 진행 중 카메라의 이동을 제한하는 지역을 지정하는 부분
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Constraint region");
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (!constraintRegion)
						ImGui::BeginDisabled();
					// 지역 이름을 클릭하면 카메라가 지역의 위치로 이동
					if (ImGui::Button(constraintRegion ? yutility::GetString(constraintRegion->pod.name.c_str()).c_str() : "Region not selected"));
					if (!constraintRegion)
						ImGui::EndDisabled();

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					if (currentPaletteState == palette::WavePalette::WavePaletteState::SelectingConstraintRegion)
						ImGui::BeginDisabled();
					if (ImGui::Button("Select region##Constraint"))
						palette::WavePalette::SingleInstance().SetWavePaletteState(palette::WavePalette::WavePaletteState::SelectingConstraintRegion);
					if (currentPaletteState == palette::WavePalette::WavePaletteState::SelectingConstraintRegion)
						ImGui::EndDisabled();

					imgui::EndSection();
				}
				if (imgui::BeginSection_2Col(countIdx, "each wave info", ImGui::GetContentRegionAvail().x))
				{
					for (int i = 1; i <= selectedWave->pod.waveSizes.size(); i++)
					{
						stringstream ss;
						ss << "Edit wave " << i;
						stringstream ssDelete;
						ssDelete << "X##" << i;
						stringstream ssUnitNum;
						ssUnitNum << "Units count : " << selectedWave->pod.waveSizes[i - 1];
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						// n차 웨이브를 수정하는 부분
						if (i - 1 == palette::WavePalette::SingleInstance().currentSelectedWaveIndex)
							ImGui::BeginDisabled();
						if (ImGui::Button(ss.str().c_str()))
						{
							palette::WavePalette::SingleInstance().UpdateWaveUnitsVisibility();
							palette::WavePalette::SingleInstance().currentSelectedWaveIndex = i - 1;
						}
						else
						{
							if (i - 1 == palette::WavePalette::SingleInstance().currentSelectedWaveIndex)
								ImGui::EndDisabled();
						}
						ImGui::Text(ssUnitNum.str().c_str());
						ImGui::TableNextColumn();
						// n차 웨이브를 삭제하는 부분
						if (ImGui::Button(ssDelete.str().c_str()))
						{
							for (auto& each : selectedWave->GetWaveUnitDataMap())
								if (each.second.waveIdx == i - 1)
									editor::InstanceManager::GetSingletonInstance().DeleteInstance(each.first->GetUUID());
							std::erase_if(selectedWave->GetWaveUnitDataMap(), [i](std::pair<UnitData*, WaveData::WaveUnitData> each) {return each.second.waveIdx == i - 1; });
							for (auto& each : selectedWave->GetWaveUnitDataMap())
								if (each.second.waveIdx > i - 1)
									each.second.waveIdx--;
							selectedWave->pod.waveSizes.resize(selectedWave->pod.waveSizes.size() - 1);
							selectedWave->ApplyMapAsPod();
							palette::WavePalette::SingleInstance().currentSelectedWaveIndex = -1;
						}
					}
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					stringstream ss;
					ss << "Add wave " << selectedWave->pod.waveSizes.size() + 1;
					// 새로이 웨이브를 추가하는 부분
					if (ImGui::Button(ss.str().c_str()))
						selectedWave->pod.waveSizes.push_back(0);

					imgui::EndSection();
				}
			}
		}

		void PalettePanel::ImGui_BeginLightPalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

			int countIdx = 0;
			if (imgui::BeginSection_1Col(countIdx, "Directional Light", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				if (lightGizmo)
				{
					static bool isEditing = false;
					static TransformData td;
					bool endEdit = false;
					auto tg = lightGizmo->GetTransform();
					Quaternion rotationQ = { directionalLight->pod.rotation.w, directionalLight->pod.rotation.x, directionalLight->pod.rotation.y, directionalLight->pod.rotation.z };
					yunutyEngine::Vector3f rotation = rotationQ.Euler();
					auto resetRotation = imgui::Vector3Control("Rotation", rotation.x, rotation.y, rotation.z);
					switch (resetRotation)
					{
						case application::editor::imgui::Vector3Flags::ResetX:
						{
							rotation.x = 0;
							endEdit = true;
							break;
						}
						case application::editor::imgui::Vector3Flags::ResetY:
						{
							rotation.y = 0;
							endEdit = true;
							break;
						}
						case application::editor::imgui::Vector3Flags::ResetZ:
						{
							rotation.z = 0;
							endEdit = true;
							break;
						}
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

					auto obj = directionalLight->GetPaletteInstance()->GetGameObject();
					if (isEditing == false &&
						(rotation != Vector3f(rotationQ.Euler())))
					{
						td = obj->GetTransform();
						isEditing = true;
					}

					rotationQ = Quaternion(rotation);
					directionalLight->pod.rotation.w = rotationQ.w;
					directionalLight->pod.rotation.x = rotationQ.x;
					directionalLight->pod.rotation.y = rotationQ.y;
					directionalLight->pod.rotation.z = rotationQ.z;
					directionalLight->ApplyAsPaletteInstance();
					tg->SetLocalRotation(Quaternion(rotation));

					if (endEdit)
					{
						CommandManager::GetSingletonInstance().AddQueue(std::make_shared<TransformEditCommand>(std::vector<std::tuple<IEditableData*, TransformData, TransformData>>(1, { directionalLight, td, obj->GetTransform() })));
						isEditing = false;
					}
				}

				imgui::EndSection();
			}

			if (imgui::BeginSection_1Col(countIdx, "Create Light", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				/// Directional Light 는 하나의 인스턴스로부터 컨트롤할 수 있도록 변경
				//if (ImGui::Button("Directional Light", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
				//{
				//	if (lp.IsSelectMode())
				//	{
				//		lightCurrentButton = (int)LightType::Directional;
				//		lp.SelectLightTemplateData(static_cast<Light_TemplateData*>(tdm.GetTemplateData("Directional_Light")));
				//		lp.SetAsSelectMode(false);
				//	}
				//	else
				//	{
				//		if (lightCurrentButton == (int)LightType::Directional)
				//		{
				//			lp.SelectLightTemplateData(nullptr);
				//			lp.SetAsSelectMode(true);
				//		}
				//		else
				//		{
				//			lightCurrentButton = (int)LightType::Directional;
				//			lp.SelectLightTemplateData(static_cast<Light_TemplateData*>(tdm.GetTemplateData("Directional_Light")));
				//			lp.SetAsSelectMode(false);
				//		}
				//	}
				//}

				if (ImGui::Button("Point Light", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
				{
					if (lp.IsSelectMode())
					{
						lightCurrentButton = (int)LightType::Point;
						lp.SelectLightTemplateData(static_cast<Light_TemplateData*>(tdm.GetTemplateData("Point_Light")));
						lp.SetAsSelectMode(false);
					}
					else
					{
						if (lightCurrentButton == (int)LightType::Point)
						{
							lp.SelectLightTemplateData(nullptr);
							lp.SetAsSelectMode(true);
						}
						else
						{
							lightCurrentButton = (int)LightType::Point;
							lp.SelectLightTemplateData(static_cast<Light_TemplateData*>(tdm.GetTemplateData("Point_Light")));
							lp.SetAsSelectMode(false);
						}
					}
				}
				imgui::EndSection();
			}

			if (imgui::BeginSection_1Col(countIdx, "Point Light List", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x * 0.8f,400 }))
				{
					int idx = 0;
					for (auto& each : LightData::GetInstances())
					{
						if (each->pod.templateData->pod.type == LightType::Directional)
						{
							continue;
						}

						ImGui::PushID(("Light Name" + each->pod.name).c_str() + idx);
						if (ImGui::Selectable(each->pod.name.c_str(), palette::LightPalette::SingleInstance().GetSingleSelectedLight() == each))
						{
							palette::LightPalette::SingleInstance().SelectLight(each);
						}
						if (palette::LightPalette::SingleInstance().GetSingleSelectedLight() == each)
						{
							ImGui::SetItemDefaultFocus();
						}
						ImGui::PopID();
						idx++;
					}
					ImGui::EndListBox();
				}
				imgui::EndSection();
			}

			if (imgui::BeginSection_2Col(countIdx, "Selected Point Light", ImGui::GetContentRegionAvail().x, 0.2f))
			{
				const auto& selection = pm.GetCurrentPalette()->GetSelections();

				if (selection.size() == 1)
				{
					auto pl = static_cast<LightData*>(*selection.begin());

					{
						std::string plName = pl->pod.name;
						plName.reserve(32);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
						ImGui::AlignTextToFramePadding();
						ImGui::Text("Light Name");
						ImGui::TableSetColumnIndex(1);
						ImGui::SetNextItemWidth(-1);
						ImGui::InputText("##Light_Name", &plName[0], 32);

						static bool beforeFocus = false;
						if (ImGui::IsItemFocused())
						{
							beforeFocus = true;
							EditorLayer::SetInputControl(false);
							ec.SetInputUpdate(false);
						}
						else if (beforeFocus)
						{
							beforeFocus = false;
							EditorLayer::SetInputControl(true);
							ec.SetInputUpdate(true);
						}

						wanderUtils::UpdateStringSize(plName);
						pl->pod.name = plName;
					}

					imgui::DragFloat_2Col("Intensity", pl->pod.intensity, true, 0.1f, 0.0f, 100.0f);
					imgui::DragFloat_2Col("Range", pl->pod.range, true, 0.1f, 0.0f, 100.0f);
					imgui::Checkbox_2Col("Is Cast", pl->pod.isCast);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Color");
					ImGui::TableSetColumnIndex(1);
					ImGui::SetNextItemWidth(-1);
					ImGui::ColorPicker4("Light Color", (float*)&pl->pod.color);
					pl->ApplyAsPaletteInstance();
				}

				imgui::EndSection();
			}
		}

		void PalettePanel::ImGui_BeginCameraPalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

			auto type = EditorCamera::GetSingletonInstance().GetCameraTypeState();

			int countIdx = 0;
			if (imgui::BeginSection_1Col(countIdx, "Create Camera", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Button("Create Here", ImVec2(ImGui::GetContentRegionAvail().x, 0)) && type == CameraTypeState::Editor)
				{
					auto cam = InstanceManager::GetSingletonInstance().CreateInstance<CameraData>("DefaultCamera");
					auto& ecam = EditorCamera::GetSingletonInstance();
					auto pos = ecam.GetPosition();
					auto rot = ecam.GetOrientation();
					auto scal = ecam.GetScale();
					cam->pod.position.x = pos.x;
					cam->pod.position.y = pos.y;
					cam->pod.position.z = pos.z;
					cam->pod.rotation.w = rot.w;
					cam->pod.rotation.x = rot.x;
					cam->pod.rotation.y = rot.y;
					cam->pod.rotation.z = rot.z;
					cam->pod.scale.x = scal.x;
					cam->pod.scale.y = scal.y;
					cam->pod.scale.z = scal.z;
					cam->ApplyAsPaletteInstance();
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Button(" + ", ImVec2(ImGui::GetContentRegionAvail().x, 0)) && type == CameraTypeState::Editor)
				{
					cp.SetAsSelectMode(!cp.IsSelectMode());
				}
				imgui::EndSection();
			}

			if (imgui::BeginSection_1Col(countIdx, "Camera List", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x * 0.8f,400 }))
				{
					int idx = 0;
					for (auto& each : CameraData::GetInstances())
					{
						ImGui::PushID(("Camera Name" + each->pod.name).c_str() + idx);
						if (ImGui::Selectable(each->pod.name.c_str(), palette::CameraPalette::SingleInstance().GetSingleSelectedCamera() == each))
						{
							palette::CameraPalette::SingleInstance().SelectCamera(each);
						}
						if (palette::CameraPalette::SingleInstance().GetSingleSelectedCamera() == each)
						{
							ImGui::SetItemDefaultFocus();
						}
						ImGui::PopID();
						idx++;
					}
					ImGui::EndListBox();
				}
				imgui::EndSection();
			}

			if (imgui::BeginSection_2Col(countIdx, "Selected Camera", ImGui::GetContentRegionAvail().x, 0.2f))
			{
				const auto& selection = pm.GetCurrentPalette()->GetSelections();
				CameraData* cam = nullptr;

				if (type == CameraTypeState::Game)
				{
					cam = CameraManager::GetSingletonInstance().GetMainCam();
				}
				else if (selection.size() == 1)
				{
					cam = static_cast<CameraData*>(*selection.begin());
				}

				if (cam)
				{
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
						ImGui::AlignTextToFramePadding();
						ImGui::Text("Transform");
						ImGui::TableSetColumnIndex(1);
						ImGui::SetNextItemWidth(-1);
						if (ImGui::Button("Edit Here"))
						{
							auto& ecam = EditorCamera::GetSingletonInstance();
							auto pos = ecam.GetPosition();
							auto rot = ecam.GetOrientation();
							auto scal = ecam.GetScale();
							cam->pod.position.x = pos.x;
							cam->pod.position.y = pos.y;
							cam->pod.position.z = pos.z;
							cam->pod.rotation.w = rot.w;
							cam->pod.rotation.x = rot.x;
							cam->pod.rotation.y = rot.y;
							cam->pod.rotation.z = rot.z;
							cam->pod.scale.x = scal.x;
							cam->pod.scale.y = scal.y;
							cam->pod.scale.z = scal.z;
							cam->ApplyAsPaletteInstance();
						}
					}

					{
						std::string camName = cam->pod.name;
						camName.reserve(32);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
						ImGui::AlignTextToFramePadding();
						ImGui::Text("Cam Name");
						ImGui::TableSetColumnIndex(1);
						ImGui::SetNextItemWidth(-1);
						ImGui::InputText("##Camera_Name", &camName[0], 32);

						static bool beforeFocus = false;
						if (ImGui::IsItemFocused())
						{
							beforeFocus = true;
							EditorLayer::SetInputControl(false);
							ec.SetInputUpdate(false);
						}
						else if (beforeFocus)
						{
							beforeFocus = false;
							EditorLayer::SetInputControl(true);
							ec.SetInputUpdate(true);
						}
						wanderUtils::UpdateStringSize(camName);
						cam->pod.name = camName;
					}

					imgui::DragFloat_2Col("Vertical FOV", cam->pod.vertical_FOV, true, 0.1f, 0.0f, 100.0f);
					imgui::DragFloat_2Col("Near", cam->pod.dis_Near, true, 0.01f, 0.01f, 100.0f);
					imgui::DragFloat_2Col("Far", cam->pod.dis_Far, true, 0.01f, 500.0f, 1500.0f);
					imgui::DragFloat_2Col("Width", cam->pod.res_Width, true, 10.0f, 100.0f, 2000.0f);
					imgui::DragFloat_2Col("Height", cam->pod.res_Height, true, 10.0f, 100.0f, 2000.0f);
					imgui::Checkbox_2Col("Main Cam", cam->pod.isMain);

					if (CameraManager::GetSingletonInstance().GetMainCam() == cam)
					{
						if (!cam->pod.isMain)
						{
							cam->pod.isMain = true;
						}
						cam->ApplyAsPaletteInstance();
					}
					else
					{
						if (cam->pod.isMain)
						{
							CameraManager::GetSingletonInstance().GetMainCam()->pod.isMain = false;
							CameraManager::GetSingletonInstance().SetMainCam(cam);
							cam->ApplyAsPaletteInstance();
							EditorCamera::GetSingletonInstance().ReloadGameCamera();
						}
					}

					EditorCamera::GetSingletonInstance().UpdateGI();
				}

				imgui::EndSection();
			}
		}

		void PalettePanel::ImGui_BeginParticlePalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

			imgui::ShiftCursorY(5);
			if (ImGui::Button("Create World Particle", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
			{
				ImGui_CreateParticlePopup();
			}

			int countIdx = 0;

			imgui::ShiftCursorY(5);
			if (imgui::BeginSection_1Col(countIdx, "World Particle List", ImGui::GetContentRegionAvail().x))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x * 0.8f,400 }))
				{
					int idx = 0;
					for (auto& each : ParticleData::GetInstances())
					{
						ImGui::PushID(("Particle Name" + each->pod.name).c_str() + idx);
						if (ImGui::Selectable(each->pod.name.c_str(), palette::ParticlePalette::SingleInstance().GetSingleSelectedParticle() == each))
						{
							palette::ParticlePalette::SingleInstance().SelectParticle(each);
						}
						if (palette::ParticlePalette::SingleInstance().GetSingleSelectedParticle() == each)
						{
							ImGui::SetItemDefaultFocus();
						}
						ImGui::PopID();
						idx++;
					}
					ImGui::EndListBox();
				}
				imgui::EndSection();
			}

			if (imgui::BeginSection_2Col(countIdx, "Selected Particle", ImGui::GetContentRegionAvail().x, 0.2f))
			{
				const auto& selection = pm.GetCurrentPalette()->GetSelections();

				ParticleData* particle = nullptr;

				if (selection.size() == 1)
				{
					particle = static_cast<ParticleData*>(*selection.begin());
				}

				if (particle)
				{
					{
						std::string particleName = particle->pod.name;
						particleName.reserve(32);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
						ImGui::AlignTextToFramePadding();
						ImGui::Text("Name");
						ImGui::TableSetColumnIndex(1);
						ImGui::SetNextItemWidth(-1);
						ImGui::InputText("##Particle_Name", &particleName[0], 32);

						static bool beforeFocus = false;
						if (ImGui::IsItemFocused())
						{
							beforeFocus = true;
							EditorLayer::SetInputControl(false);
							ec.SetInputUpdate(false);
						}
						else if (beforeFocus)
						{
							beforeFocus = false;
							EditorLayer::SetInputControl(true);
							ec.SetInputUpdate(true);
						}
						wanderUtils::UpdateStringSize(particleName);
						particle->pod.name = particleName;
					}

					/// Particle 의 원형을 바꿀 경우(TemplateData 대체)
					static auto& ptm = particle::ParticleTool_Manager::GetSingletonInstance();
					{
						static std::vector<const char*> selections = std::vector<const char*>();

						selections.resize(0);
						selections.push_back("Default");
						for (auto& each : ptm.GetParticleList())
						{
							selections.push_back(each.lock()->name.c_str());
						}

						int selectedParticle = 0;
						for (auto& each : selections)
						{
							if (each == particle->pod.particleData.name)
							{
								break;
							}
							selectedParticle++;
						}

						if (selectedParticle == selections.size())
						{
							selectedParticle = 0;
						}

						if (imgui::Dropdown_2Col("Mold", &selections[0], selections.size(), &selectedParticle))
						{
							if (selectedParticle == 0)
							{
								particle->pod.particleData = particle::ParticleToolData();
							}
							else
							{
								for (auto& ptr : ptm.GetParticleList())
								{
									if (selections[selectedParticle] == ptr.lock()->name)
									{
										particle->pod.particleData = *ptr.lock();
										break;
									}
								}
							}
						}
					}

					static std::vector<const char*> textures = std::vector<const char*>();

					textures.resize(0);
					for (auto& each : ptm.GetTexturePathList())
					{
						textures.push_back(each.c_str());
					}

					int selectedTexture = 0;
					for (auto& each : textures)
					{
						if (each == particle->pod.particleData.texturePath)
						{
							break;
						}
						selectedTexture++;
					}

					if (imgui::Dropdown_2Col("Texture", &textures[0], textures.size(), &selectedTexture))
					{
						particle->pod.particleData.texturePath = textures[selectedTexture];
					}

					static const char* shapeList[2] = { "Cone", "Circle" };
					int selectedShape = (int)particle->pod.particleData.shape;
					if (imgui::Dropdown_2Col("Shape", shapeList, 2, &selectedShape))
					{
						particle->pod.particleData.shape = (application::particle::ParticleShape)selectedShape;
					}

					static const char* modeList[2] = { "Default", "Bursts" };
					int selectedMode = (int)particle->pod.particleData.particleMode;
					if (imgui::Dropdown_2Col("Mode", modeList, 2, &selectedMode))
					{
						particle->pod.particleData.particleMode = (application::particle::ParticleMode)selectedMode;
					}

					imgui::Checkbox_2Col("Loop", particle->pod.particleData.isLoop);
					imgui::DragFloat_2Col("Duration", particle->pod.particleData.duration);
					imgui::DragFloat_2Col("Life Time", particle->pod.particleData.lifeTime);
					imgui::DragFloat_2Col("Speed", particle->pod.particleData.speed);
					imgui::DragFloat_2Col("Start Scale", particle->pod.particleData.startScale);
					imgui::DragFloat_2Col("End Scale", particle->pod.particleData.endScale);

					int maxParticle = particle->pod.particleData.maxParticle;
					if (imgui::DragInt_2Col("Max Particle", maxParticle, true, 1.f, 1, 500))
					{
						particle->pod.particleData.maxParticle = maxParticle;
					}

					imgui::Checkbox_2Col("Play Awake", particle->pod.particleData.playAwake);

					switch (particle->pod.particleData.shape)
					{
						case application::particle::ParticleShape::Cone:
						{
							imgui::DragFloat_2Col("Radius", particle->pod.particleData.radius);
							imgui::DragFloat_2Col("Angle", particle->pod.particleData.angle);
							break;
						}
						default:
							break;
					}

					switch (particle->pod.particleData.particleMode)
					{
						case application::particle::ParticleMode::Default:
						{
							imgui::DragFloat_2Col("Rate OverTime", particle->pod.particleData.rateOverTime);
							break;
						}
						case application::particle::ParticleMode::Bursts:
						{
							imgui::DragInt_2Col("Bursts Count", particle->pod.particleData.burstsCount);
							imgui::DragFloat_2Col("Interval", particle->pod.particleData.interval);
							break;
						}
						default:
							break;
					}
				}

				imgui::EndSection();
			}
		}

		void PalettePanel::ImGui_BeginInteractablePalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

			int countIdx = 0;

			if (imgui::BeginSection_1Col(countIdx, "Interactable List", ImGui::GetContentRegionAvail().x))
			{
				auto uSize = tdm.GetDataList(DataType::InteractableData).size();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				for (int i = 0; i < uSize + 1; i++)
				{
					imgui::SmartStyleVar textAlign(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
					if (i == uSize)
					{
						if (ImGui::Selectable("+", false))
						{
							if (interactableCurrentButton != -1)
							{
								interactableCurrentButton = -1;
								ip.SelectInteractableTemplateData(nullptr);
								ip.SetAsSelectMode(true);
							}
							ImGui_CreateInteractablePopup();
							EditorLayer::SetInputControl(false);
						}
					}
					else
					{
						if (ImGui::Selectable(static_cast<Interactable_TemplateData*>(tdm.GetDataList(DataType::InteractableData)[i])->GetDataKey().c_str(), interactableCurrentButton == i))
						{
							if (interactableCurrentButton != -1)
							{
								if (interactableCurrentButton == i)
								{
									interactableCurrentButton = -1;
									ip.SelectInteractableTemplateData(nullptr);
									ip.SetAsSelectMode(true);
								}
								else
								{
									interactableCurrentButton = i;
									ip.SelectInteractableTemplateData(static_cast<Interactable_TemplateData*>(tdm.GetDataList(DataType::InteractableData)[i]));
									ip.SetAsSelectMode(false);
								}
							}
							else
							{
								interactableCurrentButton = i;
								ip.SelectInteractableTemplateData(static_cast<Interactable_TemplateData*>(tdm.GetDataList(DataType::InteractableData)[i]));
								ip.SetAsSelectMode(false);
							}
						}
					}
				}
				imgui::EndSection();
			}
		}

		bool PalettePanel::ImGui_CreateUnitPopup()
		{
			bool returnVal = false;
			imgui::ShowMessageBox("Create Unit", [this, &returnVal]()
				{
					imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::SetNextItemWidth(-1);
					ImGui::InputTextWithHint("##new_unit_name", "Unit Name", unitNameBuffer, bufferSize);

					ImGui::Separator();

					static std::vector<std::string> selections = std::vector<std::string>();
					static std::string fbxName = "None";
					std::string currentFBX = fbxName;

					selections.resize(0);
					for (auto& each : ResourceManager::GetSingletonInstance().GetSkinnedFBXList())
					{
						selections.push_back(each);
					}

					ImGui::SetNextItemWidth(-1);
					if (ImGui::BeginCombo("##fbxUnitCombo", fbxName.c_str()))
					{
						for (int i = 0; i < selections.size(); i++)
						{
							const bool is_selected = (currentFBX == selections[i]);
							if (ImGui::Selectable(selections[i].c_str(), is_selected))
							{
								currentFBX = selections[i];
								fbxName = currentFBX;
							}

							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("Create"))
					{
						if (*unitNameBuffer != '\0' && fbxName != "None")
						{
							auto keyName = std::string(unitNameBuffer);
							if (tdm.CreateTemplateData<Unit_TemplateData>(keyName) != nullptr)
							{
								auto tud = tdm.GetTemplateData(keyName);
								tud->SetDataResourceName(fbxName);
								palette::UnitBrush::Instance().CreateBrush(keyName);

								unitCurrentButton = tdm.GetDataList(DataType::UnitData).size() - 1;
								up.SelectUnitTemplateData(static_cast<Unit_TemplateData*>(tud));
								up.SetAsSelectMode(false);

								memset(unitNameBuffer, 0, bufferSize);
								fbxName = "None";
								returnVal = true;
								ImGui::CloseCurrentPopup();
								imgui::CloseMessageBox("Create Unit");
								EditorLayer::SetInputControl(true);
							}
							else
							{
								memset(unitNameBuffer, 0, bufferSize);
							}
						}
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						memset(unitNameBuffer, 0, bufferSize);
						fbxName = "None";
						ImGui::CloseCurrentPopup();
						imgui::CloseMessageBox("Create Unit");
						EditorLayer::SetInputControl(true);
					}
				}, 600);
			return returnVal;
		}

		bool PalettePanel::ImGui_CreateParticlePopup()
		{
			EditorLayer::SetInputControl(false);
			bool returnVal = false;
			imgui::ShowMessageBox("Create World Particle", [this, &returnVal]()
				{
					imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					/// 등록된 Particle 에서 가져오기
					static auto& ptm = particle::ParticleTool_Manager::GetSingletonInstance();
					static std::vector<std::string> selections = std::vector<std::string>();
					static std::string particleName = "Default";
					std::string currentParticle = particleName;

					selections.resize(0);
					for (auto& each : ptm.GetParticleList())
					{
						selections.push_back(each.lock()->name);
					}

					ImGui::SetNextItemWidth(-1);
					if (ImGui::BeginCombo("##particleCombo", particleName.c_str()))
					{
						for (int i = 0; i < selections.size(); i++)
						{
							const bool is_selected = (currentParticle == selections[i]);
							if (ImGui::Selectable(selections[i].c_str(), is_selected))
							{
								currentParticle = selections[i];
								particleName = currentParticle;
							}

							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("Create"))
					{
						if (particleName == "Default")
						{
							pp.InitParticleData();
						}
						else
						{
							for (auto& pi : ptm.GetParticleList())
							{
								if (particleName == pi.lock()->name)
								{
									pp.SetParticleData(*pi.lock());
									break;
								}
							}
						}
						pp.SetAsSelectMode(false);

						returnVal = true;
						particleName = "Default";
						ImGui::CloseCurrentPopup();
						imgui::CloseMessageBox("Create World Particle");
						EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						particleName = "Default";
						ImGui::CloseCurrentPopup();
						imgui::CloseMessageBox("Create World Particle");
						EditorLayer::SetInputControl(true);
					}
				}, 600);
			return returnVal;
		}

		bool PalettePanel::ImGui_CreateInteractablePopup()
		{
			bool returnVal = false;
			imgui::ShowMessageBox("Create Interactable", [this, &returnVal]()
				{
					imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::SetNextItemWidth(-1);
					ImGui::InputTextWithHint("##new_interactable_name", "Interactable Name", interatableNameBuffer, bufferSize);

					ImGui::Separator();

					static std::vector<std::string> selections = std::vector<std::string>();
					static std::string fbxName = "None";
					std::string currentFBX = fbxName;

					selections.resize(0);
					for (auto& each : ResourceManager::GetSingletonInstance().GetStaticFBXList())
					{
						selections.push_back(each);
					}
					for (auto& each : ResourceManager::GetSingletonInstance().GetSkinnedFBXList())
					{
						selections.push_back(each);
					}

					std::sort(selections.begin(), selections.end());

					ImGui::SetNextItemWidth(-1);
					if (ImGui::BeginCombo("##fbxInteractableCombo", fbxName.c_str()))
					{
						for (int i = 0; i < selections.size(); i++)
						{
							const bool is_selected = (currentFBX == selections[i]);
							if (ImGui::Selectable(selections[i].c_str(), is_selected))
							{
								currentFBX = selections[i];
								fbxName = currentFBX;
							}

							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("Create"))
					{
						if (*interatableNameBuffer != '\0' && fbxName != "None")
						{
							auto keyName = std::string(interatableNameBuffer);
							if (tdm.CreateTemplateData<Interactable_TemplateData>(keyName) != nullptr)
							{
								auto tid = tdm.GetTemplateData(keyName);
								tid->SetDataResourceName(fbxName);
								palette::InteractableBrush::Instance().CreateBrush(keyName);

								interactableCurrentButton = tdm.GetDataList(DataType::InteractableData).size() - 1;
								ip.SelectInteractableTemplateData(static_cast<Interactable_TemplateData*>(tid));
								ip.SetAsSelectMode(false);

								memset(interatableNameBuffer, 0, bufferSize);
								fbxName = "None";
								returnVal = true;
								ImGui::CloseCurrentPopup();
								imgui::CloseMessageBox("Create Interactable");
								EditorLayer::SetInputControl(true);
							}
							else
							{
								memset(interatableNameBuffer, 0, bufferSize);
							}
						}
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						memset(interatableNameBuffer, 0, bufferSize);
						fbxName = "None";
						ImGui::CloseCurrentPopup();
						imgui::CloseMessageBox("Create Interactable");
						EditorLayer::SetInputControl(true);
					}
				}, 600);
			return returnVal;
		}

		void PalettePanel::LoadCallback()
		{
			unitCurrentButton = -1;
			ornamentCurrentButton = -1;
			lightCurrentButton = -1;
			interactableCurrentButton = -1;

			tp.SetAsSelectMode(true);
			up.SetAsSelectMode(true);
			op.SetAsSelectMode(true);
			rp.SetAsSelectMode(true);
			wp.SetAsSelectMode(true);
			cp.SetAsSelectMode(true);
			lp.SetAsSelectMode(true);
			pp.SetAsSelectMode(true);
			ip.SetAsSelectMode(true);
		}

		void PalettePanel::UpdataLightGizmo()
		{
			Quaternion quat = { directionalLight->pod.rotation.w, directionalLight->pod.rotation.x, directionalLight->pod.rotation.y, directionalLight->pod.rotation.z };
			lightGizmo->GetTransform()->SetLocalRotation(quat);
		}
	}
}
