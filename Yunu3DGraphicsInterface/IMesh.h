#pragma once
#include "IResource.h"
#include "YunuVector3.h"

namespace yunuGI
{
    class IMesh : virtual public IResource
    {
    public:
        virtual void GetBoundingBoxInfo(Vector3* min, Vector3* max) = 0;
    private:

    };
}