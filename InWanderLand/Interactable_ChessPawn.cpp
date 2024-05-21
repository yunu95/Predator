#include "Interactable_ChessPawn.h"

#include "InteractableData.h"
#include "DebugMeshes.h"

void Interactable_ChessPawn::Start()
{

}

void Interactable_ChessPawn::Update()
{
}

yunutyEngine::coroutine::Coroutine Interactable_ChessPawn::DoInteraction()
{
	co_return;
}

void Interactable_ChessPawn::SetDataFromEditorData(const application::editor::InteractableData& data)
{
	initPos.x = data.pod.position.x;
	initPos.y = data.pod.position.y;
	initPos.z = data.pod.position.z;
	initRotation.w = data.pod.rotation.w;
	initRotation.x = data.pod.rotation.x;
	initRotation.y = data.pod.rotation.y;
	initRotation.z = data.pod.rotation.z;
	initScale.x = data.pod.scale.x;
	initScale.y = data.pod.scale.y;
	initScale.z = data.pod.scale.z;
}
