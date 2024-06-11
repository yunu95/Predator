#include "SkillPreviewSystem.h"

#include "MoveEndPreviewPool.h"
#include "AttackPreviewPool.h"

#include "RTSCam.h"
#include "DirectXMath.h"
#include "UVAnimator.h"
#include "wanderResources.h"


void SkillPreviewSystem::Start()
{
	//this->camObj = GameManager::Instance().rtscam->GetGameObject();
}

void SkillPreviewSystem::ObjectInitializer(std::weak_ptr<graphics::StaticMeshRenderer> comp)
{
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	yunuGI::IShader* vs = _resourceManager->GetShader(L"TextureAnimVS.cso");
	yunuGI::IShader* ps = _resourceManager->GetShader(L"TextureAnimPS.cso");

	yunuGI::ITexture* move = _resourceManager->GetTexture(wanderResources::texture::MOVE_TEXTURE);
	comp.lock()->GetGI().GetMaterial()->SetVertexShader(vs);
	comp.lock()->GetGI().GetMaterial()->SetPixelShader(ps);
	comp.lock()->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, move);

	auto anim = comp.lock()->GetGameObject()->AddComponent<UVAnimator>();
	anim->SetStaticMeshRenderer(comp.lock().get());
	anim->SetDirection(Vector2d{ 1,0 });
}

void SkillPreviewSystem::Init()
{
	MoveEndPreviewPool::SingleInstance();
	AttackPreviewPool::SingleInstance();

	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	yunuGI::IShader* vs = _resourceManager->GetShader(L"TextureVS.cso");
	yunuGI::IShader* ps = _resourceManager->GetShader(L"GuideLinePS.cso");
	yunuGI::IShader* vs1 = _resourceManager->GetShader(L"TextureAnimVS.cso");
	yunuGI::IShader* ps1 = _resourceManager->GetShader(L"TextureAnimPS.cso");
	yunuGI::ITexture* arrowBodyTexture = _resourceManager->GetTexture(wanderResources::texture::ARROW_BODY_TEXTURE);
	yunuGI::ITexture* arrowHeadTexture = _resourceManager->GetTexture(wanderResources::texture::ARROW_HEAD_TEXTURE);
	yunuGI::ITexture* moveTexture = _resourceManager->GetTexture(wanderResources::texture::MOVE_TEXTURE);
	yunuGI::ITexture* maxSkillRangeTexture = _resourceManager->GetTexture(wanderResources::texture::MAX_SKILL_RANGE_TEXTURE);
	yunuGI::ITexture* robinWSkillTexture = _resourceManager->GetTexture(wanderResources::texture::ROBIN_W_TEXTURE);
	yunuGI::ITexture* ursulaQSkillTexture = _resourceManager->GetTexture(wanderResources::texture::URSULA_Q_TEXTURE);
	yunuGI::ITexture* ursulaWSkillTexture = _resourceManager->GetTexture(wanderResources::texture::URSULA_W_TEXTURE);
	yunuGI::ITexture* hanselQSkillTexture = _resourceManager->GetTexture(wanderResources::texture::HANSEL_Q_TEXTURE);

	yunuGI::IMesh* quadMesh = _resourceManager->GetMesh(L"Rectangle");

#pragma region SkillMaxRangeRenderer
	{
		this->skillMaxRangePreviewObj = Scene::getCurrentScene()->AddGameObject();
		this->skillMaxRangePreviewObj->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
		this->skillMaxRangePreviewRenderer = this->skillMaxRangePreviewObj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		this->skillMaxRangePreviewRenderer->GetGI().SetMesh(quadMesh);
		this->skillMaxRangePreviewRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
		this->skillMaxRangePreviewRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
		this->skillMaxRangePreviewRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, maxSkillRangeTexture);
	}
#pragma endregion

#pragma region TemporaryRouteMeshRenderer
	{
		this->temporaryRouteMeshRendererObj = Scene::getCurrentScene()->AddGameObject();
		this->temporaryRouteMeshRenderer = this->temporaryRouteMeshRendererObj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		this->temporaryRouteMeshRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, moveTexture);
		this->temporaryRouteMeshRenderer->GetGI().GetMaterial()->SetVertexShader(vs1);
		this->temporaryRouteMeshRenderer->GetGI().GetMaterial()->SetPixelShader(ps1);
		auto anim = temporaryRouteMeshRendererObj->AddComponent<UVAnimator>();
		anim->SetStaticMeshRenderer(temporaryRouteMeshRenderer);
		anim->SetDirection(Vector2d{ 1,0 });
	}
