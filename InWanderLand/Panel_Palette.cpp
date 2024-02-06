#include "InWanderLand.h"
#include "Panel_Palette.h"
#include "Application.h"
#include "imgui_Utility.h"
#include "EditorCommonEvents.h"

#include "YunutyEngine.h"

#include "imgui.h"

namespace application
{
	namespace editor
	{
		PalettePanel::~PalettePanel()
		{

		}

		void PalettePanel::Initialize()
		{
			currentPalette = pm.GetCurrentPalette();

			auto uSize = tdm.GetDataList(DataType::UnitData).size();
			unitButton.reserve(30);
			for (int i = 0; i < uSize; i++)
			{
				unitButton.push_back(false);
			}

			auto oSize = tdm.GetDataList(DataType::OrnamentData).size();
			ornamentButton.reserve(30);
			for (int i = 0; i < oSize; i++)
			{
				ornamentButton.push_back(false);
			}
		}

		void PalettePanel::Update(float ts)
		{

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

		PalettePanel::PalettePanel()
		{
			unitButton.reserve(20);
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

			if (!Application::IsFocusGameWindow())
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
							if (unitCurrentButton != -1)
							{
								unitButton[unitCurrentButton] = false;
								unitCurrentButton = -1;
							}

							up.SelectUnitTemplateData(nullptr);
						}
					}
				}

				if (eim.IsKeyboardUp(KeyCode::Delete))
				{
					if (currentPalette && currentPalette->IsSelectMode())
					{
						currentPalette->OnDeletion();
					}
				}

