#pragma once
#include "YunutyEngine.h"
using namespace yunutyEngine;

class ShakyCam : public yunutyEngine::graphics::Camera
{
private:
    Quaternion rot;

protected:
    void Start();
    
    void Update();
};
