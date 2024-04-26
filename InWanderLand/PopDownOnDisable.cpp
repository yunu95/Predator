#include "PopDownOnDisable.h"

void PopDownOnDisable::Init()
{
    isRealtime = true;
    onUpdate = [this](float t)
        {
            float scale = (t - 1) * (t - 1);
            GetTransform()->SetLocalScale({ x ? scale : 1,y ? scale : 1,z ? scale : 1 });
        };
    onActivation = [this]()
        {
            GetTransform()->SetLocalScale(Vector3d::one);
        };
}
