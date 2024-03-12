#include "InWanderLand.h"
#include "Panel_Inspector.h"
#include "Application.h"
#include "PaletteList.h"

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
                ImGui_DrawTransform();
                imgui::draw::Underline();
                imgui::ShiftCursorY(20);
                ImGui_DrawTemplateData();
            }

            ImGui::End();
        }

        void InspectorPanel::Finalize()
        {

        }

        InspectorPanel::InspectorPanel()
        {

        }

        void InspectorPanel::ImGui_DrawTransform()
        {
            auto& selections = pm.GetCurrentPalette()->GetSelections();
            int idx = 0;
            if (imgui::BeginSection_1Col(idx, "TransForm", ImGui::GetContentRegionAvail().x))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                Vector3f position = Vector3f();
                Vector3f rotation = Vector3f();
                Vector3f scale = Vector3f(1, 1, 1);

                bool reset[9] = { false };

                if (selections.size() == 1)
                {
                    position = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldPosition();
                    rotation = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldRotation().Euler();
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

                    (*selections.begin())->OnRelocate(position);
                    (*selections.begin())->OnRerotate(Quaternion(rotation));
                    (*selections.begin())->OnRescale(scale);
                    (*selections.begin())->ApplyAsPaletteInstance();
                }
                else
                {
                    bool valDiff[9] = { false };
                    bool reset[9] = { false };
                    bool type[9] = { false };
                    float errorRange = 0.0001;

                    Vector3f fposition = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldPosition();
                    Vector3f frotation = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldRotation().Euler();
                    Vector3f fscale = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetLocalScale();

                    position = fposition;
                    rotation = frotation;
                    scale = fscale;

                    for (auto each : selections)
                    {
                        position = each->GetPaletteInstance()->GetTransform()->GetWorldPosition();
                        rotation = each->GetPaletteInstance()->GetTransform()->GetWorldRotation().Euler();
                        scale = each->GetPaletteInstance()->GetTransform()->GetLocalScale();

                        if (abs(fposition.x - position.x) > errorRange)
                        {
                            valDiff[0] = true;
                        }
                        if (abs(fposition.y - position.y) > errorRange)
                        {
                            valDiff[1] = true;
                        }
                        if (abs(fposition.z - position.z) > errorRange)
                        {
                            valDiff[2] = true;
                        }
                        if (abs(frotation.x - rotation.x) > errorRange)
                        {
                            valDiff[3] = true;
                        }
                        if (abs(frotation.y - rotation.y) > errorRange)
                        {
                            valDiff[4] = true;
                        }
                        if (abs(frotation.z - rotation.z) > errorRange)
                        {
                            valDiff[5] = true;
                        }
                        if (abs(fscale.x - scale.x) > errorRange)
                        {
                            valDiff[6] = true;
                        }
                        if (abs(fscale.y - scale.y) > errorRange)
                        {
                            valDiff[7] = true;
                        }
                        if (abs(fscale.z - scale.z) > errorRange)
                        {
                            valDiff[8] = true;
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
                        type[3] = true;
                        break;
                    case application::editor::imgui::Vector3Flags::TypeY:
                        type[4] = true;
                        break;
                    case application::editor::imgui::Vector3Flags::TypeZ:
                        type[5] = true;
                        break;
                    default:
                        break;
                    }

                    Vector3d finalPosition = Vector3d();
                    Vector3d finalRotation = Vector3d();
                    Vector3d finalScale = Vector3d(1, 1, 1);

                    Vector3f deltaPos = Vector3f();
                    Vector3f deltaRot = Vector3f();
                    Vector3f deltaScl = Vector3f(1, 1, 1);

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
                        }
                        else if (reset[4])
                        {
                            finalRotation.y = 0;
                        }
                        else if (reset[5])
                        {
                            finalRotation.z = 0;
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
                            finalPosition.x = position.x;
                        }
                        else if (type[1])
                        {
                            finalPosition.y = position.y;
                        }
                        else if (type[2])
                        {
                            finalPosition.z = position.z;
                        }
                        else if (type[3])
                        {
                            finalRotation.x = rotation.x;
                        }
                        else if (type[4])
                        {
                            finalRotation.y = rotation.y;
                        }
                        else if (type[5])
                        {
                            finalRotation.z = rotation.z;
                        }
                        else if (type[6])
                        {
                            finalScale.x = scale.x;
                        }
                        else if (type[7])
                        {
                            finalScale.y = scale.y;
                        }
                        else if (type[8])
                        {
                            finalScale.z = scale.z;
                        }

                        each->OnRelocate(finalPosition);
                        each->OnRerotate(finalRotation);
                        each->OnRescale(finalScale);
                        each->ApplyAsPaletteInstance();
                    }
                }

                imgui::EndSection();
            }
        }

        void InspectorPanel::ImGui_DrawTemplateData()
        {
            auto& tdm = TemplateDataManager::GetSingletonInstance();
            auto& selections = pm.GetCurrentPalette()->GetSelections();
            int idx = 0;
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
                        break;
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
                        break;
                    case application::editor::palette::Palette_List::Wave:
                        break;
                    case application::editor::palette::Palette_List::Cam:
                    {
                        templateList = tdm.GetDataList(DataType::CameraData);
                        break;
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
    }
}