				if (eim.IsKeyboardUp(KeyCode::B))
				{
					if (currentPalette == &tp)
					{
						tp.ApplyAsPlaytimeObjects();
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

			if (imgui::BeginSection_1Col(countIdx, "Unit List", ImGui::GetContentRegionAvail().x))
			{
				auto uSize = tdm.GetDataList(DataType::UnitData).size();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				int colCount = 6;
				int rowCount = 1;

				auto style = ImGui::GetStyle();
				auto imageSize = ImGui::GetContentRegionAvail().x / colCount - style.ItemSpacing.x - style.FramePadding.x * 2;

				for (int i = 0; i < uSize + 1; i++)
				{
					if (i != 0 && i % colCount == 0)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
					}

					if (i == uSize)
					{
						bool buttonFlag = imgui::SelectableImageButton("Unit Add Button", "ImageButtons/Unit_AddButton.png", false, ImVec2(imageSize, imageSize));
						imgui::SetTooltip("Add");
						if (buttonFlag)
						{
							if (unitCurrentButton != -1)
							{
								unitButton[unitCurrentButton] = false;
								unitCurrentButton = -1;
								up.SelectUnitTemplateData(nullptr);
								up.SetAsSelectMode(true);
							}

							// Unit Template 추가 로직과 연결해야 함
							// 이때, unitButton 에도 pushback 해주어 Size를 추가해야 함
							/// 임시로 Unit Template Data 하나를 추가하는 로직을 구현함
							auto td = tdm.CreateTemplateData<Unit_TemplateData>("UnitButton" + std::to_string(i));
							unitButton.push_back(false);
						}
					}
					else
					{
						bool ref = unitButton[i];
						bool buttonFlag = imgui::SelectableImageButton("UnitButton" + std::to_string(i), static_cast<Unit_TemplateData*>(tdm.GetDataList(DataType::UnitData)[i])->pod.thumbnailPath, ref, ImVec2(imageSize, imageSize));
						imgui::SetTooltip(tdm.GetDataList(DataType::UnitData)[i]->GetDataKey());
						if (buttonFlag)
						{
							if (unitCurrentButton != -1)
							{
								unitButton[unitCurrentButton] = false;

								if (unitCurrentButton == i)
								{
									unitCurrentButton = -1;
									up.SelectUnitTemplateData(nullptr);
									up.SetAsSelectMode(true);
								}
								else
								{
									unitButton[i] = true;
									unitCurrentButton = i;
									up.SelectUnitTemplateData(static_cast<Unit_TemplateData*>(tdm.GetDataList(DataType::UnitData)[i]));
									up.SetAsSelectMode(false);
								}
							}
							else
							{
								unitButton[i] = true;
								unitCurrentButton = i;
								up.SelectUnitTemplateData(static_cast<Unit_TemplateData*>(tdm.GetDataList(DataType::UnitData)[i]));
								up.SetAsSelectMode(false);
							}
						}

						ImGui::SameLine();
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

			if (imgui::BeginSection_1Col(countIdx, "Ornament List", ImGui::GetContentRegionAvail().x))
			{
				auto oSize = tdm.GetDataList(DataType::OrnamentData).size();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				int colCount = 6;
				int rowCount = 1;

				auto style = ImGui::GetStyle();
				auto imageSize = ImGui::GetContentRegionAvail().x / colCount - style.ItemSpacing.x - style.FramePadding.x * 2;

				for (int i = 0; i < oSize + 1; i++)
				{
					if (i != 0 && i % colCount == 0)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
					}

					if (i == oSize)
					{
						bool buttonFlag = imgui::SelectableImageButton("Ornament Add Button", "ImageButtons/Ornament_AddButton.png", false, ImVec2(imageSize, imageSize));
						imgui::SetTooltip("Add");
						if (buttonFlag)
						{
							if (ornamentCurrentButton != -1)
							{
								ornamentButton[ornamentCurrentButton] = false;
								ornamentCurrentButton = -1;
								op.SelectOrnamentTemplateData(nullptr);
								op.SetAsSelectMode(true);
							}

							// Ornament Template 추가 로직과 연결해야 함
							// 이때, ornamentButton 에도 pushback 해주어 Size를 추가해야 함
							/// 임시로 Ornament Template Data 하나를 추가하는 로직을 구현함
							auto td = tdm.CreateTemplateData<Ornament_TemplateData>("OrnamentButton" + std::to_string(i));
							td->pod.fbxName = "SM_Temple_Books";
							ornamentButton.push_back(false);
						}
					}
					else
					{
						bool ref = ornamentButton[i];
						bool buttonFlag = imgui::SelectableImageButton("OrnamentButton" + std::to_string(i), static_cast<Ornament_TemplateData*>(tdm.GetDataList(DataType::OrnamentData)[i])->pod.thumbnailPath, ref, ImVec2(imageSize, imageSize));
						imgui::SetTooltip(tdm.GetDataList(DataType::OrnamentData)[i]->GetDataKey());
						if (buttonFlag)
						{
							if (ornamentCurrentButton != -1)
							{
								ornamentButton[ornamentCurrentButton] = false;

								if (ornamentCurrentButton == i)
								{
									ornamentCurrentButton = -1;
									op.SelectOrnamentTemplateData(nullptr);
									op.SetAsSelectMode(true);
								}
								else
								{
									ornamentButton[i] = true;
									ornamentCurrentButton = i;
									op.SelectOrnamentTemplateData(static_cast<Ornament_TemplateData*>(tdm.GetDataList(DataType::OrnamentData)[i]));
									op.SetAsSelectMode(false);
								}
							}
							else
							{
								ornamentButton[i] = true;
								ornamentCurrentButton = i;
								op.SelectOrnamentTemplateData(static_cast<Ornament_TemplateData*>(tdm.GetDataList(DataType::OrnamentData)[i]));
								op.SetAsSelectMode(false);
							}
						}

						ImGui::SameLine();
					}
				}
				imgui::EndSection();
			}
		}

		void PalettePanel::ImGui_BeginRegionPalette()
		{
			imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

			int countIdx = 0;
		}

		void PalettePanel::LoadCallback()
		{
			unitCurrentButton = -1;
			unitButton.clear();

			ornamentCurrentButton = -1;
			ornamentButton.clear();

			auto uSize = tdm.GetDataList(DataType::UnitData).size();
			unitButton.reserve(30);
			for (int i = 0; i < uSize; i++)
			{
				unitButton.push_back(false);
			}

			auto oSize = tdm.GetDataList(DataType::OrnamentData).size();
			ornamentButton.reserve(30);
			for (int i = 0; i < oSize; i++)
			{
				ornamentButton.push_back(false);
			}
		}
	}
}
