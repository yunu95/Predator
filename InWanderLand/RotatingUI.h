#pragma once
#include "YunutyEngine.h"

class RotatingUI : public Component
{
public:
    // initialRotation
    float initialRotation;
    float rotatingSpeed;
    float zRot = 0;
    virtual void Update()
    {
        GetTransform()->SetLocalRotation(Vector3d{ 0,0,zRot += rotatingSpeed * Time::GetDeltaTimeUnscaled() });
    }
private:
};