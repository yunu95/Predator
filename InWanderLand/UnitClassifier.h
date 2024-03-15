#pragma once
#include "YunutyEngine.h"

namespace application
{
	namespace editor
	{
		class POD_Unit;
	}
}

/// <summary>
/// pod의 unit이 웨이브 유닛인지 아닌지 분류하고, 그에 따라 적절한 동작을 담당하는 클래스.
/// </summary>
class UnitClassifier : public SingletonClass<UnitClassifier>
{
public:
	void SendPODToClassifier(application::editor::POD_Unit p_pod);
};

