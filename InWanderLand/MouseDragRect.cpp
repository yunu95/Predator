#include "MouseDragRect.h"
#include "GameObject.h"
#include "Scene.h"
#include "StaticMeshRenderer.h"
using namespace yunutyEngine;

MouseDragRect* MouseDragRect::PlaceRect(Vector3d position, float width, float height)
{
	auto gameObject = Scene::getCurrentScene()->AddGameObject();
	auto rect = gameObject->AddComponent<MouseDragRect>();
	auto staticMesh = gameObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	rect->m_width = width;
	rect->m_height = height;
	staticMesh->GetGI().LoadMesh("Cube");
	staticMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0,1,0,0 });
	staticMesh->GetGI().GetMaterial()->SetPixelShader(L"DebugPS.cso");
	staticMesh->GetTransform()->SetWorldPosition(position);
	staticMesh->GetTransform()->scale = Vector3d(width, height, 0);
	return rect;
}
