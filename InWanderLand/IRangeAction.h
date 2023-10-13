#pragma once
#include "YunutyEngine.h"

class IRangeAction :
    public Component
{
public:
    virtual void IdentifyAction(Vector3d position) = 0;          // 인식 범위 내에 들어왔을 때 호출될 함수
    virtual void AttackAction(Vector3d position) = 0;            // 공격 범위 내에 들어왔을 때 호출될 함수
};

