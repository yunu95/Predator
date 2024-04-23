#pragma once
#include "Storable.h"
struct JsonUIData
{
    //public string imageName;
    string uiname;
    int uiIndex;
    // 부모가 없다면 부모를 1920 * 1080 크기의 스크린 스페이스로 가정하게 된다.
    int parentUIIndex;
    string imagePath;
    // 만약 플래그에 openingButton이 있다면 버튼을 눌렀을 때 활성화시킬 UI 창을 의미한다.
    int openTarget;
    // 만약 플래그에 diablingButton이 있다면 버튼을 눌렀을 때 비활성화시킬 UI 창을 의미한다.
    int disablingTargetIndex;
    std::vector<float> color;
    std::vector<float> pivot;
    std::vector<float> anchor;
    std::vector<float> anchoredPosition;
    std::vector<float> enableOffset;
    std::vector<float> disableOffset;
    double enableDelay;
    double disableDelay;
    double enableDuration;
    double disableDuration;
    int enableCurveType;
    int disableCurveType;
    double adjustingRate;
    double rotation;
    double width;
    double height;
    // 임의로 사용하게 될 사용자 플래그
    int customFlags;
    // UI의 고유한 EnumID
    int enumID;
    FROM_JSON(JsonUIData)
};
