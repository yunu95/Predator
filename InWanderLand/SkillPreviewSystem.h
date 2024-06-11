#pragma once
#include "YunutyEngine.h"
#include "Unit.h"

class SkillPreviewSystem : public Component, public SingletonComponent<SkillPreviewSystem>, public GameObjectPool<graphics::StaticMeshRenderer>,
    public PermanentObservee
{
private:
    enum class RobinQSkillInfo
    {
        ArrowHead,
        ArrowBody
    };
    enum class RobinWSkillInfo
    {
        CircleOne
    };
    enum class UrsulaQSkillInfo
    {
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
    virtual void Start() override;
    virtual Component* GetComponent() override { return this; };
public:
    virtual void ObjectInitializer(std::weak_ptr<graphics::StaticMeshRenderer> comp) override;

    void Init();
#pragma region RobinQSkill
    // objectPos는 사거리 표시의 시작위치 즉, 유닛의 월드포지션입니다.
    void ShowRobinQSkill(const Vector3d& objectPos);
    void HideRobinQSkill();
#pragma endregion

#pragma region RobinWSkill
    // objectPos는 유닛의 현재 월드 포지션, circleRadius는 스킬 범위 입니다.
    void ShowRobinWSkill(const Vector3d& objectPos, float circleRadius);
    void HideRobinWSkill();
#pragma endregion

#pragma region UrsulaQSkill
    // 각각 원 3개의 월드 포지션값을 받으며 마지막 circleRadius는 x,y,z순으로 circleOne,circleTwo,circleThree의 크기입니다.
    void ShowUrsulaQSkill(Vector3d& circleOnePos, Vector3d& circleTwoPos, Vector3d& circleThreePos, Vector3d circleRadius);
    void HideUrsulaQSkill();
#pragma endregion

#pragma region UrsulaWSkill
    // 원 하나의 월드포지션 값을 받습니다.
    void ShowUrsulaWSkill(Vector3d circlePos, float circleRadius);
    void HideUrsulaWSkill();
#pragma endregion

#pragma region HanselQSkill
    // 원 하나의 월드포지션 값을 받습니다.
    void ShowHanselQSkill(Vector3d circlePos, float circleRadius);
    void HideHanselQSkill();
#pragma endregion

#pragma region HanselWSkill
    // objectPos는 사거리 표시의 시작위치 즉, 유닛의 월드포지션입니다.
    void ShowHanselWSkill(const Vector3d& objectPos);
    void HideHanselWSkill();
#pragma endregion


#pragma region Route
    // 매 프레임 불리는 함수 마우스를 따라다니며 임시 경로를 보여줍니다.
    void ShowTemporaryRoute(UnitType unitType, std::vector<Vector3d>& vertexList);
    void HideTemporaryRoute();
    // 이동 할 경로가 확정되면 불러 줄 함수 삭제 요청이 들어오기 전까지 화면에 보여집니다.
    // 이동경로의 마지막 지점에 이동 끝 이미지까지 보여줍니다.
    yunuGI::IMesh* ShowRoute(UnitType unitType, std::vector<Vector3d>& vertexList);
    // 생성된 경로 메쉬를 삭제해줄 함수
    void DeleteRouteMesh(yunuGI::IMesh* mesh);
#pragma endregion

#pragma region SkillMaxRange
    // 스킬을 눌렀을 때 유닛의 포지션에 나오는 스킬 최대 사거리를 보여주는 원형 이미지입니다.
    // 로빈, 우르술라, 헨젤이 모두 이 함수를 부르면 됩니다.
    // pos는 유닛의 월드포지션입니다.
    void ShowSkillMaxRange(UnitType unitType, Vector3d pos, float maxRange);
    void HideSkillMaxRange();
#pragma endregion

#pragma region AttackImage
    // Pos는 평타치는 월드 포지션입니다.
    yunutyEngine::graphics::StaticMeshRenderer* ShowAttackImage(UnitType unitType, Vector3d pos);
    void HideAttackImage(yunutyEngine::graphics::StaticMeshRenderer* renderer);
#pragma endregion

private:
#pragma region MoveEnd
    void ShowMoveEndImage(UnitType unitType, Vector3d pos, yunuGI::IMesh* mesh, Vector3d direction);
    void HideShowMoveEndImage(yunuGI::IMesh* mesh);
#pragma endregion

private:
    yunuGI::IMesh* CreateRouteMesh(std::vector<Vector3d>& vertexList);

public:
    yunutyEngine::GameObject* camObj;

private:
    yunutyEngine::GameObject* robinQSkillPreviewObj;
    yunutyEngine::GameObject* robinWSkillPreviewObj;

    yunutyEngine::GameObject* ursulaQSkillPreviewObj;
    yunutyEngine::GameObject* ursulaWSkillPreviewObj;

    yunutyEngine::GameObject* hanselQSkillPreviewObj;
    yunutyEngine::GameObject* hanselWSkillPreviewObj;

    yunutyEngine::GameObject* skillMaxRangePreviewObj;
    yunutyEngine::graphics::StaticMeshRenderer* skillMaxRangePreviewRenderer;

    // 매 프레임 메쉬가 바뀔 때 바뀌는 메쉬를 보여주는 변수들
    yunutyEngine::GameObject* temporaryRouteMeshRendererObj;
    yunutyEngine::graphics::StaticMeshRenderer* temporaryRouteMeshRenderer;
    yunuGI::IMesh* temporaryRouteMesh;

    // 생성된 메쉬랑 맵핑된 renderer를 보관하는 컨테이너
    std::map<yunuGI::IMesh*, graphics::StaticMeshRenderer*> rendererMap;
    // 이동 끝 메쉬를 비활성화하기위한 컨테이너 키값으로 생성된 경로 메쉬를 사용한다.
    std::map<yunuGI::IMesh*, graphics::StaticMeshRenderer*> moveEndRendererMap;
    // 평타 이미지를 지우기 위한 컨테이너
    std::set<yunutyEngine::graphics::StaticMeshRenderer*> attackRendererSet;
    unsigned int routeMeshID = 0;

    static constexpr float OFFSET = 1.414;
    const Vector3d AXIS = { -1,0,0 };
    static constexpr float Y_OFFSET = 0.1;
    static constexpr float VERTEX_OFFSET = 0.1;
};

