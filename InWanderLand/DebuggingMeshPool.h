#pragma once
#include "YunutyEngine.h"
#include "DebuggingMesh.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "DebuggingMesh.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"

/// <summary>
/// 유닛이 현재 어떤 공격을 받고 있는지, 어떤 상태이상이 적용 중인지를 알려주는 컴포넌트.
/// </summary>
class DebuggingMeshPool : public GameObjectPool<DebuggingMesh>, public GHContents::LazySingletonClass<DebuggingMeshPool>
{
	virtual void ObjectInitializer(DebuggingMesh* comp) override
	{
		comp->m_staticMeshRendererComp = AttachDebugMesh(comp->GetGameObject(), DebugMeshType::Sphere, yunuGI::Color::red(), false);
		comp->m_staticMeshRendererComp->SetActive(false);
		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(comp->GetGameObject());

	}
};