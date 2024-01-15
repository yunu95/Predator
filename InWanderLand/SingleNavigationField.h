#pragma once
#include "YunutyEngine.h"
#include "YunutyNavigationField.h"

/// <summary>
/// InWanderLand 게임의 길찾기에서 사용되는 유일무이한 NavigationField입니다. 모든 게임 유닛들은 이 위에서 움직입니다.
/// </summary>
class SingleNavigationField : public yunutyEngine::NavigationField, public yunutyEngine::SingletonComponent<SingleNavigationField>
{
private:
};
