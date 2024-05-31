//#include "ChessTrapSkillSystem.h"
//
//void ChessTrapSkillSystem::Start()
//{
//	SetOtherComponentsAsMember();
//
//	SetSkillRequirmentsActive(m_chessSkillRequirementOne, false);
//	SetSkillRequirmentsActive(m_chessSkillRequirementTwo, false);
//
//	m_chessSkillRequirementOne.colliderObject->SetParent(GetGameObject());
//	m_chessSkillRequirementOne.debugObject->SetParent(GetGameObject());
//	m_chessSkillRequirementTwo.colliderObject->SetParent(GetGameObject());
//	m_chessSkillRequirementTwo.debugObject->SetParent(GetGameObject());
//
//	/*SetSkillRequirmentsPosition(m_chessSkillRequirementOne, Vector3d::zero);
//	SetSkillRequirmentsPosition(m_chessSkillRequirementTwo, Vector3d::zero);*/
//
//	switch (m_chessPieceType)
//	{
//		case ChessTrapSkillSystem::ChessPieceType::Pawn:
//		{
//			float pawnLength = 5.0f * m_unitLength;
//			m_chessSkillRequirementOne.colliderObject->GetComponent<physics::BoxCollider>()->SetHalfExtent({ pawnLength * 0.5f, 0.5f * 0.5f , pawnLength * 0.5f });
//			m_chessSkillRequirementOne.debugObject->GetTransform()->SetWorldScale({ pawnLength , 0.5f ,pawnLength });
//
//			m_chessSkillRequirementTwo.colliderObject->GetComponent<physics::BoxCollider>()->SetHalfExtent(Vector3d::zero);
//			m_chessSkillRequirementTwo.debugObject->GetTransform()->SetWorldScale(Vector3d::zero);
//			break;
//		}
//		case ChessTrapSkillSystem::ChessPieceType::Rook:
//		{
//			float rookLength = 5.0f * m_unitLength;
//			float rookWidth = 1.0f * m_unitLength;
//			m_chessSkillRequirementOne.colliderObject->GetComponent<physics::BoxCollider>()->SetHalfExtent({ rookWidth * 0.5f, 0.5f * 0.5f, rookLength * 0.5f });
//			m_chessSkillRequirementOne.debugObject->GetTransform()->SetWorldScale({ rookWidth , 0.5f, rookLength });
//			m_chessSkillRequirementTwo.colliderObject->GetComponent<physics::BoxCollider>()->SetHalfExtent({ rookWidth * 0.5f, 0.5f * 0.5f, rookLength * 0.5f });
//			m_chessSkillRequirementTwo.debugObject->GetTransform()->SetWorldScale({ rookWidth , 0.5f, rookLength });
//			SetSkillRequirmentsRotation(m_chessSkillRequirementTwo, Quaternion(Vector3d({ 0, 90, 0 })));
//			break;
//		}
//		case ChessTrapSkillSystem::ChessPieceType::Bishop:
//		{
//			float bishopLength = 5.0f * m_unitLength;
//			float bishopWidth = 1.0f * m_unitLength;
//			m_chessSkillRequirementOne.colliderObject->GetComponent<physics::BoxCollider>()->SetHalfExtent({ bishopWidth * 0.5f, 0.5f * 0.5f, bishopLength * 0.5f });
//			m_chessSkillRequirementOne.debugObject->GetTransform()->SetWorldScale({ bishopWidth , 0.5f, bishopLength });
//			m_chessSkillRequirementTwo.colliderObject->GetComponent<physics::BoxCollider>()->SetHalfExtent({ bishopWidth * 0.5f, 0.5f * 0.5f, bishopLength * 0.5f });
//			m_chessSkillRequirementTwo.debugObject->GetTransform()->SetWorldScale({ bishopWidth , 0.5f, bishopLength });
//			SetSkillRequirmentsRotation(m_chessSkillRequirementOne, Quaternion(Vector3d({ 0, 45, 0 })));
//			SetSkillRequirmentsRotation(m_chessSkillRequirementTwo, Quaternion(Vector3d({ 0, 360 - 45, 0 })));
//			break;
//		}
//		default:
//			break;
//	}
//
//}
//
//void ChessTrapSkillSystem::Update()
//{
//
//}
//
//void ChessTrapSkillSystem::ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition)
//{
//	SetSkillRequirmentsActive(m_chessSkillRequirementOne, true);
//	SetSkillRequirmentsActive(m_chessSkillRequirementTwo, true);
//
//	m_unitDotween->DONothing(m_colliderRemainDuration).OnComplete([=]()
//		{
//			SetSkillRequirmentsActive(m_chessSkillRequirementOne, false);
//			SetSkillRequirmentsActive(m_chessSkillRequirementTwo, false);
//			GetGameObject()->SetSelfActive(false);
//		});
//
//}
//
//void ChessTrapSkillSystem::SetUnitLength(const float p_length)
//{
//	m_unitLength = p_length;
//}
//
//void ChessTrapSkillSystem::SetChessType(ChessPieceType p_type)
//{
//	m_chessPieceType = p_type;
//}
//
//void ChessTrapSkillSystem::SetChessSkillRequirmentOne(GameObject* p_colliderObj, GameObject* p_debugObj)
//{
//	m_chessSkillRequirementOne.colliderObject = p_colliderObj;
//	m_chessSkillRequirementOne.debugObject = p_debugObj;
//}
//
//void ChessTrapSkillSystem::SetChessSkillRequirmentTwo(GameObject* p_colliderObj, GameObject* p_debugObj)
//{
//	m_chessSkillRequirementTwo.colliderObject = p_colliderObj;
//	m_chessSkillRequirementTwo.debugObject = p_debugObj;
//}
//
//void ChessTrapSkillSystem::ActivateSkillOne()
//{
//
//}