#pragma endregion

#pragma region RobinQSkillPreview
	{
		this->robinQSkillPreviewObj = Scene::getCurrentScene()->AddGameObject();
		auto arrowPreviewObj = Scene::getCurrentScene()->AddGameObjectFromFBX("Guideline");
		arrowPreviewObj->SetParent(this->robinQSkillPreviewObj);
		auto arrowRenderer = arrowPreviewObj->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		arrowRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
		arrowRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
		arrowRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, arrowHeadTexture);
		arrowRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);

		auto arrowBody = Scene::getCurrentScene()->AddGameObjectFromFBX("Guideline");
		arrowBody->SetParent(this->robinQSkillPreviewObj);
		auto bodyRenderer = arrowBody->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		bodyRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
		bodyRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
		bodyRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, arrowBodyTexture);
		bodyRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);

		this->robinQSkillPreviewObj->SetSelfActive(false);
	}
#pragma endregion 

#pragma region RobinWSkillPreview
	{
		this->robinWSkillPreviewObj = Scene::getCurrentScene()->AddGameObject();
		{
			auto circleOne = Scene::getCurrentScene()->AddGameObject();
			circleOne->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
			circleOne->SetParent(this->robinWSkillPreviewObj);
			auto circleOneRenderer = circleOne->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			circleOneRenderer->GetGI().SetMesh(quadMesh);
			circleOneRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
			circleOneRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
			circleOneRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, robinWSkillTexture);
			circleOneRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);
		}
		this->robinWSkillPreviewObj->SetSelfActive(false);
	}
#pragma endregion 

#pragma region UrsulaQSkillPreview
	{
		this->ursulaQSkillPreviewObj = Scene::getCurrentScene()->AddGameObject();
		{
			auto circleOne = Scene::getCurrentScene()->AddGameObject();
			circleOne->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
			circleOne->SetParent(this->ursulaQSkillPreviewObj);
			auto circleOneRenderer = circleOne->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			circleOneRenderer->GetGI().SetMesh(quadMesh);
			circleOneRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
			circleOneRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
			circleOneRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, ursulaQSkillTexture);
			circleOneRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
		}
		{
			auto circleTwo = Scene::getCurrentScene()->AddGameObject();
			circleTwo->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
			circleTwo->SetParent(this->ursulaQSkillPreviewObj);
			auto circleTwoRenderer = circleTwo->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			circleTwoRenderer->GetGI().SetMesh(quadMesh);
			circleTwoRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
			circleTwoRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
			circleTwoRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, ursulaQSkillTexture);
			circleTwoRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
		}
		{
			auto circleThree = Scene::getCurrentScene()->AddGameObject();
			circleThree->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
			circleThree->SetParent(this->ursulaQSkillPreviewObj);
			auto circleThreeRenderer = circleThree->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			circleThreeRenderer->GetGI().SetMesh(quadMesh);
			circleThreeRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
			circleThreeRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
			circleThreeRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, ursulaQSkillTexture);
			circleThreeRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
		}
		this->ursulaQSkillPreviewObj->SetSelfActive(false);
	}
#pragma endregion

#pragma region UrsulaWSkill
	{
		this->ursulaWSkillPreviewObj = Scene::getCurrentScene()->AddGameObject();
		{
			auto circleOne = Scene::getCurrentScene()->AddGameObject();
			circleOne->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
			circleOne->SetParent(this->ursulaWSkillPreviewObj);
			auto circleOneRenderer = circleOne->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			circleOneRenderer->GetGI().SetMesh(quadMesh);
			circleOneRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
			circleOneRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
			circleOneRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, ursulaWSkillTexture);
			circleOneRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
		}
		this->ursulaWSkillPreviewObj->SetSelfActive(false);
	}
#pragma endregion

#pragma region HanselQSkill
	{
		this->hanselQSkillPreviewObj = Scene::getCurrentScene()->AddGameObject();
		{
			auto circleOne = Scene::getCurrentScene()->AddGameObject();
			circleOne->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
			circleOne->SetParent(this->hanselQSkillPreviewObj);
			auto circleOneRenderer = circleOne->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			circleOneRenderer->GetGI().SetMesh(quadMesh);
			circleOneRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
			circleOneRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
			circleOneRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, hanselQSkillTexture);
			circleOneRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);
		}
		this->hanselQSkillPreviewObj->SetSelfActive(false);
	}
#pragma endregion

