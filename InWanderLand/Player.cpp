#include "Player.h"
#include "Dotween.h"

Player::Player()
{

}

void Player::Start()
{
	unitType = "Player";
	m_speed = 2.0f;
}

void Player::Update()
{

}

//void Player::IdentifyAction(Vector3d endPosition, float distance)
//{
//	//GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 1, 0 });
//	GetGameObject()->GetComponent<Dotween>()->DOMove(endPosition, distance / m_speed).OnComplete([=]()
//		{
//			AttackAction(endPosition, distance);
//		});
//}

//void Player::AttackAction(Vector3d endPosition, float distance)
//{
//	// 공격 사거리 내에 들어왔을 경우 호출되는 함수.
//	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 1, 0 });
//}

