#pragma once
#include "IResource.h"

namespace yunuGI
{
    class IVideo : virtual public IResource
    {
    public:
        virtual float GetWidth() = 0;
        virtual float GetHeight() = 0;
        //virtual float GetDuration() = 0;
        virtual void update(float deltaTime) = 0;
        virtual void SetRepeat(bool repeat) = 0;
        virtual bool IsDonePlaying() = 0;
    };
}
