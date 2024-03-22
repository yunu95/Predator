#pragma once
#include "IResource.h"
#include "YunuVector3.h"

namespace yunuGI
{
    class IMesh : virtual public IResource
    {
    public:
        virtual void GetBoundingBoxInfo(const yunuGI::Matrix4x4& wtm,Vector3* min, Vector3* max) = 0;
        virtual void SetDiffuseExposure(float exposure) = 0;
        virtual void SetAmbientExposure(float exposure) = 0;
    private:

    };
}