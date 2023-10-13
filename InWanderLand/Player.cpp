#include "Player.h"
#include "Dotween.h"

Player::Player()
{

}

void Player::Start()
{

}

void Player::Update()
{

}

void Player::IdentifyAction(Vector3d position)
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 1, 0 });
}

void Player::AttackAction(Vector3d position)
{

}