#pragma region HanselWSkillPreview
	{
		this->hanselWSkillPreviewObj = Scene::getCurrentScene()->AddGameObject();
		auto arrowPreviewObj = Scene::getCurrentScene()->AddGameObjectFromFBX("Guideline");
		arrowPreviewObj->SetParent(this->hanselWSkillPreviewObj);
		auto arrowRenderer = arrowPreviewObj->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		arrowRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
		arrowRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
		arrowRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, arrowHeadTexture);
		arrowRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);

		auto arrowBody = Scene::getCurrentScene()->AddGameObjectFromFBX("Guideline");
		arrowBody->SetParent(this->hanselWSkillPreviewObj);
		auto bodyRenderer = arrowBody->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		bodyRenderer->GetGI().GetMaterial()->SetVertexShader(vs);
		bodyRenderer->GetGI().GetMaterial()->SetPixelShader(ps);
		bodyRenderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::Temp0, arrowBodyTexture);
		bodyRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);

		this->hanselWSkillPreviewObj->SetSelfActive(false);
	}
	robinQSkillPreviewObj->SetSelfActive(false);
	robinWSkillPreviewObj->SetSelfActive(false);
	ursulaQSkillPreviewObj->SetSelfActive(false);
	ursulaWSkillPreviewObj->SetSelfActive(false);
	hanselQSkillPreviewObj->SetSelfActive(false);
	hanselWSkillPreviewObj->SetSelfActive(false);
	skillMaxRangePreviewObj->SetSelfActive(false);
	temporaryRouteMeshRendererObj->SetSelfActive(false);
#pragma endregion 
}

void SkillPreviewSystem::ShowRobinQSkill(const Vector3d& objectPos)
{
	if (this->robinQSkillPreviewObj->GetActive() == false)
	{
		this->robinQSkillPreviewObj->SetSelfActive(true);
	}

	// 매개변수로 들어온 objectPos로부터 사거리가 표시됩니다.
	// maxDistance로 들어온 값까지 늘어납니다.

	// this->robinQSkillPreviewObj의 위치를 objectPos로
	auto offsetPos = objectPos;
	offsetPos.y += Y_OFFSET;
	this->robinQSkillPreviewObj->GetTransform()->SetLocalPosition(offsetPos);

	auto distToXZPlane = abs(camObj->GetTransform()->GetWorldPosition().y);
	auto centeredPosition = Input::getMouseScreenPositionNormalizedZeroCenter();

	Vector3d mouseWorldPos = camObj->GetComponent<yunutyEngine::graphics::Camera>()->GetProjectedPoint(centeredPosition, distToXZPlane, Vector3d(0, 1, 0));
	Vector3d mouseVector = mouseWorldPos - objectPos;
	mouseWorldPos.y = 0.f;
	auto normalizedPos = mouseVector.Normalize(mouseVector);


	auto basicAxis = AXIS;

	float dotValue = (Vector3d::Dot(basicAxis, normalizedPos));

	float angle = acos(dotValue);
	angle = angle * (180 / yunutyEngine::math::PI);

	if (mouseWorldPos.z - objectPos.z < 0)
	{
		angle = 360.0f - angle;
	}

	this->robinQSkillPreviewObj->GetChildren()[static_cast<int>(RobinQSkillInfo::ArrowHead)]->GetTransform()->SetLocalRotation(Quaternion{ Vector3f{0.f,angle,0.f} });
	this->robinQSkillPreviewObj->GetChildren()[static_cast<int>(RobinQSkillInfo::ArrowBody)]->GetTransform()->SetLocalRotation(Quaternion{ Vector3f{0.f,angle,0.f} });

	float distance = sqrt(pow(objectPos.x - mouseWorldPos.x, 2) + pow(objectPos.y - mouseWorldPos.y, 2) + pow(objectPos.z - mouseWorldPos.z, 2));

	if (distance > OFFSET)
	{
		this->robinQSkillPreviewObj->GetChildren()[static_cast<int>(RobinQSkillInfo::ArrowBody)]->GetTransform()->SetLocalScale(Vector3d{ (distance - OFFSET) / 2 ,1.0,1.0 });

		this->robinQSkillPreviewObj->GetChildren()[static_cast<int>(RobinQSkillInfo::ArrowHead)]->GetTransform()->SetLocalPosition(normalizedPos * (distance - OFFSET));
	}
}

void SkillPreviewSystem::HideRobinQSkill()
{
	if (this->robinQSkillPreviewObj->GetActive() == true)
	{
		this->robinQSkillPreviewObj->SetSelfActive(false);
	}
}

