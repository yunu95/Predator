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
        virtual void update(float deltaTime, bool autoloop) = 0;
        virtual bool IsDonePlaying() = 0;
        virtual void ResetVideo() = 0;
    };
}
