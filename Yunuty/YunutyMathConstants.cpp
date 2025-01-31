#include "YunutyEngine.h"
#include "YunutyMathConstants.h"

const double yunutyEngine::math::PI = 3.1415926535897931;
const double yunutyEngine::math::Deg2Rad = yunutyEngine::math::PI / 180;
const double yunutyEngine::math::Rad2Deg = 180 / yunutyEngine::math::PI;

float yunutyEngine::math::LerpF(float v1, float v2, float t)
{
	return v1 * (1 - t) + v2 * t;
}
