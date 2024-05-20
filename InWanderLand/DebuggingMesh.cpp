#include "DebuggingMesh.h"
#include "Unit.h"
#include "Dotween.h"
#include "DebuggingMeshPool.h"
#include "StaticMeshRenderer.h"


float y = 0.f;

void DebuggingMesh::SetUnitObject(Unit* p_unit)
{
	m_ownerObject = p_unit->GetGameObject();
}

void DebuggingMesh::PopMeshUP(yunuGI::Color p_color, MaterialNum p_matNum)
{

	m_floatingElapsed = 0.0f;
	m_Yincreasement = 0.0f;
	m_staticMeshRendererComp = GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

	m_staticMeshRendererComp->GetGI().SetMaterial(0, GetColoredDebugMaterial(p_color, false));

	GetGameObject()->GetTransform()->SetWorldPosition(m_ownerObject->GetTransform()->GetWorldPosition());

	m_staticMeshRendererComp->SetActive(true);
	isPopStarted = true;
}

void DebuggingMesh::Start()
{
	y = GetGameObject()->GetTransform()->GetLocalPosition().y;
}

void DebuggingMesh::Update()
{
	if (isPopStarted)
	{
		m_floatingElapsed += Time::GetDeltaTime();
		
		m_Yincreasement += m_floatingPerFrame;

		GetGameObject()->GetTransform()->SetWorldPosition(m_ownerObject->GetTransform()->GetWorldPosition()
			+ Vector3d(0, m_Yincreasement, 0));

		y = GetGameObject()->GetTransform()->GetLocalPosition().y;

		if (m_floatingTime <= m_floatingElapsed)
		{
			m_floatingElapsed = 0.0f;
			m_Yincreasement = 0.0f;
			isPopStarted = false;
			m_staticMeshRendererComp->SetActive(false);
			DebuggingMeshPool::Instance().Return(this);
		}
	}
}
