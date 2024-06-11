#pragma once
#include "YunutyEngine.h"
#include "InputManager.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "PermanentObservee.h"
#include "PlayerCharacterType.h"
#include <unordered_map>
#include <memory>
#include <queue>

/// <summary>
/// 전술모드 시스템.
/// 활성화 시 TimeScale(0.0f), queue에 스킬 등록.
/// 확정 시 queue 순서대로 스킬 순차적 사용.
/// 돌진 스킬은 등속 운동... 시간을 어떻게 가져올 것인가 -> 좋은 로직은 시간을 가져오는 것이 아닌, 스킬이 끝날 때 알려주는 것.
/// 투사체 스킬은 던지는 모션이 끝나면 스킬이 끝나는 것으로 간주.
/// </summary>

class RTSCam;
class CursorDetector;
class PlaytimeWave;
class UnitCommand;

class TacticModeSystem : public SingletonComponent<TacticModeSystem>, public Component, public PermanentObservee
{
public:
    virtual void OnEnable() override;
    virtual void Start() override;
    virtual void Update() override;

    virtual void OnContentsStop() override;
    virtual Component* GetComponent() override { return this; }

public:
    // 전술모드가 현재 쿨타임중인지 반환하는 함수
    bool IsCoolTime() { return this->isCoolTime; }
    // 현재 전술모드가 활성화되어 있는지 반환하는 함수 (반환값이 true일 때 만 Command를 Enqueue할 수 있다.)
    bool IsOperation() { return this->isOperating; }
    // 현재 전술모드가 등록된 명령들을 실행중인지 반환하는 함수
    bool IsExecuting() { return this->isExecuting; }

    // 전술모드를 활성화하는 함수입니다.
    void EngageTacticSystem();

    // 유닛의 행동을 큐에 등록할 수 있게 해주는 함수 입니다.
    // 이 함수의 반환값이 false라면 이미 여섯개의 Command가 들어온 상태임. 이 때 UI를 띄어주든 해야할 거 같음.
    // 혹은 전술모드가 활성화되어있지 않기 때문에 false가 나올 것임.
    bool EnqueueCommand(std::shared_ptr<UnitCommand> command);

    // 맨 마지막 명령 하나를 지우는 함수입니다.
    void PopCommand();

    // 명령을 실행하는 도중 플레이어 유닛이 방해받았을 때 부르는 함수입니다.
    void InterruptedCommand(Unit* unit);

    // 모든 명령을 지우는 함수입니다.
    void ClearCommand();

    // 전술 모드가 풀리면 실행될 함수 입니다.
    void ExecuteCommands();

    // 전술모드가 활성화 되어 있을 때 선택된 스킬이 없을 때 임시경로를 보여주는 함수입니다.
    void ShowTemporaryRouteInTacticMode(PlayerCharacterType::Enum playerType);

    // 전술모드가 활성화 되어 있을 때 이동 커맨드가 들어가기 전 경로를 반환해주는 함수 입니다.
    std::vector<Vector3d> GetPathInTacticMode(PlayerCharacterType::Enum playerType, Unit* unit = nullptr);

private:
    // 전술모드 내부에서 등록된 명령들을 실행해주는 함수입니다.
    coroutine::Coroutine ExecuteInternal();

private:
    std::deque<std::shared_ptr<UnitCommand>> commandQueue;
    std::shared_ptr<UnitCommand> robinLastCommand;
    std::shared_ptr<UnitCommand> ursulaLastCommand;
    std::shared_ptr<UnitCommand> hanselLastCommand;

    bool isOperating = false;
    bool isCoolTime = false;
    bool isExecuting = false;
    float coolTime;
    float elapsedTime;

    int commandCount = 0;
    const int MAX_COMMAND_COUNT = 6;

    friend class PlayerController;
    friend class UnitAttackCommand;
};

