#include "PopupOnEnable.h"

void PopupOnEnable::Init()
{
    isRealtime = true;
    onUpdate = [this](float t)
        {
            float scale = (1 - (1 - t) * (1 - t));
            GetTransform()->SetLocalScale({ x ? scale : 1,y ? scale : 1,z ? scale : 1 });
        };
    onActivation = [this]()
        {
            GetTransform()->SetLocalScale(Vector3d::zero);
        };
}