void SkillPreviewSystem::ShowRobinWSkill(const Vector3d& objectPos, float circleRadius)
{
	if (this->robinWSkillPreviewObj->GetActive() == false)
	{
		this->robinWSkillPreviewObj->SetSelfActive(true);
	}

	circleRadius *= 2;

	this->robinWSkillPreviewObj->GetChildren()[static_cast<int>(RobinWSkillInfo::CircleOne)]->GetTransform()->SetLocalPosition(objectPos);
	this->robinWSkillPreviewObj->GetChildren()[static_cast<int>(RobinWSkillInfo::CircleOne)]->GetTransform()->SetLocalScale(Vector3d{ circleRadius,1.0,circleRadius });
}

void SkillPreviewSystem::HideRobinWSkill()
{
	if (this->robinWSkillPreviewObj->GetActive() == true)
	{
		this->robinWSkillPreviewObj->SetSelfActive(false);
	}
}

void SkillPreviewSystem::ShowUrsulaQSkill(Vector3d& circleOnePos, Vector3d& circleTwoPos, Vector3d& circleThreePos, Vector3d circleRadius)
{
	circleOnePos.y += this->Y_OFFSET;
	circleTwoPos.y += this->Y_OFFSET;
	circleThreePos.y += this->Y_OFFSET;

	circleRadius.x *= 2;
	circleRadius.y *= 2;
	circleRadius.z *= 2;

	if (this->ursulaQSkillPreviewObj->GetActive() == false)
	{
		this->ursulaQSkillPreviewObj->SetSelfActive(true);
	}

	// 원 3개 출력
	this->ursulaQSkillPreviewObj->GetChildren()[static_cast<int>(UrsulaQSkillInfo::CircleOne)]->GetTransform()->SetLocalPosition(circleOnePos);
	this->ursulaQSkillPreviewObj->GetChildren()[static_cast<int>(UrsulaQSkillInfo::CircleOne)]->GetTransform()->SetLocalScale(Vector3d{ circleRadius.x,circleRadius.x,1.f });

	this->ursulaQSkillPreviewObj->GetChildren()[static_cast<int>(UrsulaQSkillInfo::CircleTwo)]->GetTransform()->SetLocalPosition(circleTwoPos);
	this->ursulaQSkillPreviewObj->GetChildren()[static_cast<int>(UrsulaQSkillInfo::CircleTwo)]->GetTransform()->SetLocalScale(Vector3d{ circleRadius.y,circleRadius.y,1.f });

	this->ursulaQSkillPreviewObj->GetChildren()[static_cast<int>(UrsulaQSkillInfo::CircleThree)]->GetTransform()->SetLocalPosition(circleThreePos);
	this->ursulaQSkillPreviewObj->GetChildren()[static_cast<int>(UrsulaQSkillInfo::CircleThree)]->GetTransform()->SetLocalScale(Vector3d{ circleRadius.z,circleRadius.z,1.f });
}

void SkillPreviewSystem::HideUrsulaQSkill()
{
	if (this->ursulaQSkillPreviewObj->GetActive() == true)
	{
		this->ursulaQSkillPreviewObj->SetSelfActive(false);
	}
}

void SkillPreviewSystem::ShowUrsulaWSkill(Vector3d circlePos, float circleRadius)
{
	circlePos.y += this->Y_OFFSET;
	circleRadius *= 2;
	if (this->ursulaWSkillPreviewObj->GetActive() == false)
	{
		this->ursulaWSkillPreviewObj->SetSelfActive(true);
	}

	ursulaWSkillPreviewObj->GetChildren()[static_cast<int>(UrsulaWSkillInfo::CircleOne)]->GetTransform()->SetLocalPosition(circlePos);
	ursulaWSkillPreviewObj->GetChildren()[static_cast<int>(UrsulaWSkillInfo::CircleOne)]->GetTransform()->SetLocalScale(Vector3d{ circleRadius,circleRadius, 1.f });
}

void SkillPreviewSystem::HideUrsulaWSkill()
{
	if (this->ursulaWSkillPreviewObj->GetActive() == true)
	{
		this->ursulaWSkillPreviewObj->SetSelfActive(false);
	}
}

void SkillPreviewSystem::ShowHanselQSkill(Vector3d circlePos, float circleRadius)
{
	circlePos.y += this->Y_OFFSET;
	circleRadius *= 2;

	if (this->hanselQSkillPreviewObj->GetActive() == false)
	{
		this->hanselQSkillPreviewObj->SetSelfActive(true);
	}

	// 원 출력
	this->hanselQSkillPreviewObj->GetChildren()[static_cast<int>(HanselQSkillInfo::CircleOne)]->GetTransform()->SetLocalPosition(circlePos);
	this->hanselQSkillPreviewObj->GetChildren()[static_cast<int>(HanselQSkillInfo::CircleOne)]->GetTransform()->SetLocalScale(Vector3d{ circleRadius,circleRadius,1.f });
}

