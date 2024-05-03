#pragma once
enum class UIEnumID
{
    // 아무것도 아님.
    None = 0,
    // 로빈, 우르술라, 헨젤의 초상화, 초상화 위의 이름 태그, 초상화를 가리는 적색 부상 오버레이, 체력바, 체력을 표시하는 텍스트 UI
    Portrait_Robin,
    Portrait_Robin_NameTag,
    Portrait_Blood_Robin,
    Portrait_HP_Robin,
    Portrait_HP_Robin_Number_Current,
    Portrait_HP_Robin_Number_Max,
    Portrait_Robin_Buff_Bleeding,
    Portrait_Robin_Buff_Blinding,
    Portrait_Robin_Buff_Paralysis,
    Portrait_Robin_Buff_KnockBack,
    Portrait_Robin_Buff_Taunted,
    Portrait_Ursula,
    Portrait_Ursula_NameTag,
    Portrait_Blood_Ursula,
    Portrait_HP_Ursula,
    Portrait_HP_Ursula_Number_Current,
    Portrait_HP_Ursula_Number_Max,
    Portrait_Ursula_Buff_Bleeding,
    Portrait_Ursula_Buff_Blinding,
    Portrait_Ursula_Buff_Paralysis,
    Portrait_Ursula_Buff_KnockBack,
    Portrait_Ursula_Buff_Taunted,
    Portrait_Hansel,
    Portrait_Hansel_NameTag,
    Portrait_Blood_Hansel,
    Portrait_HP_Hansel,
    Portrait_HP_Hansel_Number_Current,
    Portrait_HP_Hansel_Number_Max,
    Portrait_Hansel_Buff_Bleeding,
    Portrait_Hansel_Buff_Blinding,
    Portrait_Hansel_Buff_Paralysis,
    Portrait_Hansel_Buff_KnockBack,
    Portrait_Hansel_Buff_Taunted,
    // 로빈 스킬시전 Q
    Skill_Use_Q_Robin,
    // 로빈 스킬시전 Q 흑백 오버레이
    Skill_Use_Q_Robin_Overlay,
    Skill_Use_Q_Robin_Cooltime_Number,
    // 로빈 스킬시전 W
    Skill_Use_W_Robin,
    // 로빈 스킬시전 W 흑백 오버레이
    Skill_Use_W_Robin_Overlay,
    Skill_Use_W_Robin_Cooltime_Number,
    // 우르술라 스킬시전 Q
    Skill_Use_Q_Ursula,
    // 우르술라 스킬시전 Q 흑백 오버레이
    Skill_Use_Q_Ursula_Overlay,
    Skill_Use_Q_Ursula_Cooltime_Number,
    // 우르술라 스킬시전 W
    Skill_Use_W_Ursula,
    // 우르술라 스킬시전 W 흑백 오버레이 
    Skill_Use_W_Ursula_Overlay,
    Skill_Use_W_Ursula_Cooltime_Number,
    // 헨젤 스킬시전 Q
    Skill_Use_Q_HANSEL,
    // 헨젤 스킬시전 Q 흑백 오버레이
    Skill_Use_Q_HANSEL_Overlay,
    Skill_Use_Q_HANSEL_Cooltime_Number,
    // 헨젤 스킬시전 W
    Skill_Use_W_HANSEL,
    // 헨젤 스킬시전 W 흑백 오버레이
    Skill_Use_W_HANSEL_Overlay,
    Skill_Use_W_HANSEL_Cooltime_Number,
    // 전술모드 토글버튼
    Toggle_TacticMode,
    // 전술모드 토글버튼 흑백 오버레이
    Toggle_TacticMode_Overlay,
    Toggle_TacticMode_Cooltime_Number,
    // 마나 게이지
    ManaFill,
    // 현재 마나량을 나타내는 텍스트
    Mana_Text_CurrentMP,
    // 최대 마나량을 나타내는 텍스트
    Mana_Text_MaxMP,
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
    // 콤보 횟수를 표시함
    Ingame_Combo_Number,
    // "Combo"라는 문자를 표시함
    Ingame_Combo_Text,
    Ingame_Combo_DescriptionTitleImg,
    // 어떤 콤보를 달성해야 하는지 설명하는 텍스트, 몇 콤보를 달성해야 하는지에 대한 표시, 콤보 달성을 표시하는 체크박스의 V자 문양
    Ingame_Combo_Description1,
    Ingame_Combo_DescriptionImageUnfinished1,
    Ingame_Combo_DescriptionImageFinished1,
    Ingame_Combo_TargetNumUnfinished1,
    Ingame_Combo_TargetNumFinished1,
    Ingame_Combo_Check1,
    Ingame_Combo_Description2,
    Ingame_Combo_DescriptionImageUnFinished2,
    Ingame_Combo_DescriptionImageFinished2,
    Ingame_Combo_TargetNumUnfinished2,
    Ingame_Combo_TargetNumFinished2,
    Ingame_Combo_Check2,
    Ingame_Combo_Description3,
    Ingame_Combo_DescriptionImageUnFinished3,
    Ingame_Combo_DescriptionImageFinished3,
    Ingame_Combo_TargetNumUnfinished3,
    Ingame_Combo_TargetNumFinished3,
    Ingame_Combo_Check3,
    BlackMask_RightToLeft,
    BlackMask_TopToBottom,
    BlackMask_LeftToRight,
    BlackMask_BottomToTop,
    LetterBox_Top,
    LetterBox_Bottom,
    // 인게임 하단에서 클릭할 시 스킬트리 메뉴를 여는 메뉴버튼
    InGame_SkiltreeMenu_Active,
    InGame_SkiltreeMenu_InActive,
    SkillPoint_Number,
    SkillUpgradeButtonRobin00,
    SkillUpgradeButtonRobin11,
    SkillUpgradeButtonRobin12,
    SkillUpgradeButtonRobin21,
    SkillUpgradeButtonRobin22,
    SkillUpgradeButtonUrsula00,
    SkillUpgradeButtonUrsula11,
    SkillUpgradeButtonUrsula12,
    SkillUpgradeButtonUrsula21,
    SkillUpgradeButtonUrsula22,
    SkillUpgradeButtonHansel00,
    SkillUpgradeButtonHansel11,
    SkillUpgradeButtonHansel12,
    SkillUpgradeButtonHansel21,
    SkillUpgradeButtonHansel22,
    PopUpMessage_NotEnoughSP,
    PopUpMessage_RequirementNotMet,
    PopUpMessage_PermissionForUpgrade,
    PopUpMessage_PermissionForUpgradeProceedButton,
    PopUpMessage_WarningForRestart,
    PopUpMessage_WarningForRestart_ProceedButton,
    EnemyStatus,
    EnemyStatus_HP_Number_Current,
    EnemyStatus_HP_Number_Max,
    EnemyStatus_HP_Cells,
    EnemyStatus_HP_Fill,
    EnemyStatus_Buff_Bleeding,
    EnemyStatus_Buff_Blinding,
    EnemyStatus_Buff_Paralysis,
    EnemyStatus_Buff_KnockBack,
    EnemyStatus_Buff_Taunted,
};
