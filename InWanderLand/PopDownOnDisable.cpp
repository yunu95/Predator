#include "PopDownOnDisable.h"

void PopDownOnDisable::Init()
{
    isRealtime = true;
    onUpdate = [this](float t)
        {
            GetTransform()->SetLocalScale(Vector3f::Lerp(popDownFrom, Vector3f::one * popDownTo, (1 - (1 - t) * (1 - t))));
        };
    onActivation = [this]()
        {
            popDownFrom = GetTransform()->GetLocalScale();
        };
    onCompleteFunction = [this]()
        {
            GetGameObject()->SetSelfActive(false);
        };
}
