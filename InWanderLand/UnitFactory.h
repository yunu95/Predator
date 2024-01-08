#pragma once
#include "YunutyEngine.h"
/// <summary>
/// UnitProductionOrder 클래스에 유닛 생성을 명령하고,
/// 만들어진 유닛에 길찾기 컴포넌트를 붙여주는 클래스.
/// 는 필요 없지
/// </summary>

class UnitFactory
{
public:
	void OrderCreateUnit(yunutyEngine::GameObject* orderedUnit, yunutyEngine::NavigationField* navField, Vector3d startPosition);
};

