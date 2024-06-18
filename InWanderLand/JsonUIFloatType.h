#pragma once

struct JsonUIFloatType
{
    enum Enum {
        NONE,
        popUpDuration,
        popUpFrom,
        popUpTo,
        popDownDuration,
        popDownFrom,
        popDownTo,
        soundOnClick_delay,
        soundOnHover_delay,
        soundOnEnable_delay,
        soundOnDisable_delay,
        layoutNormalizingTime,
        pulsingMin,
        pulsingMax,
        pulsingPeriod,
        enableDelay,
        disableDelay,
        enableDuration,
        disableDuration,
        timeStoppingDuration,
        opacityFadeInDuration,
        opacityFadeOutDuration,
        adjustingRate,
        rotation,
        width,
        height,
        linearClipOnEnableDuration,
        linearClipOnDisableDuration,
        colorTintOnEnableDuration,
        colorTintOnDisableDuration,
        // 셀 하나당 차지하는 수치량
        barCells_GaugePerCell,
        // 셀이 덮어씌울 게이지의 가로, 세로 크기
        barCells_BarWidth,
        barCells_BarHeight,
        adjustLinearClipAdjustingRate,
        adjustLinearClipDirectionX, adjustLinearClipDirectionY,
        adjustLinearClipStartX, adjustLinearClipStartY,
        // 임의로 사용하게 될 사용자 플래그
        musicPlayOnEnable_fadeOut,
        musicPlayOnEnable_fadeIn,
        musicPlayOnDisable_fadeOut,
        musicPlayOnDisable_fadeIn,
        musicMultiplyVolumeOnEnableDisable_enableFactor,
        musicMultiplyVolumeOnEnableDisable_fadeDuration,
        disableAfterEnable_delayUntilDisable,
        rotatingSpeed,
        rotatingInitialRotation,
        videoDuration1,
        NUM
    };
};