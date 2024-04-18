enum class UIExportFlag
{
    None = 0,
    // 마우스가 올라갔을 때, 마우스가 클릭했을 때 등의 상호작용을 받습니다.
    IsButton = 1 << 0,
    // 창을 닫는 용도로 쓰입니다.
    CloseButton = 1 << 1,
    // 특정 창을 여는 용도로 쓰입니다.
    OpeningButton = 1 << 2,
    // 특정 UI를 닫는 용도로 쓰입니다.
    DiablingButton = 1 << 3,
    // 비활성화시 그레이 스케일 적용된 이미지를 출력합니다.
    GrayScaleDisable = 1 << 4,
    // 활성화될때 팝업 효과를 줍니다.
    IsPoppingUp = 1 << 5,
    // 버튼 위에 마우스 포인터가 올라왔을 때 자식 오브젝트들을 보여줍니다.
    IsIncludingTooltips = 1 << 6,
    // 게임을 시작할때 비활성화된 채로 시작합니다.
    DisableOnStart = 1 << 7,
    // 버튼이 토글 동작을 할때 사용됩니다.
    IsToggle = 1 << 8,
    // UI 이미지가 활성화될때 트랜슬레이션 오프셋 애니메이션을 재생합니다.
    IsTranslatingOnEnable = 1 << 9,
    // UI 이미지가 비활성화될때 트랜슬레이션 오프셋 애니메이션을 재생합니다.
    IsTranslatingOnDisable = 1 << 10,
    // 텍스트입니다.
    IsText = 1 << 11,
    // 체력, 마나와 같은 값을 70 / 100과 같은 형태로 표시하는 텍스트입니다.
    // Adjuster에 들어가는 실수 값을 분자로 사용하며, 최대값이 갱신되면 분모도 갱신됩니다.
    IsGuageText = 1 << 12,
    // 0~1 사이의 값으로 UI 이미지의 세로 크기를 조절할 수 있습니다.
    CanAdjustHeight = 1 << 13,
};
