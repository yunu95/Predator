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
    LIGHTING_MEMBER_COUNT = 4,
    IRRADIANCE_MEMBER_COUNT = 1,
    SPECIBL_MEMBER_COUNT = 1,
    DOWN2X2_0_COUNT = 1,
    DOWN2X2_1_COUNT = 1,
    DOWN2X2_2_COUNT = 1,
    BLUR_COUNT = 1,
	UP2X2_0_COUNT = 1,
	UP2X2_1_COUNT = 1,
	UP2X2_2_COUNT = 1,
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
    SPECULAR_LIGHT = 3,
    IRRADIANCE = 0,
    SPECIBL = 0,
    DOWN2X2_0 = 0,
    DOWN2X2_1 = 0,
    DOWN2X2_2 = 0,
    BLUR = 0,
    UP2X2_0 = 0,
    UP2X2_1 = 0,
    UP2X2_2 = 0,
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
    DOWN2X2_0,
    DOWN2X2_1,
    DOWN2X2_2,
    BLUR,
    UP2X2_0,
    UP2X2_1,
    UP2X2_2,
    FINAL,
    END
};
