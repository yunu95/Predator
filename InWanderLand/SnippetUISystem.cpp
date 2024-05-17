#define NOMINMAX 
#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "RTSCam.h"
#include "DebugMeshes.h"
#include <algorithm>
//#include "YGFactoryAdapter.h"
#include "UIButton.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class UIImageCollider : virtual public Component
{
private:
	float imageWidth;			
	float imageHeight;

	yunuGI::ITexture* m_currentTexture;
	yunuGI::ITexture* m_idleTexture;
	yunuGI::ITexture* m_highLightTexture;

	Vector2d imageLeftTopPos;

	Vector2d centerImagePos;

public:
	yunutyEngine::graphics::UIImage* m_imageComponent;

	void SetImageSize(float p_width, float p_height)
	{
		imageWidth = p_width;
		imageHeight = p_height;
	}
	
	void SetImagePosition(Vector3d p_objectPos)
	{
		imageLeftTopPos = p_objectPos;
	}

	void SetTexture(yunuGI::ITexture* p_idleTexture, yunuGI::ITexture* p_highlightTexture)
	{
		m_idleTexture = p_idleTexture;
		m_highLightTexture = p_highlightTexture;
	}

	virtual void Start() override
	{
		m_currentTexture = m_idleTexture;
		centerImagePos = Vector2d(imageLeftTopPos.x + imageWidth / 2, imageLeftTopPos.y + imageHeight / 2);
	}

	virtual void Update() override
	{
		/// 1900 * 1040
		auto centeredPosition = Input::getMouseScreenPosition();

		if (centeredPosition.x <= centerImagePos.x + imageWidth / 2 && centeredPosition.x >= centerImagePos.x - imageWidth / 2 &&
			centeredPosition.y <= centerImagePos.y + imageHeight / 2 && centeredPosition.y >= centerImagePos.y - imageHeight / 2)
		{
			m_imageComponent->GetGI().SetImage(m_highLightTexture);
		}
		else
		{
			m_imageComponent->GetGI().SetImage(m_idleTexture);
		}
	}
};

void SnippetInitializerUISystem()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	auto sphereMesh = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(L"Sphere");
	auto mouseCursorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto mouseCursorMesh = mouseCursorObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	mouseCursorMesh->GetGI().SetMesh(sphereMesh);
	mouseCursorMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 1 });

	/// ui전용 마우스 커서 콜라이더
	auto mouseColliderObjectForUI = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto mouseColliderForUI = mouseColliderObjectForUI->AddComponent<physics::SphereCollider>();
	mouseColliderForUI->SetRadius(1.0f);
	mouseColliderObjectForUI->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

	auto cam = Scene::getCurrentScene()->AddGameObject()->AddComponent<RTSCam>();
	cam->GetTransform()->SetWorldPosition(Vector3d(0, 30, 0));
	cam->groundHoveringClickCallback = [=](Vector3d pos)
	{
		mouseCursorObject->GetTransform()->SetWorldPosition(pos);
		mouseColliderObjectForUI->GetTransform()->SetWorldPosition(pos + Vector3d(50000, 0, 50000));
	};

	auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	rsrcMgr->LoadFile("Texture/zoro.jpg");
	rsrcMgr->LoadFile("Texture/zoro_highLighted.jpg");
	rsrcMgr->LoadFile("Texture/zoro_Clicked.jpg");
	
	yunuGI::ITexture* uiTexture;
	uiTexture = rsrcMgr->GetTexture(L"Texture/zoro.jpg");
	
	auto uiImageObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto uiImageComponent = uiImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
	uiImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
	uiImageComponent->GetGI().SetLayer(100);
	uiImageObject->GetTransform()->SetLocalScale({ 0.5,1,1 });
	//auto uiButtonComponent = uiImageObject->AddComponent<UIButton>();
	//uiButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
	//uiButtonComponent->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro_highLighted.jpg"));
	//uiButtonComponent->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro_Clicked.jpg"));
	//uiButtonComponent->SetImageComponent(uiImageComponent);
	uiImageObject->GetTransform()->SetLocalPosition({ 0, 255, 0 });

	auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
	directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 10,10,0 }));
}

namespace snippets
{
	TEST_CLASS(SnippetUISystemClass)
	{
	public:
		TEST_METHOD(SnippetUISystem)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerUISystem);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif
