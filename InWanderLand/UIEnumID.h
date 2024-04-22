#pragma once
enum class UIEnumID
{
    // 아무것도 아님.
    None = 0,
    // 로빈, 우르술라, 헨젤의 초상화, 초상화를 가리는 적색 부상 오버레이, 체력을 표시하는 텍스트 UI
    Portrait_Robin,
    Portrait_Blood_Robin,
    Portrait_HP_Robin,
    Portrait_Ursula,
    Portrait_Blood_Ursula,
    Portrait_HP_Ursula,
    Portrait_Hansel,
    Portrait_Blood_Hansel,
    Portrait_HP_Hansel,
    // 로빈 스킬시전 Q
    Skill_Use_Q_Robin,
    // 로빈 스킬시전 Q 흑백 오버레이
    Skill_Use_Q_Robin_Overlay,
    // 로빈 스킬시전 W
    Skill_Use_W_Robin,
    // 로빈 스킬시전 W 흑백 오버레이
    Skill_Use_W_Robin_Overlay,
    // 우르술라 스킬시전 Q
    Skill_Use_Q_Ursula,
    // 우르술라 스킬시전 Q 흑백 오버레이
    Skill_Use_Q_Ursula_Overlay,
    // 우르술라 스킬시전 W
    Skill_Use_W_Ursula,
    // 우르술라 스킬시전 W 흑백 오버레이 
    Skill_Use_W_Ursula_Overlay,
    // 헨젤 스킬시전 Q
    Skill_Use_Q_HANSEL,
    // 헨젤 스킬시전 Q 흑백 오버레이
    Skill_Use_Q_HANSEL_Overlay,
    // 헨젤 스킬시전 W
    Skill_Use_W_HANSEL,
    // 헨젤 스킬시전 W 흑백 오버레이
    Skill_Use_W_HANSEL_Overlay,
    // 전술모드 토글버튼
    Toggle_TacticMode,
    // 전술모드 토글버튼 흑백 오버레이
    Toggle_TacticMode_Overlay,
    // 사운드가 켜진 상태에서 표시할 UI버튼
    Sound_On,
    // 사운드가 꺼진 상태에서 표시할 UI버튼
    Sound_Off,
    // 음악이 켜진 상태에서 표시할 UI버튼
    Music_On,
    // 음악이 꺼진 상태에서 표시할 UI버튼
    Music_Off,
    // 인게임에서 사용되는 캐릭터 상태창, 전술모드 진입 버튼을 포함하는 하단 레이아웃
    Ingame_Bottom_Layout,
    // 인게임에서 사용되는 메뉴버튼
    Ingame_MenuButton,
    // 콤보를 표시하는 위치, 콤보가 진행중인게 아니라면 표시되지 않을 수도 있음.
    Ingame_Combo,
    // 어떤 콤보를 달성해야 하는지 설명하는 텍스트
    Ingame_Combo_Description,
    BlackMask_RightToLeft,
    BlackMask_TopToBottom,
    BlackMask_LeftToRight,
    BlackMask_BottomToTop,
};
