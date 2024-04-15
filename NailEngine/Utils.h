#pragma once
#define _HAS_STD_BYTE 0

#include <Windows.h>
#include <string>

template <class T>
class LazyObjects
{
public:
    T& Get()
    {
        if (!instance)
            instance = new T();
        return *instance;
    }

    void Release()
    {
        if (!instance)
            return;

        delete instance;
        instance = nullptr;
    }

private:
    T* instance = nullptr;
};

enum
{
    SWAP_CHAIN_BUFFER_COUNT = 2,
    G_BUFFER_MEMBER_COUNT = 6,
    LIGHTING_MEMBER_COUNT = 3,
    IRRADIANCE_MEMBER_COUNT = 1,
    SPECIBL_MEMBER_COUNT = 1,
    DOWN4X4_0_COUNT = 1,
    DOWN6X6_0_COUNT = 1,
    DOWN6X6_1_COUNT = 1,
    BLUR_COUNT = 1,
    UP6x6_0_COUNT = 1,
    UP4x4_0_COUNT = 1,
    FINAL_COUNT = 1,
};

enum
{
    POSITION,
    NORMAL,
    ALBEDO,
    UTIL,
    ARM,
    EMISSIVE,
    DIFFUSE = 0,
    AMBIENT = 1,
    LIGHT_SHADOW = 2,
    IRRADIANCE = 0,
    SPECIBL = 0,
    DOWN4X4_0 = 0,
    DOWN6X6_0 = 0,
    DOWN6X6_1 = 0,
    BLUR = 0,
    UP6x6_0 = 0,
    UP4x4_0 = 0,
    FINAL = 0,
    GBUFFER_COUNT
};

enum class RENDER_TARGET_TYPE
{
    //SWAP_CHAIN,
    G_BUFFER,
    LIGHTING,
    IRRADIANCE,
    SPECLIBL,
    DOWN4X4_0,
    DOWN6X6_0,
    DOWN6X6_1,
    BLUR,
    UP6x6_0,
    UP4x4_0,
    FINAL,
    END
};
