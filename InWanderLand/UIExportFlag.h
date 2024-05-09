enum class UIExportFlag
{
    None = 0,
    // 마우스가 올라갔을 때, 마우스가 클릭했을 때 등의 상호작용을 받습니다.
    IsButton = 1 << 0,
    // 특정 UI들을 닫는 용도로 쓰입니다.
    CloseButton = 1 << 1,
    // 특정 UI들을 여는 용도로 쓰입니다.
    OpeningButton = 1 << 2,
    // 비활성화시 그레이 스케일 적용된 이미지를 출력합니다.
    GrayScaleDisable = 1 << 4,
    // 활성화될때 팝업 효과를 줍니다.
    IsPoppingUp = 1 << 5,
    // 비활성화될때 사이즈를 줄입니다.
    IsPoppingDown = 1 << 6,
    // 버튼 위에 마우스 포인터가 올라왔을 때 자식 오브젝트들을 보여줍니다.
    IsIncludingTooltips = 1 << 7,
    // 게임을 시작할때 비활성화된 채로 시작합니다.
    DisableOnStart = 1 << 8,
    // 버튼이 토글 동작을 할때 사용됩니다.
    IsToggle = 1 << 9,
    // UI 이미지가 활성화될때 트랜슬레이션 오프셋 애니메이션을 재생합니다.
    IsTranslatingOnEnable = 1 << 10,
    // UI 이미지가 비활성화될때 트랜슬레이션 오프셋 애니메이션을 재생합니다.
    IsTranslatingOnDisable = 1 << 11,
    // 체력, 마나와 같은 값을 70 / 100과 같은 형태로 표시하는 텍스트입니다.
    // Adjuster에 들어가는 실수 값을 분자로 사용하며, 최대값이 갱신되면 분모도 갱신됩니다.
    //IsGuageText = 1 << 12,
    // 0~1 사이의 값으로 UI 이미지의 세로 크기를 조절할 수 있습니다.
    CanAdjustHeight = 1 << 13,
    // 0~1 사이의 값으로 UI 이미지의 가로 크기를 조절할 수 있습니다.
    CanAdjustWidth = 1 << 14,
    // 0~1 사이의 값으로 UI 이미지의 Radial Fill 각을 조절할 수 있습니다.
    CanAdjustRadialFill = 1 << 15,
    // 원본 이미지가 그저 사각형 흰색 이미지인지 나타냅니다.
    IsBlankImage = 1 << 16,
    // 이 이미지 아래에 깔린 버튼은 클릭이 가능합니다.
    NoOverlaying = 1 << 17,
    // 이미지가 커졌다가 작아지는 애니메이션을 반복적으로 재생합니다.
    IsPulsing = 1 << 18,
    ///////////////////////////
    // 여러 자리의 숫자를 표현합니다. 기본적으로 내림으로 연산이 되나, 0~1 사이의 값을 올림처리할지, 내림처리할지는 속성값에 따라 결정됩니다.
    IsNumber = 1 << 19,
    // 클릭할 때 효과음을 재생합니다.
    PlaySoundOnClick = 1 << 20,
    // 마우스가 올라갔을 때 효과음을 재생합니다.
    PlaySoundOnHover = 1 << 21,
    // UI 객체가 활성화될때 효과음을 재생합니다.
    PlaySoundOnEnable = 1 << 22,
    // UI 객체가 비활성화될때 효과음을 재생합니다.
    PlaySoundOnDisable = 1 << 23,
    // 자식 게임 객체들의 위치를 인덱스별로 저장해뒀다가, 활성화된 자식 객체들을 인덱스별로 저장된 위치에 자식 인덱스 순서대로 배치합니다.
    PriorityLayout = 1 << 24,
    // 0~9까지의 숫자 폰트를 의미하는 폰트 이미지 세트를 자식으로 가집니다. 이 이미지 세트는 IsNumber 속성을 가진 UI객체가 폰트 이미지를 바꾸는 데에 사용됩니다.
    IsDigitFont = 1 << 25,
    // 스킬 업그레이드에 쓰이는 버튼입니다. 선행 업그레이드를 가질 수 있습니다.
    IsSkillUpgrade = 1 << 26,
    // UI의 크기가 화면의 해상도에 따라 달라집니다.
    // 1920, 1080 해상도에서 만들어진 UI 이미지를 1280, 720 해상도에서 사용할 때, 이미지의 크기를 0.6667배로 조정합니다.
    ScaledByResolution = 1 << 27,
    // UI가 활성화될때 시간을 멈춥니다.
    TimeStopOnEnable = 1 << 28,
    // UI가 비활성화될때 시간정지를 해제합니다.
    TimeContinueOnDisable = 1 << 29,
    // UI가 활성화될때 투명도 애니메이션을 재생합니다.
    ColorTintOnEnable = 1 << 30,
    // UI가 비활성화될때 투명도 애니메이션을 재생합니다.
    ColorTintOnDisable = 1 << 31,

};
enum class UIExportFlag2
{
    None = 0,
    LinearClipOnEnable = 1 << 0,
    LinearClipOnDisable = 1 << 1,
    Duplicatable = 1 << 2,
    // 체력바, 마나 바 등 연속적인 데이터의 수치를 추정하기 위해 게이지의 일정단위마다 셀을 끊어 표시하고 싶을때 사용됩니다.
    IsBarCells = 1 << 3,
    // 0일때 하나도 클립 안함, 1일때 완전히 클립함
    AdjustLinearClip = 1 << 4,
    PlayMusicOnEnable = 1 << 5,
    PlayMusicOnDisable = 1 << 6,
    PauseMusicOnEnable = 1 << 7,
    UnPauseMusicOnDisable = 1 << 8,
    MultiplyMusicVolumeOnEnableDisable = 1 << 9,
    ExclusiveEnable = 1 << 10,
    DisableAfterEnable = 1 << 11,
    Dialogue_Manual = 1 << 12,
    Dialogue_Timed = 1 << 13,
    RedundantEnable = 1 << 14,
};
