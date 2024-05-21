#include "Interactable_ChessBishop.h"

#include "InteractableData.h"
#include "DebugMeshes.h"

void Interactable_ChessBishop::Start()
{
	
}

void Interactable_ChessBishop::Update()
{

}

yunutyEngine::coroutine::Coroutine Interactable_ChessBishop::DoInteraction()
{
	co_return;
}

void Interactable_ChessBishop::SetDataFromEditorData(const application::editor::InteractableData& data)
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
