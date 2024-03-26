/// 2023. 10. 04 김상준
/// 선택한 특정 객체에 대하여 상세한 편집 내용을 확인할 수 있는 패널

#pragma once

#include "Singleton.h"
#include "EditorPanel.h"

#include "PaletteManager.h"
#include "EditorResourceManager.h"
#include "imgui_ModuleData.h"
#include "GCTemplate.h"
#include "PodStructs.h"
#include "imgui_Utility.h"

#include <nlohmann/json.hpp>
#include <boost/pfr.hpp>
#include <type_traits>

namespace application
{
	namespace editor
	{
		class InspectorPanel
			: public Panel, public Singleton<InspectorPanel>
		{
			friend class Singleton<InspectorPanel>;

		public:
			virtual ~InspectorPanel();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		private:
			InspectorPanel();

			void ImGui_DrawTransform(int& idx);
			void ImGui_DrawTemplateData(int& idx);
			void ImGui_DrawRestPOD(int& idx);

			template <typename T>
			bool DrawPOD(std::string label, T& data)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(label.c_str());
				ImGui::TableSetColumnIndex(1);
				return false;
			}

			/// 포인터는 넘어갑니다.
			template <typename T> requires std::is_pointer_v<std::remove_reference_t<T>>
			bool DrawPOD(std::string label, T& data)
			{
				return false;
			}

			/// transform 정보는 넘어갑니다.
			template <typename T>
			bool DrawPOD(std::string label, const POD_Vector3<T>& data)
			{
				return false;
			}

			/// transform 정보는 넘어갑니다.
			template <typename T>
			bool DrawPOD(std::string label, const POD_Quaternion<T>& data)
			{
				return false;
			}

			template <>
			bool DrawPOD(std::string label, const int& data)
			{
				return imgui::DragInt_2Col(label, const_cast<int&>(data));
			}

			template <>
			bool DrawPOD(std::string label, const GC<int>& data)
			{
				imgui::SmartStyleColor bgColor1(ImGuiCol_FrameBg, IM_COL32(255, 20, 20, 255));
				imgui::SmartStyleColor bgColor2(ImGuiCol_FrameBgHovered, IM_COL32(200, 20, 20, 255));
				imgui::SmartStyleColor bgColor3(ImGuiCol_FrameBgActive, IM_COL32(255, 20, 20, 255));
				int copyData = data.data;
				return imgui::DragInt_2Col(label, copyData, false);
			}

			template <>
			bool DrawPOD(std::string label, const float& data)
			{
				return imgui::DragFloat_2Col(label, const_cast<float&>(data));
			}

			template <>
			bool DrawPOD(std::string label, const GC<float>& data)
			{
				imgui::SmartStyleColor bgColor1(ImGuiCol_FrameBg, IM_COL32(255, 20, 20, 255));
				imgui::SmartStyleColor bgColor2(ImGuiCol_FrameBgHovered, IM_COL32(200, 20, 20, 255));
				imgui::SmartStyleColor bgColor3(ImGuiCol_FrameBgActive, IM_COL32(255, 20, 20, 255));
				float copyData = data.data;
				return imgui::DragFloat_2Col(label, copyData, false);
			}

			template <>
			bool DrawPOD(std::string label, const double& data)
			{
				float val = data;
				bool returnVal = imgui::DragFloat_2Col(label, val);
				const_cast<double&>(data) = val;
				return returnVal;
			}

			template <>
			bool DrawPOD(std::string label, const GC<double>& data)
			{
				float copyData = data.data;
				imgui::SmartStyleColor bgColor1(ImGuiCol_FrameBg, IM_COL32(255, 20, 20, 255));
				imgui::SmartStyleColor bgColor2(ImGuiCol_FrameBgHovered, IM_COL32(200, 20, 20, 255));
				imgui::SmartStyleColor bgColor3(ImGuiCol_FrameBgActive, IM_COL32(255, 20, 20, 255));
				return imgui::DragFloat_2Col(label, copyData, false);
			}

			template <>
			bool DrawPOD(std::string label, const bool& data)
			{
				return imgui::Checkbox_2Col(label, const_cast<bool&>(data));
			}

			template <>
			bool DrawPOD(std::string label, const GC<bool>& data)
			{
				imgui::SmartStyleColor bgColor1(ImGuiCol_FrameBg, IM_COL32(255, 20, 20, 255));
				imgui::SmartStyleColor bgColor2(ImGuiCol_FrameBgHovered, IM_COL32(200, 20, 20, 255));
				imgui::SmartStyleColor bgColor3(ImGuiCol_FrameBgActive, IM_COL32(255, 20, 20, 255));
				bool copyData = data.data;
				return imgui::Checkbox_2Col(label, copyData);
			}

			template <>
			bool DrawPOD(std::string label, const std::string& data)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(label.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::PushItemWidth(-1);

				imgui::SmartStyleColor textColor2(ImGuiCol_Text, IM_COL32_WHITE);
				std::string buffer = data;
				buffer.reserve(64);
				if (ImGui::InputText(("##" + label).c_str(), &buffer[0], 64))
				{	
					const_cast<std::string&>(data).assign(buffer.c_str());
					return true;
				}

				return false;
			}

			template <>
			bool DrawPOD(std::string label, const GC<std::string>& data)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(label.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::PushItemWidth(-1);

				imgui::SmartStyleColor textColor2(ImGuiCol_Text, IM_COL32(180, 0, 0, 255));
				ImGui::Text(data.data.c_str());

				return false;
			}

			template <typename T, int N = 0>
			void DrawRestPOD(const T& data)
			{
				if constexpr (N == boost::pfr::tuple_size_v<T>)
				{
					return;
				}
				else
				{
					DrawPOD(std::string(boost::pfr::get_name<N, T>()), boost::pfr::get<N>(data));
					DrawRestPOD<T, N + 1>(data);
				}
			}

			palette::PaletteManager& pm = palette::PaletteManager::GetSingletonInstance();
			ResourceManager& erm = ResourceManager::GetSingletonInstance();
		};
	}
}
