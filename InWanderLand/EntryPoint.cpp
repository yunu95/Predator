#include "InWanderLand.h"
#include "Application.h"
#include "WarriorProductor.h"
#include "MagicianProductor.h"
#include "HealerProductor.h"
#include "DebugMeshes.h"
#include "RTSCam.h"


// Main code
int main(int argc, char** argv)
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	application::Application& client = application::Application::CreateApplication(argc, argv);
	//yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	//auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	//auto sphereMesh = rsrcMgr->GetMesh(L"Sphere");
	//auto mouseCursorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//auto mouseCursorMesh = mouseCursorObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	//mouseCursorMesh->GetGI().SetMesh(sphereMesh);
	//mouseCursorMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 1 });


	//WarriorProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 0.0f));;
	//MagicianProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, -7.0f));;
	//HealerProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 7.0f));

	//auto skillPreviewCubeMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//AttachDebugMesh(skillPreviewCubeMeshObject, DebugMeshType::Cube)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
	//SkillPreviewSystem::SingleInstance().SetPathPreviewObject(skillPreviewCubeMeshObject);

	//auto skillPreviewSphereMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//AttachDebugMesh(skillPreviewSphereMeshObject, DebugMeshType::Sphere)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
	//SkillPreviewSystem::SingleInstance().SetRangePreviewObject(skillPreviewSphereMeshObject);

	//auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	//auto rtsCam = camObj->AddComponent<RTSCam>();
	//rtsCam->groundHoveringClickCallback = [=](Vector3d pos)
	//{
	//	mouseCursorObject->GetTransform()->SetWorldPosition(pos);
	//	SkillPreviewSystem::SingleInstance().SetCurrentMousPosition(pos);
	//};


	client.Initialize();
	client.Run();
	client.Finalize();

	return 0;
}
