#include "SkillPreviewSystem.h"

SkillPreviewSystem::~SkillPreviewSystem()
{

}

void SkillPreviewSystem::Update()
{
	if (isActivated)
	{
		if (m_pathPreviewObject->GetSelfActive())
		{
			SetPathPreviewObjectRotation();
		}

		if (m_rangePreviewObject->GetSelfActive())
		{
			SetRangePreviewObjectPosition();
		}
	}
}

void SkillPreviewSystem::SetPathPreviewObjectRotation()
{
	m_pathPreviewObject->GetTransform()->SetWorldRotation(Quaternion({ 0,0,0 }));

	Vector3d playerPosition = { m_currentSelectedPlayerObject->GetTransform()->GetWorldPosition() };
	Vector3d distanceVector = m_currentMousePosition - playerPosition;

	float pathMeshHeight = distanceVector.Magnitude();

	Vector3d meshPosition = playerPosition + distanceVector.Normalized() * pathMeshHeight / 2;

	m_pathPreviewObject->GetTransform()->SetWorldPosition(meshPosition);

	Vector3d objectFront = m_pathPreviewObject->GetTransform()->GetWorldRotation().Forward();

	Vector3d betweeenVector = m_currentMousePosition - m_pathPreviewObject->GetTransform()->GetWorldPosition();
	double angle = (Vector3d::Dot(objectFront, betweeenVector));
	double sq = objectFront.Magnitude() * betweeenVector.Magnitude();
	double finalDegree = acos(std::clamp(angle / sq, -1.0, 1.0)) * 57.2969f;
	Vector3d axis = Vector3d::Cross(objectFront, betweeenVector);
	if (axis.y < 0)
		finalDegree *= -1;

	m_pathPreviewObject->GetTransform()->SetWorldRotation(Quaternion({ 0, finalDegree, 0 }));

	m_pathPreviewObject->GetTransform()->SetWorldScale({ 2, 0.3f, pathMeshHeight });
	previousDegree = finalDegree;
}

void SkillPreviewSystem::SetRangePreviewObjectPosition()
{
	m_rangePreviewObject->GetTransform()->SetWorldPosition(m_currentMousePosition);
	m_rangePreviewObject->GetTransform()->SetWorldScale({ 2.0f,2.0f,2.0f });
}

void SkillPreviewSystem::SetPathPreviewObject(GameObject* p_obj)
{
	m_pathPreviewObject = p_obj;
	m_pathMeshComponent = p_obj->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	m_pathPreviewObject->SetSelfActive(false);
}

void SkillPreviewSystem::SetRangePreviewObject(GameObject* p_obj)
{
	m_rangePreviewObject = p_obj;
	m_rangeMeshComponent = p_obj->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	m_rangePreviewObject->SetSelfActive(false);
}

void SkillPreviewSystem::SetCurrentMousPosition(Vector3d pos)
{
	m_currentMousePosition = pos;
}

void SkillPreviewSystem::SetCurrentSelectedPlayerGameObject(GameObject* obj)
{
	m_currentSelectedPlayerObject = obj;
}

void SkillPreviewSystem::SetCurrentSkillPreviewType(SkillPreviewMesh p_type)
{
	m_currentSkillPreviewType = p_type;
}

void SkillPreviewSystem::ActivateSkillPreview(bool p_boolen)
{
	isActivated = p_boolen;

	if (p_boolen)
	{
		switch (m_currentSkillPreviewType)
		{
			case SkillPreviewSystem::SkillPreviewMesh::OnlyPath:
				SetPathPreviewObjectRotation();
				m_pathPreviewObject->SetSelfActive(true);
				break;
			case SkillPreviewSystem::SkillPreviewMesh::OnlyRange:
				SetRangePreviewObjectPosition();
				m_rangePreviewObject->SetSelfActive(true);
				break;
			case SkillPreviewSystem::SkillPreviewMesh::Both:
				SetPathPreviewObjectRotation();
				SetRangePreviewObjectPosition();
				m_pathPreviewObject->SetSelfActive(true);
				m_rangePreviewObject->SetSelfActive(true);
				break;
			case SkillPreviewSystem::SkillPreviewMesh::None:
				m_pathPreviewObject->SetSelfActive(false);
				m_rangePreviewObject->SetSelfActive(false);
				break;
			default:
				break;
		}
	}
	else
	{
		m_pathPreviewObject->SetSelfActive(false);
		m_rangePreviewObject->SetSelfActive(false);
		isOnceRotated = false;
		previousDegree = 0.0f;
	}
}