void SkillPreviewSystem::HideHanselQSkill()
{
	if (this->hanselQSkillPreviewObj->GetActive() == true)
	{
		this->hanselQSkillPreviewObj->SetSelfActive(false);
	}
}

void SkillPreviewSystem::ShowHanselWSkill(const Vector3d& objectPos)
{
	if (this->hanselWSkillPreviewObj->GetActive() == false)
	{
		this->hanselWSkillPreviewObj->SetSelfActive(true);
	}

	// 매개변수로 들어온 objectPos로부터 사거리가 표시됩니다.
	// maxDistance로 들어온 값까지 늘어납니다.

	// this->robinQSkillPreviewObj의 위치를 objectPos로
	auto offsetPos = objectPos;
	offsetPos.y += Y_OFFSET;
	this->hanselWSkillPreviewObj->GetTransform()->SetLocalPosition(offsetPos);

	auto distToXZPlane = abs(camObj->GetTransform()->GetWorldPosition().y);
	auto centeredPosition = Input::getMouseScreenPositionNormalizedZeroCenter();

	Vector3d mouseWorldPos = camObj->GetComponent<yunutyEngine::graphics::Camera>()->GetProjectedPoint(centeredPosition, distToXZPlane, Vector3d(0, 1, 0));
	Vector3d mouseVector = mouseWorldPos - objectPos;
	mouseWorldPos.y = 0.f;
	auto normalizedPos = mouseVector.Normalize(mouseVector);


	auto basicAxis = AXIS;

	float dotValue = (Vector3d::Dot(basicAxis, normalizedPos));

	float angle = acos(dotValue);
	angle = angle * (180 / yunutyEngine::math::PI);

	if (mouseWorldPos.z - objectPos.z < 0)
	{
		angle = 360.0f - angle;
	}

	this->hanselWSkillPreviewObj->GetChildren()[static_cast<int>(HanselWSkillInfo::ArrowHead)]->GetTransform()->SetLocalRotation(Quaternion{ Vector3f{0.f,angle,0.f} });
	this->hanselWSkillPreviewObj->GetChildren()[static_cast<int>(HanselWSkillInfo::ArrowBody)]->GetTransform()->SetLocalRotation(Quaternion{ Vector3f{0.f,angle,0.f} });

	float distance = sqrt(pow(objectPos.x - mouseWorldPos.x, 2) + pow(objectPos.y - mouseWorldPos.y, 2) + pow(objectPos.z - mouseWorldPos.z, 2));

	if (distance > OFFSET)
	{
		this->hanselWSkillPreviewObj->GetChildren()[static_cast<int>(HanselWSkillInfo::ArrowBody)]->GetTransform()->SetLocalScale(Vector3d{ (distance - OFFSET) / 2 ,1.0,1.0 });

		this->hanselWSkillPreviewObj->GetChildren()[static_cast<int>(HanselWSkillInfo::ArrowHead)]->GetTransform()->SetLocalPosition(normalizedPos * (distance - OFFSET));
	}
}

void SkillPreviewSystem::HideHanselWSkill()
{
	if (this->hanselWSkillPreviewObj->GetActive() == true)
	{
		this->hanselWSkillPreviewObj->SetSelfActive(false);
	}
}

