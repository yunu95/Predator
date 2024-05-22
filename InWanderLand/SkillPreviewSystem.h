#pragma once
#include "YunutyEngine.h"
#include "Unit.h"

class SkillPreviewSystem : public Component, public SingletonComponent<SkillPreviewSystem>, public GameObjectPool<graphics::StaticMeshRenderer>
{
private:
	enum class RobinQSkillInfo
	{
		ArrowHead,
		ArrowBody
	};
	enum class UrsulaQSkillInfo
	{
		ArrowHead,
		ArrowBody,
		CircleOne,
		CircleTwo,
		CircleThree,
	};
	enum class UrsulaWSkillInfo
	{
		CircleOne,
	};
	enum class HanselQSkillInfo
	{
		ArrowHead,
		ArrowBody,
		CircleOne,
	};
	enum class HanselWSkillInfo
	{
		ArrowHead,
		ArrowBody
	};
public:
	enum class UnitType
	{
		Robin,
		Ursula,
		Hansel
	};
public:
	virtual void ObjectInitializer(graphics::StaticMeshRenderer* comp) override;

	void Init();
#pragma region RobinQSkill
	// objectPos는 사거리 표시의 시작위치, maxDistance는 스킬 최대 사거리 입니다.
	void ShowRobinQSkill(const Vector3d& objectPos, float maxDistance);
	void HideRobinQSkill();
#pragma endregion

#pragma region UrsulaQSkill
	// objectPos는 사거리 표시의 시작위치, maxDistance는 스킬 최대 사거리, circle의 pos들은 원의 위치들, circleRadius가 Vector3d인 이유는 원 3개의 반지름을 다양하게 설정할 수 있도록 만들었습니다.
	void ShowUrsulaQSkill(const Vector3d& objectPos, float maxDistance, const Vector3d& circleOnePos, const Vector3d& circleTwoPos, const Vector3d& circleThreePos, Vector3d circleRadius);
	void HideUrsulaQSkill();
#pragma endregion

#pragma region UrsulaQSkill
	// 우르술라의 W스킬은 objectPos, 스킬 사거리, 원의 반지름을 입력해주면 알아서 렌더링되게 만들었습니다.
	void ShowUrsulaWSkill(const Vector3d& objectPos ,float maxDistance, float circleRadius);
	void HideUrsulaWSkill();
#pragma endregion

#pragma region HanselQSkill
	// 헨젤의 Q 스킬은 원의 위치는 마우스의 위치로 고정되어 있어 원의 위치는 받지 않고 반지름만 받게 만들었습니다.
	void ShowHanselQSkill(const Vector3d& objectPos, float maxDistance, float circleRadius);
	void HideHanselQSkill();
#pragma endregion

#pragma region HanselWSkill
	// objectPos는 사거리 표시의 시작위치, maxDistance는 스킬 최대 사거리 입니다.
	void ShowHanselWSkill(const Vector3d& objectPos, float maxDistance);
	void HideHanselWSkill();
#pragma endregion


#pragma region Route
	// 매 프레임 불리는 함수 마우스를 따라다니며 임시 경로를 보여줍니다.
	void ShowTemporaryRoute(UnitType unitType , std::vector<Vector3d>& vertexList);
	// 이동 할 경로가 확정되면 불러 줄 함수 삭제 요청이 들어오기 전까지 화면에 보여집니다.
	yunuGI::IMesh* ShowRoute(UnitType unitType, std::vector<Vector3d>& vertexList);
	// 생성된 경로 메쉬를 삭제해줄 함수
	void DeleteRouteMesh(yunuGI::IMesh* mesh);
#pragma endregion

private:
	yunuGI::IMesh* CreateRouteMesh(std::vector<Vector3d>& vertexList);

public:
	yunutyEngine::GameObject* camObj;


private:
	yunutyEngine::GameObject* robinQSkillPreviewObj;

	yunutyEngine::GameObject* ursulaQSkillPreviewObj;
	yunutyEngine::GameObject* ursulaWSkillPreviewObj;

	yunutyEngine::GameObject* hanselQSkillPreviewObj;
	yunutyEngine::GameObject* hanselWSkillPreviewObj;

	// 매 프레임 메쉬가 바뀔 때 바뀌는 메쉬를 보여주는 변수들
	yunutyEngine::GameObject* temporaryRouteMeshRendererObj;
	yunutyEngine::graphics::StaticMeshRenderer* temporaryRouteMeshRenderer;
	yunuGI::IMesh* temporaryRouteMesh;

	// 생성된 메쉬랑 맵핑된 renderer를 보관하는 컨테이너
	std::map<yunuGI::IMesh*, graphics::StaticMeshRenderer*> rendererMap;
	unsigned int routeMeshID = 0;
};

