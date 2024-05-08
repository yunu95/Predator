#pragma once
#include "Storable.h"

struct JsonUIData
{
    // string imageName;
    std::string uiName;
    int uiIndex;
    // 부모가 없다면 부모를 1920 * 1080 크기의 스크린 스페이스로 가정하게 된다.
    int parentUIIndex = -1;
    std::string imagePath;
    int imagePriority;
    // 만약 플래그에 openingButton이 있다면 버튼을 눌렀을 때 활성화시킬 UI 창을 의미한다.
    std::vector<int> openTargets;
    // 만약 플래그에 diablingButton이 있다면 버튼을 눌렀을 때 비활성화시킬 UI 창을 의미한다.
    std::vector<int> closeTargets;
    std::vector<int> hoverEnableTargets;
    std::vector<float> color;
    std::vector<float> pivot;
    std::vector<float> anchor;
    std::vector<float> anchoredPosition;
    std::vector<float> enableOffset;
    std::vector<float> disableOffset;
    float popUpDuration;
    float popDownDuration;
    bool popUpX, popUpY, popUpZ;
    bool popDownX, popDownY, popDownZ;
    // 숫자를 표현할 때, 각 자릿수별로 숫자 폰트 이미지를 갈아치울 UI 객체들을 의미합니다.
    std::vector<int> numberDigits;
    // 숫자를 표현할 때 사용할 숫자 폰트 이미지 세트를 의미합니다.
    int numberFontSet;
    // 0~9까지의 숫자 폰트 이미지를 매핑합니다.
    std::vector<int> numberFontSetImages;
    // 숫자를 표현할 때, 올림처리하면 true, 내림처리하면 false입니다.
    bool numberCeil;
    // 0을 표시할지 여부입니다.
    bool numberShowZero;
    std::string soundOnClick;
    float soundOnClick_delay;
    std::string soundOnHover;
    float soundOnHover_delay;
    std::string soundOnEnable;
    float soundOnEnable_delay;
    std::string soundOnDisable;
    float soundOnDisable_delay;
    float layoutNormalizingTime;
    float pulsingMin;
    float pulsingMax;
    float pulsingPeriod;
    float enableDelay;
    float disableDelay;
    float enableDuration;
    float disableDuration;
    int enableCurveType;
    int disableCurveType;
    float timeStoppingDuration;
    float opacityFadeInDuration;
    float opacityFadeOutDuration;
    float adjustingRate;
    float rotation;
    float width;
    float height;
    // 업그레이드 버튼의 경우, 활성화하기 위해 필요한 다른 버튼의 인덱스를 의미합니다.
    int dependentUpgrade{ -1 };
    vector<float> linearClipOnEnableStart;
    vector<float> linearClipOnEnableDir;
    float linearClipOnEnableDuration;
    int linearClipOnEnableCurveType;
    vector<float> linearClipOnDisableStart;
    vector<float> linearClipOnDisableDir;
    float linearClipOnDisableDuration;
    int linearClipOnDisableCurveType;
    vector<float> colorTintOnEnableStart;
    vector<float> colorTintOnEnableEnd;
    float colorTintOnEnableDuration;
    int colorTintOnEnableCurveType;
    vector<float> colorTintOnDisableStart;
    vector<float> colorTintOnDisableEnd;
    float colorTintOnDisableDuration;
    int colorTintOnDisableCurveType;
    // 전체 셀의 갯수
    int barCells_CellNumber;
    // 셀 하나당 차지하는 수치량
    float barCells_GaugePerCell;
    // 셀이 덮어씌울 게이지의 가로, 세로 크기
    float barCells_BarWidth;
    float barCells_BarHeight;
    float adjustLinearClipAdjustingRate;
    float adjustLinearClipDirectionX, adjustLinearClipDirectionY;
    float adjustLinearClipStartX, adjustLinearClipStartY;
    // 임의로 사용하게 될 사용자 플래그
    bool disableOnStartEdtior;
    bool disableOnStartExe;
    string musicPlayOnEnable_musicClip;
    float musicPlayOnEnable_fadeOut;
    float musicPlayOnEnable_fadeIn;
    string musicPlayOnDisable_musicClip;
    float musicPlayOnDisable_fadeOut;
    float musicPlayOnDisable_fadeIn;
    float musicMultiplyVolumeOnEnableDisable_enableFactor;
    int customFlags;
    int customFlags2;
    // UI의 고유한 EnumID
    int enumID;
    FROM_JSON(JsonUIData);
};
