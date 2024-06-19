#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"
//#include "RegionData.h"
class Unit;

/// <summary>
/// Player가 진입했을 경우 wave를 실행시키는 로직을 담는 컴포넌트.
/// </summary>

namespace application::editor
{
    class RegionData;
}
class PlaytimeRegion : public Component, public ContentsObservee
{
public:
    virtual ~PlaytimeRegion();

    // 주인공 일행이 하나라도 지역에 입장할 시에 추가로 호출되는 콜백
    std::vector<std::function<void()>> OnEnter;
    // 주인공 일행이 모두 지역에서 퇴장할 시에 추가로 호출되는 콜백
    std::vector<std::function<void()>> OnLeave;
    //void SetRegionName(std::wstring p_name);
    virtual void Start() override;
    virtual void OnTriggerEnter(physics::Collider* collider) override;
    virtual void OnTriggerExit(physics::Collider* collider) override;
    virtual Component* GetComponent() override { return this; }
    friend application::editor::RegionData;
private:
    application::editor::RegionData* regionData{ nullptr };
    //std::wstring m_regionName;

    bool isOnceActivatedEnter{ false };
    bool isOnceActivatedLeave{ false };
    std::unordered_set<physics::Collider*> enteredPlayerColliders;
    std::vector<std::shared_ptr<application::editor::OrnamentData::DisablingReference>> disablingReferences;
};