void SkillPreviewSystem::ShowTemporaryRoute(UnitType unitType, std::vector<Vector3d>& vertexList)
{
	if (temporaryRouteMeshRendererObj->GetActive() == false)
	{
		temporaryRouteMeshRendererObj->SetSelfActive(true);
	}
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	// 버텍스가 1개만 들어오는 일은 없을 것으로 예상 혹시모르니 return
	if (vertexList.size() < 2)
	{
		return;
	}
	// 이 함수는 계속 불릴 함수이기 때문에 이미 메쉬가 생성된 것이 있다면 삭제해줘야함
	if (temporaryRouteMesh)
	{
		_resourceManager->DeleteMesh(temporaryRouteMesh);
		temporaryRouteMesh = nullptr;
	}
	temporaryRouteMesh = CreateRouteMesh(vertexList);

	this->temporaryRouteMeshRenderer->GetGI().SetMesh(temporaryRouteMesh);
	switch (unitType)
	{
		case SkillPreviewSystem::UnitType::Robin:
		{
			this->temporaryRouteMeshRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Ursula:
		{
			this->temporaryRouteMeshRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Hansel:
		{
			this->temporaryRouteMeshRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);
		}
		break;
		default:
		{}
		break;
	}

}

void SkillPreviewSystem::HideTemporaryRoute()
{
	if (temporaryRouteMeshRendererObj->GetActive() == true)
	{
		temporaryRouteMeshRendererObj->SetSelfActive(false);
	}
}

yunuGI::IMesh* SkillPreviewSystem::ShowRoute(UnitType unitType, std::vector<Vector3d>& vertexList)
{
	auto tempDirection = vertexList.back() - vertexList[vertexList.size() - 2];
	tempDirection = Vector3d::Normalize(tempDirection);

	auto mesh = CreateRouteMesh(vertexList);
	auto renderer = this->Borrow();
	renderer.lock()->GetGI().SetMesh(mesh);

	this->rendererMap.insert({ mesh, renderer.lock().get() });


	this->ShowMoveEndImage(unitType, vertexList.back(), mesh, tempDirection);


	switch (unitType)
	{
		case SkillPreviewSystem::UnitType::Robin:
		{
			renderer.lock()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Ursula:
		{
			renderer.lock()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Hansel:
		{
			renderer.lock()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);
		}
		break;
		default:
		{}
		break;
	}

	return mesh;
}
void SkillPreviewSystem::DeleteRouteMesh(yunuGI::IMesh* mesh)
{
	// 생성된 메쉬를 지우며 renderer가 붙은 게임 오브젝트를 Pool에 반납
	auto iter = this->rendererMap.find(mesh);
	if (iter != this->rendererMap.end())
	{
		iter->second->GetGI().SetMesh(nullptr);
		this->Return(iter->second->GetWeakPtr<graphics::StaticMeshRenderer>());
		this->rendererMap.erase(iter);

		// 이동 끝 이미지도 풀에 반납
		this->HideShowMoveEndImage(mesh);


		const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		_resourceManager->DeleteMesh(mesh);
	}
}

void SkillPreviewSystem::ShowSkillMaxRange(UnitType unitType, Vector3d pos, float maxRange)
{
	maxRange *= 2;
	if (this->skillMaxRangePreviewObj->GetActive() == false)
	{
		this->skillMaxRangePreviewObj->SetSelfActive(true);
	}

	auto tempPos = pos;
	tempPos.y += Y_OFFSET;
	this->skillMaxRangePreviewObj->GetTransform()->SetLocalPosition(tempPos);
	this->skillMaxRangePreviewObj->GetTransform()->SetLocalScale(Vector3d{ maxRange,maxRange,1 });
	switch (unitType)
	{
		case SkillPreviewSystem::UnitType::Robin:
		{
			skillMaxRangePreviewRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Ursula:
		{
			skillMaxRangePreviewRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Hansel:
		{
			skillMaxRangePreviewRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);
		}
		break;
		default:
		{}
		break;
	}
}

void SkillPreviewSystem::HideSkillMaxRange()
{
	if (this->skillMaxRangePreviewObj->GetActive() == true)
	{
		this->skillMaxRangePreviewObj->SetSelfActive(false);
	}
}

yunutyEngine::graphics::StaticMeshRenderer* SkillPreviewSystem::ShowAttackImage(UnitType unitType, Vector3d pos)
{
	auto attackRenderer = AttackPreviewPool::SingleInstance().Borrow();
	this->attackRendererSet.insert({ attackRenderer.lock().get() });
	attackRenderer.lock()->GetGameObject()->GetTransform()->SetLocalPosition(pos);
	switch (unitType)
	{
		case SkillPreviewSystem::UnitType::Robin:
		{
			attackRenderer.lock()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Ursula:
		{
			attackRenderer.lock()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Hansel:
		{
			attackRenderer.lock()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);
		}
		break;
		default:
		{}
		break;
	}
	return attackRenderer.lock().get();
}

void SkillPreviewSystem::HideAttackImage(yunutyEngine::graphics::StaticMeshRenderer* renderer)
{
	auto iter2 = this->attackRendererSet.find(renderer);
	AttackPreviewPool::SingleInstance().Return((*iter2)->GetWeakPtr<graphics::StaticMeshRenderer>());
	this->attackRendererSet.erase(iter2);
}

void SkillPreviewSystem::ShowMoveEndImage(UnitType unitType, Vector3d pos, yunuGI::IMesh* mesh, Vector3d direction)
{
	auto moveEndRenderer = MoveEndPreviewPool::SingleInstance().Borrow();
	this->moveEndRendererMap.insert({ mesh, moveEndRenderer.lock().get() });
	pos.y = this->Y_OFFSET;
	moveEndRenderer.lock()->GetGameObject()->GetTransform()->SetLocalPosition(pos);
	moveEndRenderer.lock()->GetGameObject()->GetTransform()->SetLocalScale(Vector3d{ 2,2,1 });
	auto euler = moveEndRenderer.lock()->GetGameObject()->GetTransform()->GetLocalRotation().Euler();
	auto dotProduct = std::clamp(Vector3d::Dot(Vector3d{ 0, 0, 1 }, direction), -1.0, 1.0);
	auto angle = std::acos(dotProduct) * (180.0 / yunutyEngine::math::PI);

	// 외적 계산
	auto crossProduct = Vector3d::Cross(Vector3d{ 0, 0, 1 }, direction);

	// 반대 방향 회전 문제 해결
	if (dotProduct > 0.9999) 
	{
		// 방향이 거의 동일함
		angle = 0;
	}
	else if (dotProduct < -0.9999)
	{
		// 방향이 정반대임
		angle = 180.0;
	}
	else 
	{
		// 일반적인 경우, crossProduct의 y 성분으로 회전 방향 결정
		if (crossProduct.y < 0)
		{
			angle = -angle;
		}
	}
	euler.y = angle;
	moveEndRenderer.lock()->GetGameObject()->GetTransform()->SetLocalRotation(Quaternion{ euler });
	switch (unitType)
	{
		case SkillPreviewSystem::UnitType::Robin:
		{
			moveEndRenderer.lock()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Ursula:
		{
			moveEndRenderer.lock()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
		}
		break;
		case SkillPreviewSystem::UnitType::Hansel:
		{
			moveEndRenderer.lock()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);
		}
		break;
		default:
		{}
		break;
	}
}

void SkillPreviewSystem::HideShowMoveEndImage(yunuGI::IMesh* mesh)
{
	auto iter2 = this->moveEndRendererMap.find(mesh);
	MoveEndPreviewPool::SingleInstance().Return(iter2->second->GetWeakPtr<graphics::StaticMeshRenderer>());
	this->moveEndRendererMap.erase(iter2);
}

yunuGI::IMesh* SkillPreviewSystem::CreateRouteMesh(std::vector<Vector3d>& vertexList)
{
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	unsigned int vertexCount = vertexList.size();
	unsigned int meshVertexCount = vertexCount * 2;

	Vector3d direction = Vector3d{ 0,0,0 };
	Vector3d prevDirection = Vector3d{ 0,0,0 };
	std::vector<yunuGI::Vector3> posVec;
	std::vector<unsigned int> idxVec;
	std::vector<yunuGI::Vector2> uvVec;
	std::vector<yunuGI::Vector3> normalVec;


	for (int i = 0; i < vertexCount - 1; i++)
	{
		if (i == 0)
		{
			// 처음 버텍스 일 때
			direction = vertexList[i + 1] - vertexList[i];
			direction.y = 0;
			Vector3d crossVector = Vector3d{ -direction.z,0,direction.x };
			crossVector = crossVector.Normalize(crossVector);

			yunuGI::Vector3 vertexPos = vertexList[i];
			yunuGI::Vector3 vertexPos2 = vertexList[i];
			yunuGI::Vector3 vertexPos3 = vertexList[i + 1];
			yunuGI::Vector3 vertexPos4 = vertexList[i + 1];
			vertexPos += (crossVector * VERTEX_OFFSET);
			vertexPos.y += Y_OFFSET;

			vertexPos2 += (-crossVector * VERTEX_OFFSET);
			vertexPos2.y += Y_OFFSET;

			vertexPos3 += (crossVector * VERTEX_OFFSET);
			vertexPos3.y += Y_OFFSET;

			vertexPos4 += (-crossVector * VERTEX_OFFSET);
			vertexPos4.y += Y_OFFSET;

			posVec.push_back(vertexPos);
			posVec.push_back(vertexPos2);
			posVec.push_back(vertexPos3);
			posVec.push_back(vertexPos4);
			normalVec.emplace_back(yunuGI::Vector3{ 0,1,0 });
			normalVec.emplace_back(yunuGI::Vector3{ 0,1,0 });
			normalVec.emplace_back(yunuGI::Vector3{ 0,1,0 });
			normalVec.emplace_back(yunuGI::Vector3{ 0,1,0 });
			idxVec.emplace_back(i * 2);
			idxVec.emplace_back((i * 2) + 2);
			idxVec.emplace_back((i * 2) + 1);
			idxVec.emplace_back((i * 2) + 2);
			idxVec.emplace_back((i * 2) + 3);
			idxVec.emplace_back((i * 2) + 1);
		}
		else
		{
			// 중간 버텍스 일 때
			// 우선 끝 두 점을 구하고 인덱스 버퍼를 갱신한다.
			direction = vertexList[i + 1] - vertexList[i];
			direction = direction.Normalize(direction);
			direction.y = 0;
			Vector3d crossVector = Vector3d{ -direction.z,0,direction.x };
			crossVector = crossVector.Normalize(crossVector);

			yunuGI::Vector3 vertexPos = vertexList[i + 1];
			yunuGI::Vector3 vertexPos2 = vertexList[i + 1];

			vertexPos += (crossVector * VERTEX_OFFSET);
			vertexPos.y += Y_OFFSET;

			vertexPos2 += (-crossVector * VERTEX_OFFSET);
			vertexPos2.y += Y_OFFSET;

			posVec.push_back(vertexPos);
			posVec.push_back(vertexPos2);
			normalVec.emplace_back(yunuGI::Vector3{ 0,1,0 });
			normalVec.emplace_back(yunuGI::Vector3{ 0,1,0 });

			idxVec.emplace_back(i * 2);
			idxVec.emplace_back((i * 2) + 2);
			idxVec.emplace_back((i * 2) + 1);
			idxVec.emplace_back((i * 2) + 2);
			idxVec.emplace_back((i * 2) + 3);
			idxVec.emplace_back((i * 2) + 1);

			// posVec의 마직막 요소 2개의 포지션을 현재 방향에 맞게 수정
			prevDirection = vertexList[i] - vertexList[i - 1];
			prevDirection = prevDirection.Normalize(prevDirection);

			float dotValue = Vector3d::Dot(direction, prevDirection);

			float angle = acos(dotValue);
			angle /= 2;
			//angle = angle * (180 / yunutyEngine::math::PI);

			auto& prevPos = posVec[posVec.size() - 4];
			auto& prevPos2 = posVec[posVec.size() - 3];
			auto center = (prevPos + prevPos2) * 0.5;
			prevPos -= center;
			prevPos2 -= center;

			float cosTheta = cos(angle);
			float sinTheta = sin(angle);

			float v1x = prevPos.x;
			float v1z = prevPos.z;
			prevPos.x = v1x * cosTheta + v1z * sinTheta;
			prevPos.z = -v1x * sinTheta + v1z * cosTheta;

			float v2x = prevPos2.x;
			float v2z = prevPos2.z;
			prevPos2.x = v2x * cosTheta + v2z * sinTheta;
			prevPos2.z = -v2x * sinTheta + v2z * cosTheta;

			prevPos += center;
			prevPos2 += center;
		}
	}

	uvVec.resize(posVec.size());

	float outDistance = 0.f;
	float inDistance = 0.f;
	for (int i = 0; i < (vertexList.size() * 2) - 2; i += 2)
	{
		// uv setting용 반복문
		outDistance += sqrt(pow(posVec[i].x - posVec[i + 2].x, 2) + pow(posVec[i].z - posVec[i + 2].z, 2));
		inDistance += sqrt(pow(posVec[i + 1].x - posVec[i + 3].x, 2) + pow(posVec[i + 1].z - posVec[i + 3].z, 2));

		if (i == 0)
		{
			uvVec[i] = yunuGI::Vector2{ 0,0 };
			uvVec[i + 2] = yunuGI::Vector2{ outDistance,0 };

			uvVec[i + 1] = yunuGI::Vector2{ 0,1 };
			uvVec[i + 3] = yunuGI::Vector2{ inDistance,1 };
		}
		else
		{
			uvVec[i + 2].x = outDistance;
			uvVec[i + 2].y = 0;

			uvVec[(i + 2) + 1].x = inDistance;
			uvVec[(i + 2) + 1].y = 1;
		}
	}

	for (auto& each : posVec)
	{
		each.y = Y_OFFSET;
	}

	std::wstring meshName = L"RouteMesh_";
	meshName += std::to_wstring(routeMeshID);
	auto tempMesh = _resourceManager->CreateMesh(meshName, posVec, idxVec, normalVec, uvVec);
	routeMeshID++;
	return tempMesh;
}
