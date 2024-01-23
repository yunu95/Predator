#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "DebugMeshes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class ObjectPatrolMove : public Component
{
public:
	float t;
	float y = 0;
	float a;
	virtual void Update() override
	{
		t += Time::GetDeltaTime();
		GetGameObject()->GetTransform()->position = { a * sin(t * 10), y, 0 };
	}
};

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetDebugMeshScaleCheckInit()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
	
	auto testObject1 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto patrol1 = testObject1->AddComponent<ObjectPatrolMove>();
	patrol1->a = 3;
	auto object1DebugObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	auto testObject2 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto patrol2 = testObject2->AddComponent<ObjectPatrolMove>();
	patrol2->a = -3;
	auto object2DebugObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	
	auto obj1Collider = testObject1->AddComponent<physics::SphereCollider>();
	obj1Collider->SetRadius(2.0f);
	object1DebugObj->GetTransform()->scale = { 2.0f, 2.0f, 2.0f };
	testObject1->AddComponent<physics::RigidBody>();
	auto obj1DebugMesh = AttachDebugMesh(object1DebugObj, DebugMeshType::Sphere, yunuGI::Color::red(), false);
	object1DebugObj->SetParent(testObject1);


	auto obj2Collider = testObject2->AddComponent<physics::SphereCollider>();
	obj2Collider->SetRadius(3.0f);
	object2DebugObj->GetTransform()->scale = { 5.0f, 5.0f, 5.0f };
	testObject2->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	auto obj2DebugMesh = AttachDebugMesh(object2DebugObj, DebugMeshType::Sphere, yunuGI::Color::green(), false);
	object2DebugObj->SetParent(testObject2);

	testObject1->GetTransform()->SetWorldPosition({-5, 0, 0});
	testObject2->GetTransform()->SetWorldPosition({5, 0, 0});

	

	auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
	directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::Camera>();

	camObj->GetTransform()->SetWorldPosition({ 0,0,-25 });

	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{
	TEST_CLASS(InWanderLand)
	{
	public:
		// 테스트 함수의 이름이 Snippet으로 시작하는 테스트들은 빌드의 성공 여부 판단에 쓰이지 않습니다.
		// RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
		// 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
		TEST_METHOD(SnippetDebugMeshScaleCheck)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::contents::ContentsLayer::AssignTestInitializer(SnippetDebugMeshScaleCheckInit);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif