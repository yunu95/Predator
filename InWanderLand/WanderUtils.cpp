#include "WanderUtils.h"

float wanderUtils::ResourceRecursiveLoader::coroutineDeltaTimeThreshold = 0.02f;
bool wanderUtils::ResourceRecursiveLoader::isLoadingResources = false;
bool wanderUtils::ResourceRecursiveLoader::IsLoadingResources()
{
    return isLoadingResources;
}
