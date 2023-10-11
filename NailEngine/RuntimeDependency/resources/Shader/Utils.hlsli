#ifndef _UTILS_HLSLI_
#define _UTILS_HLSLI_

bool UseTexture(uint useTexture)
{
    if(useTexture == 1)
    {
        return true;
    }
    return false;
}

#endif