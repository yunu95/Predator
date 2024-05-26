#pragma once
#include "YunutyEngine.h"
#include <map>
#include "PermanentObservee.h"

// 플레이어 유닛들의 행동을 관리하는 클래스
// 짬통 중의 짬통이다.
class RTSCam;
class PlayerUnit;
class CursorDetector;

class PlayerController : public SingletonComponent<PlayerController>, public Component, public PermanentObservee
{
    enum CharacterType { ROBIN, URSULA, HANSEL, CHARACTER_NUM };
    Vector3d cameraOffset = { 0, 20, -15 };
    float cameraMoveDuration{ 0.3f };
    void Select(CharacterType charType);
public:
    virtual Component* GetComponent() override { return this; }
    virtual void OnContentsStop() override;
    CursorDetector* m_cursorDetector;
    // 아래 둘은 한번 호출한 후 내용을 다 지워버리는 휘발성 콜백들
    VolatileCallbacks onRobinQSelect;
    VolatileCallbacks onRobinQActivate;
    void UsePrimarySkill(const Vector3d& pos);
    void UseSecondarySkill(const Vector3d& pos);
private:
    template<CharacterType character>
    Skill& PrimarySkill(const Vector3d& pos);
    template<CharacterType character>
    Skill& SecondarySkill(const Vector3d& pos);
    std::weak_ptr<Unit> characters[CHARACTER_NUM];
    //std::weak_ptr<const Unit> characters[CHARACTER_NUM] robin, ursula, hansel;
    int a = ROBIN;
};
