#include "Random.h"

using namespace yunutyEngine;
using namespace yunutyEngine::math;

Random* Random::instance = nullptr;
Random::Random()
{
    gen = mt19937(rd());
}
int Random::GetRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> dis(min, max);
    return dis(SingleInstance()->gen);
}
float Random::GetRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> dis(min, max);
    return dis(SingleInstance()->gen);
}
float Random::GetRandomFloat(float amplitude)
{
    std::uniform_real_distribution<float> dis(-amplitude, amplitude);
    return dis(SingleInstance()->gen);
}
bool yunutyEngine::math::Random::ByRandomChance(float chance)
{
    return GetRandomFloat(0.f, 1.f) < chance;
}
Random* Random::SingleInstance()
{
    if (instance == nullptr)
        instance = new Random();
    return instance;
}
