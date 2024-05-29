/// 2024. 02. 05 김상준
/// POD 에서 사용할 수 있는 기본적인 Struct 를 제공하는 헤더 파일

#pragma once

#include "Storable.h"
#include "imgui.h"
#include "imgui_Utility.h"
#include "EditorMath.h"
#include <type_traits>

namespace application
{
    template <typename T> requires std::is_floating_point_v<T>
    struct POD_Vector2
    {
        T x = 0;
        T y = 0;

        operator yunuGI::Vector2() const
        {
            yunuGI::Vector2 finalVal;
            finalVal.x = x;
            finalVal.y = y;
            return finalVal;
        }

        TO_JSON(POD_Vector2)
            FROM_JSON(POD_Vector2)
    };

    template <typename T> requires std::is_floating_point_v<T>
    struct POD_Vector3
    {
        T x = 0;
        T y = 0;
        T z = 0;

        operator yunuGI::Vector3() const
        {
            yunuGI::Vector3 finalVal;
            finalVal.x = x;
            finalVal.y = y;
            finalVal.z = z;
            return finalVal;
        }

        TO_JSON(POD_Vector3)
            FROM_JSON(POD_Vector3)
    };

    template <typename T> requires std::is_floating_point_v<T>
    struct POD_Vector4
    {
        T x = 0;
        T y = 0;
        T z = 0;
        T w = 1;

        operator yunuGI::Vector4() const
        {
            yunuGI::Vector4 finalVal;
            finalVal.x = x;
            finalVal.y = y;
            finalVal.z = z;
            finalVal.w = w;
            return finalVal;
        }

        TO_JSON(POD_Vector4)
            FROM_JSON(POD_Vector4)
    };

    template <typename T> requires std::is_floating_point_v<T>
    struct POD_Quaternion
    {
        T x = 0;
        T y = 0;
        T z = 0;
        T w = 1;

        operator yunuGI::Quaternion() const
        {
            yunuGI::Quaternion finalVal;
            finalVal.x = x;
            finalVal.y = y;
            finalVal.z = z;
            finalVal.w = w;
            return finalVal;
        }

        TO_JSON(POD_Quaternion)
            FROM_JSON(POD_Quaternion)
    };

    // GetEnumNameMap 함수는 Enum을 정의하는 곳에서 알아서 특수화를 해줘야 한다.
    template <typename EnumType>
    struct POD_Enum
    {
        int enumValue;
        std::string enumName;
        POD_Enum() = default;
        POD_Enum(EnumType enumValue) { operator=(enumValue); };
        POD_Enum(const POD_Enum<EnumType>&) = default;
        POD_Enum& operator=(const POD_Enum<EnumType>&) = default;
        ~POD_Enum() = default;

        operator EnumType() const {
            return static_cast<EnumType>(enumValue);
        }
        POD_Enum& operator=(const EnumType& value) {
            enumValue = static_cast<int>(value);
            enumName = GetEnumNameMap().at(value);
        }
        const std::unordered_map<int, std::string>& GetEnumNameMap() const;
        const std::unordered_map<std::string, int>& GetNameEnumMap()
        {
            static std::unordered_map<std::string, int> nameEnumMap;
            if (nameEnumMap.empty())
            {
                for (auto& pair : GetEnumNameMap())
                {
                    nameEnumMap[pair.second] = pair.first;
                }
            }
            return nameEnumMap;
        }
        TO_JSON(POD_Enum)
            FROM_JSON(POD_Enum)
    };

    namespace imgui
    {
        namespace data
        {
            template <typename EnumType>
            bool DrawData(std::string label, const POD_Enum<EnumType>& data)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                //imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
                ImGui::AlignTextToFramePadding();
                ImGui::Text(label.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::PushItemWidth(-1);

                //imgui::SmartStyleColor textColor2(ImGuiCol_Text, IM_COL32_WHITE);

                bool returnVal = false;
                static std::vector<std::string> selections = std::vector<std::string>();
                std::string current = data.GetEnumNameMap().at(data.Get());

                selections.resize(0);
                std::transform(data.GetEnumNameMap().begin(), data.GetEnumNameMap().end(), std::back_inserter(selections)
                    , [](const std::pair<int, std::string>& each) { return each.second; });

                if (ImGui::BeginCombo(("##editableEnum : " + label).c_str(), current.c_str()))
                {
                    for (int i = 0; i < selections.size(); i++)
                    {
                        const bool is_selected = (current == selections[i]);
                        if (ImGui::Selectable(selections[i].c_str(), is_selected))
                        {
                            current = selections[i];
                            TemplateDataManager::GetSingletonInstance().GetSelectedTemplateData()->SetDataResourceName(current);
                            data.Set(data.GetNameEnumMap().at(current));
                            returnVal = true;
                        }

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();
                return returnVal;
            }
            template <typename T>
            bool DrawData(std::string label, const POD_Vector2<T>& data)
            {
                return Vector2_2Col(label, const_cast<T&>(data.x), const_cast<T&>(data.y));
            }

            template <typename T>
            bool DrawData(std::string label, const POD_Vector3<T>& data)
            {
                return Vector3_2Col(label, const_cast<T&>(data.x), const_cast<T&>(data.y), const_cast<T&>(data.z));
            }

            template <typename T>
            bool DrawData(std::string label, const POD_Vector4<T>& data)
            {
                return Vector4_2Col(label, const_cast<T&>(data.x), const_cast<T&>(data.y), const_cast<T&>(data.z), const_cast<T&>(data.w));
            }

            template <typename T>
            bool DrawData(std::string label, const POD_Quaternion<T>& data)
            {
                bool returnVal = false;
                auto angle = editor::math::GetEulerAngle(data);

                returnVal = Vector3_2Col(label, angle.x, angle.y, angle.z);
                auto finalQuat = editor::math::GetQuaternion(angle);
                const_cast<T&>(data.x) = finalQuat.x;
                const_cast<T&>(data.y) = finalQuat.y;
                const_cast<T&>(data.z) = finalQuat.z;
                const_cast<T&>(data.w) = finalQuat.w;

                return returnVal;
            }
        }
    }
}
