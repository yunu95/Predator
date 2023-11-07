#include "MoveDetector.h"
#include "Unit.h"

MoveDetector* MoveDetector::instance = nullptr;

MoveDetector::MoveDetector()
{
	instance = this;
}

MoveDetector* MoveDetector::GetInstance()
{
	if (instance == nullptr)
		instance = new MoveDetector;
	return instance;
}

void MoveDetector::SetTargetUnit(Unit* target)
{
	std::vector<Unit*> tempVector;
	m_unitMap.insert({ target, tempVector });
}

void MoveDetector::SetChaserUnit(Unit* target, Unit* chaser)
{
	m_unitMap.find(target)->second.push_back(chaser);
}

void MoveDetector::TargetMove(Unit* target, Vector3d endPosition)
{
	if (m_unitMap.find(target)->second.empty() == false)
	{
		for (auto e : m_unitMap.find(target)->second)
		{
			e->MoveDetect(endPosition);
		}
	}

}

