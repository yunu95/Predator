#pragma once
#include "YunutyEngine.h"
#include "DebuggingMesh.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "DebuggingMesh.h"
#include "PermanentObservee.h"

/// <summary>
/// 유닛이 현재 어떤 공격을 받고 있는지, 어떤 상태이상이 적용 중인지를 알려주는 컴포넌트.
/// </summary>
class DebuggingMeshPool : public GameObjectPool<DebuggingMesh>, public SingletonComponent<DebuggingMeshPool>, public Component, public PermanentObservee
{
private:
	virtual void ObjectInitializer(std::weak_ptr<DebuggingMesh> comp) override
	{
		comp.lock()->m_staticMeshRendererComp = AttachDebugMesh(comp.lock()->GetGameObject(), DebugMeshType::Sphere, yunuGI::Color::red(), false);
		comp.lock()->m_staticMeshRendererComp->SetActive(false);
	}
	virtual void Start() override;
	virtual void OnContentsStop() override;
	virtual Component* GetComponent() override { return this; }
};

void DebuggingMeshPool::Start()
{

}

void DebuggingMeshPool::OnContentsStop()
{
	this->SetActive(false);
	poolObjects.clear();
	expendableObjects.clear();
}
