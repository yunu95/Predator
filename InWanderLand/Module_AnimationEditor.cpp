#include "InWanderLand.h"
#include "Module_AnimationEditor.h"
#include "Application.h"

namespace application
{
    namespace editor
    {
        Module_AnimationEditor::~Module_AnimationEditor()
        {
        }

        void Module_AnimationEditor::Initialize()
        {
        }

        void Module_AnimationEditor::Update(float ts)
        {
        }

        void Module_AnimationEditor::GUIProgress()
        {
            CheckActivation();

            ImGui::Begin("Animation Editor", &activation);

            /// ImGui 관련 내부 변수 업데이트
            isMouseOver = ImGui::IsWindowHovered();
            isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

            /// 실제 패널에 그리는 영역
            DrawLayout();

            ImGui::End();
        }

        void Module_AnimationEditor::Finalize()
        {
        }

        Module_AnimationEditor::Module_AnimationEditor()
        {
        }

        void Module_AnimationEditor::DrawLayout()
        {
            static auto spacing = ImGui::GetStyle().ItemSpacing;

            static int idx = 0;
            imgui::BeginSection_2Col(idx, "Animation Module", ImGui::GetContentRegionAvail().x, 0.2f);

            static std::vector<std::string> selections = std::vector<std::string>();
            std::string current = selectedTemplateUnitKey;

            selections.resize(0);
            for (auto& each : TemplateDataManager::GetSingletonInstance().GetDataList(DataType::UnitData))
            {
                selections.push_back(each->GetDataKey());
            }

            std::sort(selections.begin(), selections.end());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Selected Unit Template");
            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-1);

            imgui::SmartStyleColor textColor2(ImGuiCol_Text, IM_COL32_WHITE);

            if (ImGui::BeginCombo("##Selected Unit Template", selectedTemplateUnitKey.c_str()))
            {
                for (int i = 0; i < selections.size(); i++)
                {
                    const bool is_selected = (current == selections[i]);
                    if (ImGui::Selectable(selections[i].c_str(), is_selected))
                    {
                        current = selections[i];
                        selectedTemplateUnitKey = current;
                    }

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::PopItemWidth();

            imgui::ShiftCursorY(10);
            if (auto ut = TemplateDataManager::GetSingletonInstance().GetTemplateData(selectedTemplateUnitKey))
            {
                DrawList(static_cast<Unit_TemplateData*>(ut));
            }
            else
            {
                selectedTemplateUnitKey = "None";
                startAnim = 0;
                destAnim = 0;
            }
            imgui::EndSection();
        }

        void Module_AnimationEditor::DrawList(Unit_TemplateData* unitTemplate)
        {
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

            imgui::Dropdown_2Col("Start Anim Type", &nameList[0], (int)UnitAnimType::End, &startAnim);
            imgui::Dropdown_2Col("Dest Anim Type", &nameList[0], (int)UnitAnimType::End, &destAnim);

            if (startAnim != (int)UnitAnimType::None && destAnim != (int)UnitAnimType::None && startAnim != destAnim)
            {
                auto val = unitTemplate->pod.animationBlendMap[std::pair(startAnim, destAnim)];
                imgui::DragFloat_2Col("Transition Duration", unitTemplate->pod.animationBlendMap.at(std::pair(startAnim, destAnim)), true, 0.1f);
            }
        }
    }
}