#include "Unit.h"

void Unit::Start()
{

}

void Unit::Update()
{

}

void Unit::IdentifyAction(Vector3d position)
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 0, 0 });
}

void Unit::AttackAction(Vector3d position)
{

}
