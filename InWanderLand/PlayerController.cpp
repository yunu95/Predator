#include "InWanderLand.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "SkillPreviewSystem.h"
#include "UIManager.h"
#include "SkillList.h"
#include "VFXAnimator.h"
#include "TacticModeSystem.h"
#include "UnitMoveCommand.h"
#include "UnitAttackCommand.h"
#include "UnitSkillCommand.h"

const std::unordered_map<UIEnumID, SkillUpgradeType::Enum> PlayerController::skillUpgradeByUI
{
    {UIEnumID::SkillUpgradeButtonRobin00,SkillUpgradeType::ROBIN_PASSIVE_LONGER},
    {UIEnumID::SkillUpgradeButtonRobin11,SkillUpgradeType::ROBIN_Q_RANGE},
    {UIEnumID::SkillUpgradeButtonRobin12,SkillUpgradeType::ROBIN_Q_DAMAGE},
    {UIEnumID::SkillUpgradeButtonRobin21,SkillUpgradeType::ROBIN_W_DAMAGE},
    {UIEnumID::SkillUpgradeButtonRobin22,SkillUpgradeType::ROBIN_W_RADIUS},
    {UIEnumID::SkillUpgradeButtonUrsula00,SkillUpgradeType::URSULA_PASSIVE_ENHANCE},
    {UIEnumID::SkillUpgradeButtonUrsula11,SkillUpgradeType::URSULA_Q_DAMAGE},
    {UIEnumID::SkillUpgradeButtonUrsula12,SkillUpgradeType::URSULA_Q_RANGE},
    {UIEnumID::SkillUpgradeButtonUrsula21,SkillUpgradeType::URSULA_W_DAMAGE},
    {UIEnumID::SkillUpgradeButtonUrsula22,SkillUpgradeType::URSULA_W_RADIUS},
    {UIEnumID::SkillUpgradeButtonHansel00,SkillUpgradeType::HANSEL_PASSIVE_ENHANCE},
    {UIEnumID::SkillUpgradeButtonHansel11,SkillUpgradeType::HANSEL_Q_DAMAGE},
    {UIEnumID::SkillUpgradeButtonHansel12,SkillUpgradeType::HANSEL_Q_RANGE},
    {UIEnumID::SkillUpgradeButtonHansel21,SkillUpgradeType::HANSEL_W_RANGE},
    {UIEnumID::SkillUpgradeButtonHansel22,SkillUpgradeType::HANSEL_W_MORE_HITS},
};
void PlayerController::RegisterUnit(std::weak_ptr<Unit> unit)
{
    auto playerType = unit.lock()->GetUnitTemplateData().pod.playerUnitType.enumValue;
    if (playerType == PlayerCharacterType::None)
        return;

    characters[unit.lock()->GetUnitTemplateData().pod.playerUnitType.enumValue] = unit;
    unit.lock()->onStateEngage[UnitBehaviourTree::Death].AddCallback([this, unit]() { UnSelectSkill(unit); });
    unit.lock()->onStateEngage[UnitBehaviourTree::Death].AddCallback(std::bind(&PlayerController::OnPlayerChracterDead,
        this,
        unit));
    unit.lock()->onStateEngage[UnitBehaviourTree::Paralysis].AddCallback([this, unit]() { UnSelectSkill(unit); });
    unit.lock()->onSkillActivation.AddCallback(std::bind(&PlayerController::OnPlayerUnitSkillActivation,
        this,
        unit,
        std::placeholders::_1));
    unit.lock()->onSkillExpiration.AddCallback(std::bind(&PlayerController::OnPlayerUnitSkillExpiration,
        this,
        unit,
        std::placeholders::_1));

    unit.lock()->OnStateEngageCallback()[UnitBehaviourTree::Keywords::Knockback].AddCallback([=]() {
        TacticModeSystem::Instance().InterruptedCommand(unit.lock().get());
        });

    unit.lock()->OnStateEngageCallback()[UnitBehaviourTree::Keywords::Death].AddCallback([=]() {
        TacticModeSystem::Instance().InterruptedCommand(unit.lock().get());
        });
}

void PlayerController::SetSkillUpgradeTarget(UIEnumID skillUpgradeUITarget)
{
    this->skillUpgradeUITarget = skillUpgradeUITarget;
}
bool PlayerController::IsSkillUpgraded(SkillUpgradeType::Enum id)
{
    return skillUpgraded[id];
}
bool PlayerController::IsSkillUpgraded(UIEnumID skillUpgradeUITarget)
{
    return skillUpgraded.at(skillUpgradeByUI.at(skillUpgradeUITarget));
}
void PlayerController::UpgradeSkill()
{
    SetSkillPoints(skillPointsLeft - 1);
    static constexpr float gray = 0.3f;
    // AdjustUpgradeButtonsByState
    skillUpgraded[skillUpgradeByUI.at(skillUpgradeUITarget)] = true;
    for (auto each : skillUpgradeByUI)
    {
        UIElement* uiElement = UIManager::Instance().GetUIElementByEnum(each.first);
        SkillUpgradeType::Enum dependentSkillUpgrade = SkillUpgradeType::NONE;
        if (UIElement* dependent = UIManager::Instance().GetUIElementWithIndex(uiElement->importedUIData.dependentUpgrade))
        {
            dependentSkillUpgrade = skillUpgradeByUI.at((UIEnumID)dependent->importedUIData.enumID);
        }
        if (skillUpgraded[each.second])
        {
            // 스킬이 이미 업그레이드된 경우
            uiElement->GetLocalUIsByEnumID().at(UIEnumID::SkillUpgradeButton_UpgradedImage)->EnableElement();
        }
        else if (dependentSkillUpgrade == SkillUpgradeType::NONE || skillUpgraded[dependentSkillUpgrade])
        {
            // 의존하는 스킬이 업그레이드된 경우
            uiElement->GetLocalUIsByEnumID().at(UIEnumID::SkillUpgradeButton_Upgradable)->EnableElement();
        }
        else
        {
            // 아예 업그레이드를 할 수 없는 경우
            uiElement->GetLocalUIsByEnumID().at(UIEnumID::SkillUpgradeButton_InUpgradableImage)->EnableElement();
        }
    }
}
void PlayerController::SetSkillPoints(int points)
{
    skillPointsLeft = points;
    UIManager::Instance().GetUIElementByEnum(UIEnumID::SkillPoint_Number)->SetNumber(skillPointsLeft);
    if (skillPointsLeft > 0)
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::InGame_SkiltreeMenu_Active)->EnableElement();
    }
    else
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::InGame_SkiltreeMenu_InActive)->EnableElement();
    }

}
int PlayerController::GetSkillPoints()
{
    return skillPointsLeft;
}
void PlayerController::IncrementSkillPoint()
{
    SetSkillPoints(skillPointsLeft + 1);
}

void PlayerController::UnconstrainCamUpdateDirection()
{
    isConstraingCamUpdateDirection = false;
}

void PlayerController::ConstrainCamUpdateDirection(const Vector3d& direction)
{
    camContrainingDirection = direction.Normalized();
    isConstraingCamUpdateDirection = true;
}

void PlayerController::LockCamInRegion(const application::editor::RegionData* camLockRegion)
{
    this->camLockRegion = camLockRegion;
}

void PlayerController::UnlockCamFromRegion()
{
    this->camLockRegion = nullptr;
}

bool PlayerController::CanUnitSelectSkill(std::weak_ptr<Unit> unit)
{
    return (!unit.lock()->referenceParalysis.BeingReferenced() && unit.lock()->IsAlive());
}

void PlayerController::Start()
{
}
void PlayerController::OnContentsPlay()
{
    SetActive(true);
    cursorUnitDetector = Scene::getCurrentScene()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionBoxCollider>();
    AttachDebugMesh(cursorUnitDetector.lock()->GetGameObject(), DebugMeshType::Cube, yunuGI::Color::white());
    SetSkillPoints(0);
    SetManaFull();
    SetState(State::Peace);
    InitUnitMouseInteractionEffects();

    SetCameraOffset();
    SelectPlayerUnit(PlayerCharacterType::Robin);
}
void PlayerController::OnContentsStop()
{
    selectedCharacterType = PlayerCharacterType::None;
    stateRequestedByAction = State::None;
    SetState(State::Peace);
    SetActive(false);
    UnlockCamFromRegion();
    Unit::SetPauseAll(false);
    Scene::getCurrentScene()->DestroyGameObject(cursorUnitDetector.lock()->GetGameObject());
    currentCombo = 0;
    isConstraingCamUpdateDirection = false;
}

void PlayerController::Update()
{
    cursorUnitDetector.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetWorldCursorPosition());
    HandleInput();
    HandleState();
    HandleCamera();
    HandleSkillPreview();
    HandleByState();
    HandleSkillCooltime();
    HandleManaRegen();
    HandleMouseHover();
    HandleUnitPickingCollider();
    HandleComboState();
    static yunutyEngine::graphics::UIText* text_State{ nullptr };
    if (text_State == nullptr)
    {
        text_State = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::UIText>();
        text_State->GetGI().SetFontSize(30);
        text_State->GetGI().SetColor(yunuGI::Color{ 1,1,1,1 });
        text_State->GetTransform()->SetLocalScale(Vector3d{ 1200,500,0 });
        text_State->GetTransform()->SetLocalPosition(Vector3d{ 0,260,0 });
    }
    if (auto cursorOnUnit = GetUnitOnCursor())
    {
        Unit::debuggingUnit = cursorOnUnit;
    }
    if (Unit::debuggingUnit)
    {
        selectedDebugCharacter = Unit::debuggingUnit->GetWeakPtr<Unit>();
        wstringstream wsstream;
        wsstream << L"\ndebug unit name: " << yutility::GetWString(Unit::debuggingUnit->name);
        wsstream << L"unitState : ";
        auto& activeStates = selectedDebugCharacter.lock()->GetBehaviourTree().GetActiveNodes();
        for (const auto& each : activeStates)
        {
            wsstream << wstring(L"[") + yutility::GetWString(POD_Enum<UnitBehaviourTree::Keywords>::GetEnumNameMap().at(each->GetNodeKey())) + wstring(L"]");
        }
        wsstream << L"\nacq : " << selectedDebugCharacter.lock()->acquisitionRange.lock()->GetEnemies().size();
        wsstream << L"\ncurrent pos : " << selectedDebugCharacter.lock()->GetTransform()->GetWorldPosition();
        wsstream << L"\nattack target pos : " << selectedDebugCharacter.lock()->attackMoveDestination;
        wsstream << L"\nnavObstacle : " << selectedDebugCharacter.lock()->referenceEnableNavObstacle.BeingReferenced();
        wsstream << L"\nnavAgent : " << selectedDebugCharacter.lock()->navAgentComponent.lock()->GetTransform()->GetWorldPosition();
        wsstream << L"\nnavAgent pos : " << !selectedDebugCharacter.lock()->referenceDisableNavAgent.BeingReferenced();
        wsstream << L"\nblockFollowingNavAgent : " << selectedDebugCharacter.lock()->referenceBlockFollowingNavAgent.BeingReferenced();
        if (auto target = Unit::debuggingUnit->currentTargetUnit.lock())
        {
            wsstream << L"\nattack target unit : " << yutility::GetWString(target->name);
        }

        text_State->GetGI().SetText(wsstream.str());
    }
    text_State->SetActive(Unit::debuggingUnit && DebugGraphic::AreDebugGraphicsEnabled());
}

void PlayerController::HandleByState()
{
    switch (state)
    {
    case State::Peace:
    {
        const auto& gc = GlobalConstant::GetSingletonInstance().pod;
        Vector3d forward = { std::cos(selectedCharacter.lock()->desiredRotation * math::Deg2Rad) , 0, std::sin(selectedCharacter.lock()->desiredRotation * math::Deg2Rad) };
        Vector3d right = { std::cos((selectedCharacter.lock()->desiredRotation - 90) * math::Deg2Rad) , 0, std::sin((selectedCharacter.lock()->desiredRotation - 90) * math::Deg2Rad) };
        peaceFollowingDestination[0] = selectedCharacter.lock()->GetTransform()->GetWorldPosition();
        peaceFollowingDestination[0] += forward * gc.peaceFollowingZOffest;
        peaceFollowingDestination[0] += right * gc.peaceFollowingXOffest;
        peaceFollowingDestination[1] = selectedCharacter.lock()->GetTransform()->GetWorldPosition();
        peaceFollowingDestination[1] += forward * gc.peaceFollowingZOffest * 1.2f;
        peaceFollowingDestination[1] -= right * gc.peaceFollowingXOffest * 0.8f;

        if (std::max(peaceFollowingUnits[0].lock()->DistanceSquare(peaceFollowingDestination[0]),
            peaceFollowingUnits[1].lock()->DistanceSquare(peaceFollowingDestination[1])) >
            std::max(peaceFollowingUnits[0].lock()->DistanceSquare(peaceFollowingDestination[1]),
                peaceFollowingUnits[1].lock()->DistanceSquare(peaceFollowingDestination[0])))
        {
            std::swap(peaceFollowingDestination[0], peaceFollowingDestination[1]);
        }
        peaceFollowingUnits[0].lock()->OrderMove(peaceFollowingDestination[0]);
        peaceFollowingUnits[1].lock()->OrderMove(peaceFollowingDestination[1]);
        break;
    }
    }
}

void PlayerController::HandleInput()
{
    if (state == State::Cinematic) return;
    if (Input::isKeyPushed(KeyCode::Space))
    {
        TryTogglingTacticMode();
    }

    // 전술모드의 마지막 명령을 지우는 키
    if (Input::isKeyPushed(KeyCode::R))
    {
        TacticModeSystem::Instance().PopCommand();
    }

    //if (Input::isKeyPushed(KeyCode::ESC) && TacticModeSystem::Instance().IsOperation() && !TacticModeSystem::Instance().IsExecuting())
    //{
    //    TacticModeSystem::Instance().ClearCommand();
    //}

    if ((TacticModeSystem::Instance().IsExecuting() == false))
    {
        if (Input::isKeyPushed(KeyCode::Q))
        {
            switch (selectedCharacterType)
            {
            case PlayerCharacterType::Robin: SelectSkill(SkillType::ROBIN_Q); break;
            case PlayerCharacterType::Ursula: SelectSkill(SkillType::URSULA_Q); break;
            case PlayerCharacterType::Hansel: SelectSkill(SkillType::HANSEL_Q); break;
            }
        }
        if (Input::isKeyPushed(KeyCode::W))
        {
            switch (selectedCharacterType)
            {
            case PlayerCharacterType::Robin: SelectSkill(SkillType::ROBIN_W); break;
            case PlayerCharacterType::Ursula: SelectSkill(SkillType::URSULA_W); break;
            case PlayerCharacterType::Hansel: SelectSkill(SkillType::HANSEL_W); break;
            }
        }
        if (Input::isKeyPushed(KeyCode::NUM_1))
        {
            SelectPlayerUnit(PlayerCharacterType::Robin);
        }
        if (Input::isKeyPushed(KeyCode::NUM_2))
        {
            SelectPlayerUnit(PlayerCharacterType::Ursula);
        }
        if (Input::isKeyPushed(KeyCode::NUM_3))
        {
            SelectPlayerUnit(PlayerCharacterType::Hansel);
        }
        if (Input::isKeyPushed(KeyCode::A))
        {
            OrderAttackMove(GetWorldCursorPosition());
        }
        if (Input::isKeyPushed(KeyCode::MouseLeftClick) && !UIManager::Instance().IsMouseOnButton())
        {
            OnLeftClick();
        }
        if (Input::isKeyPushed(KeyCode::MouseRightClick))
        {
            OnRightClick();
        }
    }
}

void PlayerController::HandleState()
{
    ///
// 액션쪽에서 활성화한 스테이트가 있는지 판단하는 부분
    //this->isStateAction = false;
    /*if (this->isStateAction)
    {
        return;
    }*/
    //
    if (application::CinematicManager::Instance().IsCinematicMode())
    {
        SetState(State::Cinematic);
        return;
    }
    if ((TacticModeSystem::Instance().IsOperation() == true) || (TacticModeSystem::Instance().IsExecuting() == true))
    {
        this->SetState(State::Tactic);
        return;
    }

    /// Battle 과 Peace 는 Request 요청을 우선시합니다.
    switch (stateRequestedByAction)
    {
    case PlayerController::State::Battle:
    {
        this->SetState(State::Battle);
        return;
    }
    case PlayerController::State::Peace:
    {
        this->SetState(State::Peace);
        return;
    }
    default:
        break;
    }

    if (!PlaytimeWave::GetCurrentOperatingWave().expired())
    {
        //if (PlaytimeWave::GetCurrentOperatingWave().lock()->IsRemainEnemyAndWave())
        //{
        this->SetState(State::Battle);
        //}
        //else
        //{
        //this->SetState(State::Peace);
        //}
    }
    else
    {
        this->SetState(State::Peace);
    }
}

void PlayerController::HandleCamera()
{
    static constexpr float tacticZoomoutDistanceFactor = 1.2f;
    // 영웅이 선택되어 있고, 카메라가 선택된 영웅을 따라가는 경우 targetPos는 영웅의 위치로 설정됩니다.
    Vector3d targetPos;
    if (TacticModeSystem::Instance().IsOperation() && !TacticModeSystem::Instance().IsExecuting())
    {
        if (tacticCameraRef)
        {
            RTSCam::Instance().SetIdealPosition(tacticCameraRef->GetTransform()->GetWorldPosition());
            RTSCam::Instance().SetIdealRotation(tacticCameraRef->GetTransform()->GetWorldRotation());
        }
    }
    else
    {
        if (!selectedCharacter.expired())
        {
            camPivotPoint = selectedCharacter.lock()->GetTransform()->GetWorldPosition();
            zoomMultiplierByNonSelection.reset();
        }
        else
        {
            if (!zoomMultiplierByNonSelection)
            {
                zoomMultiplierByNonSelection = camZoomMultiplier.AcquireFactor();
                *zoomMultiplierByNonSelection = GlobalConstant::GetSingletonInstance().pod.tacticZoomMultiplier;
            }
            camPivotPoint = GetMiddlePoint();
        }
        // 카메라가 지역 제한에 걸렸을 경우, targetPos를 지역 안으로 정의합니다.
        if (camLockRegion)
        {
            camPivotPoint.x = std::clamp(camPivotPoint.x, camLockRegion->pod.x - camLockRegion->pod.width * 0.5, camLockRegion->pod.x + camLockRegion->pod.width * 0.5);
            camPivotPoint.z = std::clamp(camPivotPoint.z, camLockRegion->pod.z - camLockRegion->pod.height * 0.5, camLockRegion->pod.z + camLockRegion->pod.height * 0.5);
        }
        if (isConstraingCamUpdateDirection)
        {
            camPivotPoint = camPreviousPivotPoint + std::fmaxf(0, Vector3d::Dot(camPivotPoint - camPreviousPivotPoint, camContrainingDirection)) * camContrainingDirection;
        }
        targetPos = camPivotPoint + camOffsetNorm * camZoomFactor * camZoomMultiplier;
        camPreviousPivotPoint = camPivotPoint;
        RTSCam::Instance().SetIdealPosition(targetPos);
        RTSCam::Instance().SetIdealRotation(camRotation);
    }
}

void PlayerController::HandleSkillPreview()
{
    if (state != State::Tactic)
    {
        switch (selectedSkill)
        {
        case SkillType::ROBIN_Q:
            SkillPreviewSystem::Instance().ShowRobinQSkill(characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition());
            SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Robin, characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition(), RobinChargeSkill::GetMaxDistance());
            break;
        case SkillType::URSULA_Q:
        {
            UrsulaBlindSkill::UpdatePosition(characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), GetWorldCursorPosition());
            auto pos1 = UrsulaBlindSkill::GetSkillObjectPos_Left(GetWorldCursorPosition());
            auto pos2 = UrsulaBlindSkill::GetSkillObjectPos_Right(GetWorldCursorPosition());
            auto pos3 = UrsulaBlindSkill::GetSkillObjectPos_Top(GetWorldCursorPosition());
            SkillPreviewSystem::Instance().ShowUrsulaQSkill(pos1, pos2, pos3, Vector3d::one * UrsulaBlindSkill::pod.skillRadius);
            SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), UrsulaBlindSkill::GetSkillRange());
            break;
        }
        case SkillType::URSULA_W:
            SkillPreviewSystem::Instance().ShowUrsulaWSkill(GetWorldCursorPosition(), UrsulaParalysisSkill::GetSkillRadius());
            SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), UrsulaParalysisSkill::pod.skillRange);
            break;
        case SkillType::HANSEL_Q:
            SkillPreviewSystem::Instance().ShowHanselQSkill(GetWorldCursorPosition(), HanselChargeSkill::pod.skillRadius);
            SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition(), HanselChargeSkill::GetMaxRange());
            break;
        case SkillType::HANSEL_W:
            SkillPreviewSystem::Instance().ShowHanselWSkill(characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition());
            SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition(), HanselProjectileSkill::GetMaxRange());
            break;
        }
    }
    else
    {
        if (auto unit = selectedCharacter.lock(); unit && unit->IsTacTicReady())
        {
            TacticModeSystem::Instance().ShowSkillPreviewInTacticMode(selectedSkill);
        }
        // 임시 이동 경로 보여주는 부분
        if (selectedSkill == SkillType::NONE)
        {
            if (auto unit = selectedCharacter.lock(); unit && unit->IsTacTicReady())
            {
                TacticModeSystem::Instance().ShowTemporaryRouteInTacticMode(this->selectedCharacterType);
            }
        }
    }
    // 스킬 버튼 위에 마우스 커서가 올라갔을 때 표시할 스킬 프리뷰
    if (selectedSkill == SkillType::NONE)
    {
        int skillButtonIDHoveringPending;
        int parentEnumIDHoveringPending;
        if (auto btn = UIManager::Instance().GetHighlightedButton())
        {
            auto uiElement = btn->GetUIElement().lock();
            skillButtonIDHoveringPending = uiElement->importedUIData.enumID;
            parentEnumIDHoveringPending = (int)uiElement->GetDuplicateParentEnumID();
        }
        else
        {
            parentEnumIDHoveringPending = (int)UIEnumID::None;
            skillButtonIDHoveringPending = (int)UIEnumID::None;
        }
        if (skillButtonIDHovering != skillButtonIDHoveringPending || parentEnumIDHovering != parentEnumIDHoveringPending)
        {
            skillButtonIDHovering = skillButtonIDHoveringPending;
            parentEnumIDHovering = parentEnumIDHoveringPending;

            SkillPreviewSystem::Instance().HideRobinQSkill();
            SkillPreviewSystem::Instance().HideRobinWSkill();
            SkillPreviewSystem::Instance().HideUrsulaQSkill();
            SkillPreviewSystem::Instance().HideUrsulaWSkill();
            SkillPreviewSystem::Instance().HideHanselQSkill();
            SkillPreviewSystem::Instance().HideHanselWSkill();
            SkillPreviewSystem::Instance().HideSkillMaxRange();
        }

        switch (skillButtonIDHovering)
        {
        case (int)UIEnumID::CharInfo_Skill_Use_Q:
            switch (parentEnumIDHovering)
            {
            case (int)UIEnumID::CharInfo_Robin:
            case (int)UIEnumID::CharInfo_Robin_Left:
            {
                auto pos{ characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition() };
                auto forward{ characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldRotation().Forward() };
                SkillPreviewSystem::Instance().ShowRobinQSkill(pos, pos + forward * RobinChargeSkill::GetMaxDistance());
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Robin, characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition(), RobinChargeSkill::GetMaxDistance());
                SetPendingManaCost(RobinChargeSkill::pod.cost);
                break;
            }
            case (int)UIEnumID::CharInfo_Ursula:
            case (int)UIEnumID::CharInfo_Ursula_Left:
            {
                auto pos{ characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition() };
                //auto forward{ characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldRotation().Forward() };
                static constexpr float epsilon = 0.0001f;
                UrsulaBlindSkill::UpdatePosition(pos + epsilon * Vector3d::one, pos);
                auto pos1 = UrsulaBlindSkill::GetSkillObjectPos_Left(pos);
                auto pos2 = UrsulaBlindSkill::GetSkillObjectPos_Right(pos);
                auto pos3 = UrsulaBlindSkill::GetSkillObjectPos_Top(pos);
                SkillPreviewSystem::Instance().ShowUrsulaQSkill(pos1, pos2, pos3, Vector3d::one * UrsulaBlindSkill::pod.skillRadius);
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), UrsulaBlindSkill::GetSkillRange());
                SetPendingManaCost(UrsulaBlindSkill::pod.skillCost);
                break;
            }
            case (int)UIEnumID::CharInfo_Hansel:
            case (int)UIEnumID::CharInfo_Hansel_Left:
            {
                auto pos{ characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition() };
                SkillPreviewSystem::Instance().ShowHanselQSkill(pos, HanselChargeSkill::pod.skillRadius);
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition(), HanselChargeSkill::GetMaxRange());
                SetPendingManaCost(HanselChargeSkill::pod.skillCost);
                break;
            }
            }
            break;
        case (int)UIEnumID::CharInfo_Skill_Use_W:
            switch (parentEnumIDHovering)
            {
            case (int)UIEnumID::CharInfo_Robin:
            case (int)UIEnumID::CharInfo_Robin_Left:
            {
                auto pos{ characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition() };
                SkillPreviewSystem::Instance().ShowRobinWSkill(pos, RobinTauntSkill::GetSkillRadius());
                SetPendingManaCost(RobinTauntSkill::pod.skillCost);
                break;
            }
            case (int)UIEnumID::CharInfo_Ursula:
            case (int)UIEnumID::CharInfo_Ursula_Left:
            {
                auto pos{ characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition() };
                SkillPreviewSystem::Instance().ShowUrsulaWSkill(pos, UrsulaParalysisSkill::GetSkillRadius());
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), UrsulaParalysisSkill::pod.skillRange);
                SetPendingManaCost(UrsulaParalysisSkill::pod.skillCost);
                break;
            }
            case (int)UIEnumID::CharInfo_Hansel:
            case (int)UIEnumID::CharInfo_Hansel_Left:
            {
                auto pos{ characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition() };
                auto forward{ characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldRotation().Forward() };
                SkillPreviewSystem::Instance().ShowHanselWSkill(pos, pos + forward * HanselProjectileSkill::GetMaxRange());
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition(), HanselProjectileSkill::GetMaxRange());
                SetPendingManaCost(HanselProjectileSkill::pod.skillCost);
                break;
            }
            }

            break;
        default:
            SetPendingManaCost(0);
            break;
        }
    }
}

void PlayerController::HandleSkillCooltime()
{
    if (state == State::Tactic)
    {
        return;
    }

    for (int skillType = SkillType::ROBIN_Q; skillType <= SkillType::HANSEL_W; skillType++)
    {
        SetCooltime((SkillType::Enum)skillType, skillCooltimeLeft[skillType] - Time::GetDeltaTime());
    }
}

void PlayerController::HandleManaRegen()
{
    if (state != State::Tactic)
    {
        if (state == State::Peace)
        {
            SetMana(mana + GlobalConstant::GetSingletonInstance().pod.manaRegen_Peace * Time::GetDeltaTime());
        }
        else
        {
            SetMana(mana + GlobalConstant::GetSingletonInstance().pod.manaRegen * Time::GetDeltaTime());
        }
    }
}

void PlayerController::HandleMouseHover()
{
    if (auto unit = GetUnitOnCursor(); unit && state != State::Cinematic)
    {
        ApplyHoverEffect(unit->GetWeakPtr<Unit>());
        if (auto btn = UIManager::Instance().GetHighlightedButton(); btn && btn->IsFunctioningButton())
        {
            UIManager::Instance().GetUIElementByEnum(UIEnumID::MouseCursor_OnButton)->EnableElement();
        }
        else if (!UIManager::Instance().IsMouseOnButton())
        {
            if (unit->GetTeamIndex() == playerTeamIndex)
            {
                UIManager::Instance().GetUIElementByEnum(UIEnumID::MouseCursor_OnAlly)->EnableElement();
            }
            else
            {
                UIManager::Instance().GetUIElementByEnum(UIEnumID::MouseCursor_OnEnemy)->EnableElement();
            }
        }
        unitHoverOutlineGuard = unit->referenceHoverOutline.Acquire();
    }
    else
    {
        DisableHoverEffect();
        if (auto btn = UIManager::Instance().GetHighlightedButton(); btn && btn->IsFunctioningButton())
        {
            UIManager::Instance().GetUIElementByEnum(UIEnumID::MouseCursor_OnButton)->EnableElement();
        }
        else
        {
            UIManager::Instance().GetUIElementByEnum(UIEnumID::MouseCursor_Free)->EnableElement();
        }
        unitHoverOutlineGuard.reset();
    }
}
// 카메라의 near plane으로부터 far plane까지 뻗는 직선의 형태로
// 셀렉션 박스의 트랜스폼을 변경합니다.
// screenSpacePos는 x,y좌표가 -0.5에서 0.5 사이의 값을 가지는 정규화된 스크린좌표입니다.
// 화면의 중심이 {0,0} 상단이 0.5, 하단이 -0.5인 좌단이 -0.5, 우단이 0.5입니다.
void PlayerController::HandleUnitPickingCollider()
{
    auto cam = graphics::Camera::GetMainCamera();

    Input::getMouseScreenPositionNormalizedZeroCenter();
    auto nearPoint = cam->GetTransform()->GetWorldPosition() - cam->GetTransform()->GetWorldRotation().Forward() * cam->GetGI().GetNear();
    auto farPoint = cam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), 2000);
    auto transform = cursorUnitDetector.lock()->GetTransform();
    transform->SetLocalScale({ 0.1,0.1, (nearPoint - farPoint).Magnitude() });
    transform->SetWorldPosition((nearPoint + farPoint) / 2.0);
    transform->SetWorldRotation(Quaternion::MakeWithForwardUp(farPoint - nearPoint, cam->GetTransform()->GetWorldRotation().Up()));
}

void PlayerController::HandleComboState()
{
    if (currentCombo > 0)
    {
        elapsedTimeSinceLastCombo += Time::GetDeltaTime();
        if (elapsedTimeSinceLastCombo > GlobalConstant::GetSingletonInstance().pod.comboTimeLimit)
        {
            ResetCombo();
        }
    }
}

void PlayerController::SelectPlayerUnit(PlayerCharacterType::Enum charType)
{
    UnSelectSkill();
    if (charType == selectedCharacterType)
    {
        return;
    }

    selectedCharacterType = charType;
    if (allySelectedEffectRenderer)
        allySelectedEffectRenderer->SetActive(selectedCharacterType != PlayerCharacterType::None);
    // 이전 유닛의 선택 UI 비활성화시키기
    if (auto previous = selectedCharacter.lock())
    {
        previous->unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_SelectionName)->DisableElement();
        unitSelectOutlineGuard.reset();
    }
    if (charType == PlayerCharacterType::None)
    {
        selectedCharacter.reset();
    }
    else
    {
        selectedCharacter = characters[charType];
        unitSelectOutlineGuard = selectedCharacter.lock()->referenceSelectOutline.Acquire();
        ApplySelectEffect(characters[charType]);
        // 체력바의 선택 UI 활성화시키기
        characters[charType].lock()->unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_SelectionName)->EnableElement();
    }

    switch (selectedCharacterType)
    {
    case PlayerCharacterType::Robin:
        peaceFollowingUnits[0] = characters[PlayerCharacterType::Ursula];
        peaceFollowingUnits[1] = characters[PlayerCharacterType::Hansel];
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->
            GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin_Left)->
            GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
        break;
    case PlayerCharacterType::Ursula:
        peaceFollowingUnits[0] = characters[PlayerCharacterType::Robin];
        peaceFollowingUnits[1] = characters[PlayerCharacterType::Hansel];
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->
            GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula_Left)->
            GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
        break;
    case PlayerCharacterType::Hansel:
        peaceFollowingUnits[0] = characters[PlayerCharacterType::Robin];
        peaceFollowingUnits[1] = characters[PlayerCharacterType::Ursula];
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel)->
            GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel_Left)->
            GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
    default:
        break;
    }
}

void PlayerController::OnLeftClick()
{
    if (selectedSkill == SkillType::NONE)
    {
        if (!cursorUnitDetector.lock()->GetUnits().empty())
        {
            SelectUnit(GetUnitOnCursor()->GetWeakPtr<Unit>());
        }
    }
    else
    {
        ActivateSkill(selectedSkill, GetWorldCursorPosition());
        if (state != State::Tactic)
        {
            selectedSkill = SkillType::NONE;
        }
    }
}

void PlayerController::OnPlayerChracterDead(std::weak_ptr<Unit> unit)
{
    bool alldead = true;
    for (auto& each : characters)
    {
        if (each.lock()->IsAlive())
        {
            alldead = false;
            break;
        }
    }
    if (alldead)
    {
        OnPlayerChracterAllDead();
    }
}

void PlayerController::OnPlayerChracterAllDead()
{
    UIManager::Instance().GetUIElementByEnum(UIEnumID::DefeatPage)->EnableElement();
}

void PlayerController::OnRightClick()
{
    if (selectedSkill == SkillType::NONE)
    {
        if (selectedCharacter.expired()) return;
        if (state != State::Tactic)
        {
            if (auto target = GetUnitOnCursor(); target && target->teamIndex != playerTeamIndex)
            {
                OrderAttack(target->GetWeakPtr<Unit>());
            }
            else
            {
                SFXManager::PlaySoundfile("sounds/Etc/Character move click.mp3");
                OrderMove(GetWorldCursorPosition());
            }
        }
        else
        {
            SkillPreviewSystem::Instance().HideTemporaryRoute();
            EnqueErrorType errorType = EnqueErrorType::NONE;
            if (auto target = GetUnitOnCursor(); target && target->teamIndex != playerTeamIndex)
            {
                // Attack
                // 걸어가서 공격을 하게 될 수 있음
                std::vector<Vector3d> path;
                path = TacticModeSystem::Instance().GetPathInTacticMode(selectedCharacterType, GetUnitOnCursor());
                this->ModifyPathForAttack(path);
                if (!path.empty())
                {
                    // 이동을 해야한다면
                    // 이동 명령
                    errorType = TacticModeSystem::Instance().EnqueueCommand(std::make_shared<UnitMoveCommand>(characters[selectedCharacterType].lock().get()
                        , path.back()
                        , path
                        , true
                    ));

                    // 공격 명령
                    if (errorType == EnqueErrorType::Success)
                    {
                        errorType = TacticModeSystem::Instance().EnqueueCommand(std::make_shared<UnitAttackCommand>(
                            characters[selectedCharacterType].lock().get()
                            , path.back()
                            , GetUnitOnCursor()
                            , true
                            , path.back() - path[path.size() - 2]));

                        // 이동 명령은 Enque됐지만 공격명령이 Enque되지 않았을 경우 이동 명령까지 지운다
                        if (errorType != EnqueErrorType::Success)
                        {
                            TacticModeSystem::Instance().PopCommand();
                        }
                    }
                }
                else
                {
                    // 이동없이 공격이 가능하다면 공격 명령
                    errorType = TacticModeSystem::Instance().EnqueueCommand(std::make_shared<UnitAttackCommand>(
                        characters[selectedCharacterType].lock().get()
                        , Vector3d::zero
                        , GetUnitOnCursor()
                        , false
                        , GetUnitOnCursor()->GetGameObject()->GetTransform()->GetWorldPosition() -
                        characters[selectedCharacterType].lock().get()->GetGameObject()->GetTransform()->GetWorldPosition()));
                    // 에러 타입에 따른 UI활성화
                }

            }
            else
            {
                // Move
                std::vector<Vector3d> path;
                path = TacticModeSystem::Instance().GetPathInTacticMode(selectedCharacterType);

                errorType = TacticModeSystem::Instance().EnqueueCommand(std::make_shared<UnitMoveCommand>(characters[selectedCharacterType].lock().get()
                    , GetWorldCursorPosition(),
                    path, false));
            }
            // 에러 타입에 따른 UI활성화
            EnableErrorUI(errorType);
        }
    }
    else if (selectedSkill != SkillType::NONE)
    {
        UnSelectSkill();
    }
}

void PlayerController::UnselectUnit()
{
    SelectPlayerUnit(PlayerCharacterType::None);
}
void PlayerController::SelectUnit(std::weak_ptr<Unit> unit)
{
    if (auto playerType = static_cast<PlayerCharacterType::Enum>(unit.lock()->GetUnitTemplateData().pod.playerUnitType.enumValue);
        playerType != PlayerCharacterType::None)
    {
        SelectPlayerUnit(playerType);
    }
}

void PlayerController::OrderMove(Vector3d position)
{
    if (auto unit = selectedCharacter.lock())
    {
        unit->OrderMove(position);
        UIManager::Instance().SummonMoveToFeedback(GetWorldCursorPosition());
    }
}

void PlayerController::OrderAttackMove(Vector3d position)
{
    if (auto unit = selectedCharacter.lock())
    {
        unit->OrderAttackMove(position);
        UIManager::Instance().SummonMoveToFeedback(GetWorldCursorPosition());
    }
}

void PlayerController::OrderAttack(std::weak_ptr<Unit> target)
{
    if (auto unit = selectedCharacter.lock())
    {
        ApplyTargetedEffect(target);
        unit->OrderAttack(target);
    }
}

void PlayerController::OrderInteraction(std::weak_ptr<IInteractableComponent> interactable)
{
    if (auto unit = selectedCharacter.lock())
    {
        unit->OrderMove(interactable.lock()->GetTransform()->GetWorldPosition());
    }
}

void PlayerController::ActivateSkill(SkillType::Enum skillType, Vector3d pos)
{
    if (skillCooltimeLeft[skillType] > 0) return;
    if (state == State::Cinematic) return;
    switch (skillType)
    {
    case SkillType::ROBIN_Q:
    case SkillType::HANSEL_Q:
        static constexpr float epsilon = 16.01f;
        Vector3d deltaDistance = (pos - SingleNavigationField::Instance().GetClosestPointOnField(pos));
        deltaDistance.y = 0;
        if (deltaDistance.MagnitudeSqr() > epsilon)
        {
            UIManager::Instance().GetUIElementByEnum(UIEnumID::ErrorPopup_CantLand)->EnableElement();
            UnSelectSkill();
            return;
        }
        pos -= deltaDistance;
        break;
    }
    onSkillTargeted[skillType]();
    if (state != State::Tactic)
    {
        onSkillActivate[skillType]();
        switch (skillType)
        {
        case SkillType::ROBIN_Q:
            selectedCharacter.lock()->OrderSkill(RobinChargeSkill{  }, pos);
            break;
        case SkillType::ROBIN_W:
            selectedCharacter.lock()->OrderSkill(RobinTauntSkill{  }, pos);
            break;
        case SkillType::URSULA_Q:
            selectedCharacter.lock()->OrderSkill(UrsulaBlindSkill{  }, pos);
            break;
        case SkillType::URSULA_W:
            selectedCharacter.lock()->OrderSkill(UrsulaParalysisSkill{  }, pos);
            break;
        case SkillType::HANSEL_Q:
            selectedCharacter.lock()->OrderSkill(HanselChargeSkill{}, pos);
            break;
        case SkillType::HANSEL_W:
            selectedCharacter.lock()->OrderSkill(HanselProjectileSkill{}, pos);
            break;
        }
        UnSelectSkill(true);
        // 스킬 프리뷰를 비활성화시킨다.
    }
    else
    {
        SkillPreviewSystem::Instance().HideSkillMaxRange();
        // 전술모드일 때 
        // Skill
        // 걸어가서 스킬을 쓰게될 수 있음
        EnqueErrorType errorType = EnqueErrorType::NONE;
        std::vector<Vector3d> path;
        path = TacticModeSystem::Instance().GetPathInTacticMode(selectedCharacterType);
        this->ModifyPathForSkill(path, skillType);
        if (!path.empty())
        {

            // 이동을 해야한다면
            // 이동 명령
            errorType = TacticModeSystem::Instance().EnqueueCommand(std::make_shared<UnitMoveCommand>(characters[selectedCharacterType].lock().get()
                , path.back()
                , path
                , false
            ));

            // 스킬 명령
            if (errorType == EnqueErrorType::Success)
            {
                errorType = TacticModeSystem::Instance().EnqueueCommand(std::make_shared<UnitSkillCommand>(characters[selectedCharacterType].lock().get()
                    , GetWorldCursorPosition()
                    , skillType
                ));

                // 이동 명령은 Enque됐지만 공격명령이 Enque되지 않았을 경우 이동 명령까지 지운다
                if (errorType != EnqueErrorType::Success)
                {
                    TacticModeSystem::Instance().PopCommand();
                }
                if (errorType == EnqueErrorType::Success)
                {
                    selectedSkill = SkillType::NONE;
                }
            }
        }
        else
        {
            // 이동없이 스킬 사용이 가능하다면 스킬 명령

            errorType = TacticModeSystem::Instance().EnqueueCommand(std::make_shared<UnitSkillCommand>(characters[selectedCharacterType].lock().get()
                , GetWorldCursorPosition()
                , skillType
            ));
            if (errorType == EnqueErrorType::Success)
            {
                selectedSkill = SkillType::NONE;
            }

        }
        // 에러 타입에 따른 UI활성화
        EnableErrorUI(errorType);
    }
}

void PlayerController::SelectSkill(SkillType::Enum skillType)
{
    if ((TacticModeSystem::Instance().CanSelectSkill(skillType) == false) && (TacticModeSystem::Instance().IsOperation()))
    {
        return;
    }
    if (blockSkillSelection[skillType] == true)
    {
        return;
    }

    UnSelectSkill();
    // 소모될 마나량을 출력
    switch (skillType)
    {
    case SkillType::ROBIN_Q:  SetPendingManaCost(RobinChargeSkill::pod.cost); break;
    case SkillType::ROBIN_W:  SetPendingManaCost(RobinTauntSkill::pod.skillCost); break;
    case SkillType::URSULA_Q: SetPendingManaCost(UrsulaBlindSkill::pod.skillCost); break;
    case SkillType::URSULA_W: SetPendingManaCost(UrsulaParalysisSkill::pod.skillCost); break;
    case SkillType::HANSEL_Q: SetPendingManaCost(HanselChargeSkill::pod.skillCost); break;
    case SkillType::HANSEL_W: SetPendingManaCost(HanselProjectileSkill::pod.skillCost); break;
    }
    SkillPreviewSystem::Instance().HideTemporaryRoute();
    if (skillCooltimeLeft[skillType] > 0)
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::ErrorPopup_Cooltime)->EnableElement();
        return;
    }
    if (mana < RequiredManaForSkill(skillType))
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::ErrorPopup_NoMana)->EnableElement();
        return;
    }
    // 이 부분은 협의가 필요함
    if ((state == State::Peace) || (state == State::Cinematic) || (TacticModeSystem::Instance().IsExecuting())) return;
    onSkillSelect[skillType]();
    switch (skillType)
    {
    case SkillType::ROBIN_Q: case SkillType::ROBIN_W: SelectPlayerUnit(PlayerCharacterType::Robin); break;
    case SkillType::URSULA_Q: case SkillType::URSULA_W: SelectPlayerUnit(PlayerCharacterType::Ursula); break;
    case SkillType::HANSEL_Q: case SkillType::HANSEL_W: SelectPlayerUnit(PlayerCharacterType::Hansel); break;
    }

    // 즉발은 그냥 써버리고 나머지는 준비상태로 만든다.
    switch (skillType)
    {
    case SkillType::ROBIN_W:
    {
        ActivateSkill(SkillType::ROBIN_W, characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition());
    }
    break;
    default:
        selectedSkill = skillType;
        // 스킬 프리뷰를 활성화시킨다.
        break;
    }
}
void PlayerController::SetState(State::Enum newState)
{
    if (newState == state)
        return;
    switch (state)
    {
    case PlayerController::State::Tactic:
        UIManager::Instance().GetUIElementByEnum(UIEnumID::TacticModeIngameUI)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Vinetting)->EnableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_MenuButton)->EnableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Bottom_Layout)->EnableElement();
        //skillCooltimeLeft.fill(0);
        //skillCooltimeLeft.fill(0);
        break;
    case PlayerController::State::Battle:
        zoomMultiplierByState.reset();
        break;
    }
    state = newState;
    switch (state)
    {
    case State::Peace:
        if (selectedCharacter.expired())
        {
            SelectPlayerUnit(PlayerCharacterType::Robin);
        }
        UnSelectSkill();
        for (auto& each : characters)
        {
            each.lock()->playingBattleAnim = false;
            each.lock()->Revive();
        }
        break;
    case State::Cinematic:
        UnselectUnit();
        UnSelectSkill();
        for (auto& each : characters)
        {
            each.lock()->playingBattleAnim = false;
        }
        break;
    case State::Battle:
        zoomMultiplierByState = camZoomMultiplier.AcquireFactor();
        *zoomMultiplierByState = GlobalConstant::GetSingletonInstance().pod.battleZoomMultiplier;
        for (auto& each : characters)
        {
            each.lock()->playingBattleAnim = true;
        }
        break;
    case State::Tactic:
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::TacticModeIngameUI)->EnableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Vinetting)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_MenuButton)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Bottom_Layout)->DisableElement();
        UnselectUnit();
        UnSelectSkill();
        previousSkillCooltimeLeft = skillCooltimeLeft;
        skillCooltimeLeft.fill(0);
        for (int skillType = SkillType::ROBIN_Q; skillType <= SkillType::HANSEL_W; skillType++)
        {
            SetCooltime((SkillType::Enum)skillType, 0);
        }

    }
    break;
    }
}
std::array<float, (int)PlayerCharacterType::Num> PlayerController::GetAggroProportions() const
{
    std::array<float, (int)PlayerCharacterType::Num> proportions;
    float sum = 0;
    sum = proportions[PlayerCharacterType::Robin] = characters.at(PlayerCharacterType::Robin).lock()->GetUnitTemplateData().pod.playerAggroRatio;
    sum += proportions[PlayerCharacterType::Ursula] = characters.at(PlayerCharacterType::Ursula).lock()->GetUnitTemplateData().pod.playerAggroRatio;
    sum += proportions[PlayerCharacterType::Hansel] = characters.at(PlayerCharacterType::Hansel).lock()->GetUnitTemplateData().pod.playerAggroRatio;
    proportions[PlayerCharacterType::Robin] /= sum;
    proportions[PlayerCharacterType::Ursula] /= sum;
    proportions[PlayerCharacterType::Hansel] /= sum;
    return proportions;
}
// 필요한 것들을 다 초기화한다.
void PlayerController::Reset()
{
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Number)->DisableElement();
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Text)->DisableElement();
    for (auto& each : onSkillTargeted) each.Clear();
    for (auto& each : onSkillExpiration) each.Clear();
    for (auto& each : onSkillActivate) each.Clear();
    for (auto& each : onSkillSelect) each.Clear();
    for (auto& each : blockSkillSelection) each = false;
    if (cursorUnitDetector.expired())
        cursorUnitDetector = Scene::getCurrentScene()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionBoxCollider>();
    std::for_each(skillUpgradeByUI.begin(), skillUpgradeByUI.end(), [&](auto& pair) {
        auto& [ui, upgrade] = pair;
        skillUpgraded[upgrade] = false;
        });
    skillPointsLeft = 0;
}

// 현재 카메라의 위치에 따라 카메라의 플레이어 기준 오프셋 위치와 회전각을 결정합니다.
void PlayerController::SetCameraOffset()
{
    if (characters[PlayerCharacterType::Robin].expired())
        return;

    auto camPos = RTSCam::Instance().GetTransform()->GetWorldPosition();
    camOffsetNorm = camPos - characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition();
    camZoomFactor = camOffsetNorm.y;
    camOffsetNorm /= camZoomFactor;
    camRotation = RTSCam::Instance().GetTransform()->GetWorldRotation();
}

void PlayerController::SetComboObjectives(const std::array<int, 3>& targetCombos)
{
    comboObjective = targetCombos;
    comboAchieved.fill(false);
    for (int i = 0; i < 3; i++)
    {
        comboAchieved[i] = false;
        UIManager::Instance().GetUIElementByEnum(UIManager::comboNumbers[i])->SetNumber(comboObjective[i]);
        UIManager::Instance().GetUIElementByEnum(UIManager::comboNumbers[i + 3])->SetNumber(comboObjective[i]);
        UIManager::Instance().GetUIElementByEnum(UIManager::comboFinishedImgs[i])->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIManager::comboCheckImgs[i])->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIManager::comboUnFinishedImgs[i])->EnableElement();
    }
}

void PlayerController::AddCombo()
{
    currentCombo++;
    elapsedTimeSinceLastCombo = 0;
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Number)->EnableElement();
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Text)->EnableElement();
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Number)->SetNumber(currentCombo);
    for (auto i = 0; i < 3; i++)
    {
        if (!comboAchieved[i] && comboObjective[i] > 0 && currentCombo >= comboObjective[i])
        {
            comboAchieved[i] = true;
            IncrementSkillPoint();
            UIManager::Instance().GetUIElementByEnum(UIManager::comboCheckImgs[i])->EnableElement();
            UIManager::Instance().GetUIElementByEnum(UIManager::comboFinishedImgs[i])->EnableElement();
            UIManager::Instance().GetUIElementByEnum(UIManager::comboUnFinishedImgs[i])->DisableElement();
        }
    }
}

void PlayerController::UnSelectSkill(bool forceUnselect)
{
    if (blockSkillCancel == true && !forceUnselect)
    {
        return;
    }
    switch (selectedSkill)
    {
    case SkillType::ROBIN_Q: SkillPreviewSystem::Instance().HideRobinQSkill(); break;
    case SkillType::URSULA_Q: SkillPreviewSystem::Instance().HideUrsulaQSkill(); break;
    case SkillType::URSULA_W: SkillPreviewSystem::Instance().HideUrsulaWSkill(); break;
    case SkillType::HANSEL_Q: SkillPreviewSystem::Instance().HideHanselQSkill(); break;
    case SkillType::HANSEL_W: SkillPreviewSystem::Instance().HideHanselWSkill(); break;
    }
    if (selectedSkill != SkillType::NONE)
    {
        SkillPreviewSystem::Instance().HideSkillMaxRange();
        SetPendingManaCost(0);
    }
    selectedSkill = SkillType::NONE;
}

void PlayerController::UnSelectSkill(std::weak_ptr<Unit> unit)
{
    auto playerType = unit.lock()->unitTemplateData->pod.playerUnitType.enumValue;
    switch (playerType)
    {
    case PlayerCharacterType::Robin:
        if (selectedSkill == SkillType::ROBIN_Q || selectedSkill == SkillType::ROBIN_W)
        {
            UnSelectSkill();
        }
        break;
    case PlayerCharacterType::Ursula:
        if (selectedSkill == SkillType::URSULA_Q || selectedSkill == SkillType::URSULA_W)
        {
            UnSelectSkill();
        }
        break;
    case PlayerCharacterType::Hansel:
        if (selectedSkill == SkillType::HANSEL_Q || selectedSkill == SkillType::HANSEL_W)
        {
            UnSelectSkill();
        }
        break;
    }
}

// xz평면에 사영된 마우스 위치를 반환한다.
Vector3d PlayerController::GetWorldCursorPosition()
{
    auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
    auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
    auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));
    return projectedPoint;
}

Vector3d PlayerController::GetMiddlePoint()
{
    auto ret = std::accumulate(characters.begin(), characters.end(), Vector3d(), [](Vector3d acc, auto& each)
        { return acc + each.lock()->GetTransform()->GetWorldPosition(); }) / characters.size();
    return ret;
}

void PlayerController::ResetCombo()
{
    currentCombo = 0;
    elapsedTimeSinceLastCombo = 0;
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Number)->DisableElement();
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Text)->DisableElement();
}

void PlayerController::SetTacticCameraActive(bool boolen)
{
    if (boolen)
    {
        auto tacticCamTransform = tacticCameraRef->GetTransform();
        Vector3d endPos = { tacticCamTransform->GetWorldPosition().x, tacticCamTransform->GetWorldPosition().y, tacticCamTransform->GetWorldPosition().z };
        Quaternion endRot = { tacticCamTransform->GetWorldRotation().w, tacticCamTransform->GetWorldRotation().x, tacticCamTransform->GetWorldRotation().y, tacticCamTransform->GetWorldRotation().z };
    }
    else
    {

    }
    if (GlobalConstant::GetSingletonInstance().pod.tacticCameraLerpTime == 0)
    {

    }
}

void PlayerController::SetManaFull()
{
    SetMana(GlobalConstant::GetSingletonInstance().pod.maxMana);
}
void PlayerController::SetMana(float mana)
{
    const auto& gc = GlobalConstant::GetSingletonInstance().pod;
    this->mana = std::clamp<float>(mana, 0, gc.maxMana);
    UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaBar1)->adjuster->SetTargetFloat(1 - GetManaNormalized());
    UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaBar2)->adjuster->SetTargetFloat(1 - GetManaNormalized());
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Mana_Text_MaxMP)->SetNumber(gc.maxMana);
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Mana_Text_CurrentMP)->SetNumber(this->mana);
}

void PlayerController::SetPendingManaCost(float manaCost)
{
    const auto& maxMana = GlobalConstant::GetSingletonInstance().pod.maxMana;
    float manabar1Width = UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaBar1)->imageComponent.lock()->GetGI().GetWidth();
    float manabar2Width = UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaBar2)->imageComponent.lock()->GetGI().GetWidth();
    UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaBarSpendOverlay1)->adjuster->SetTargetFloat(std::fminf(manaCost / maxMana, GetManaNormalized()));
    UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaBarSpendOverlay2)->adjuster->SetTargetFloat(std::fminf(manaCost / maxMana, GetManaNormalized()));
    UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaBarSpendOverlay1)->GetTransform()->SetLocalPosition(Vector3d::right * manabar1Width * GetManaNormalized());
    UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaBarSpendOverlay2)->GetTransform()->SetLocalPosition(Vector3d::right * manabar2Width * GetManaNormalized());
}

void PlayerController::TryTogglingTacticMode()
{
    if (UIManager::Instance().GetScriptUI("UiEnum:Toggle_TacticMode")->GetUIEnabled() && ((state == State::Battle) || state == State::Tactic))
    {
        if ((TacticModeSystem::Instance().IsCoolTime() == false) && (TacticModeSystem::Instance().IsExecuting() == false))
        {
            TacticModeSystem::Instance().EngageTacticSystem();
        }
        else if ((TacticModeSystem::Instance().IsOperation() == true) && (TacticModeSystem::Instance().IsExecuting() == false))
        {
            TacticModeSystem::Instance().ExecuteCommands();
        }
    }

}

Unit* PlayerController::GetUnitOnCursor()
{
    if (cursorUnitDetector.lock()->GetUnits().empty()) return nullptr;

    auto mainCamPos = graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition();
    return *std::min_element(cursorUnitDetector.lock()->GetUnits().begin(), cursorUnitDetector.lock()->GetUnits().end(),
        [&mainCamPos](Unit* a, Unit* b) {
            return a->DistanceSquare(mainCamPos) < b->DistanceSquare(mainCamPos);
        });
}

void PlayerController::OnPause()
{

}

void PlayerController::OnResume()
{
}

void PlayerController::RequestStateFromAction(State::Enum newState)
{
    this->stateRequestedByAction = newState;
}

void PlayerController::ApplyBeforeEngageSkillCoolTime()
{
    skillCooltimeLeft = previousSkillCooltimeLeft;
}

void PlayerController::SetTacticCamera(GameObject* cam)
{
    tacticCameraRef = cam;
}

float PlayerController::GetMana()
{
    return this->mana;
}

float PlayerController::GetManaNormalized()
{
    return this->mana / GlobalConstant::GetSingletonInstance().pod.maxMana;
}

Vector3d PlayerController::GetCamPivotPoint()
{
    return camPivotPoint;
}

void PlayerController::SetCooltime(SkillType::Enum skillType, float cooltime)
{
    if (TacticModeSystem::Instance().IsExecuting())
        return;
    if (skillType == SkillType::EnemyImpale)
        return;
    skillCooltimeLeft[skillType] = std::fmax(0.0f, cooltime);
    PlayerPortraitUIs::ReflectCooltime(skillType, cooltime, GetCooltimeForSkill(skillType));
}

void PlayerController::SetCooltime(std::weak_ptr<Unit> unit)
{
    SetCooltime(unit.lock()->onGoingSkill->GetSkillType(), GetCooltimeForSkill(unit.lock()->onGoingSkill->GetSkillType()));
}

void PlayerController::SetZoomFactor(float zoomFactor)
{
    this->camZoomFactor = zoomFactor;
}

float PlayerController::GetCooltimeForSkill(SkillType::Enum skillType)
{
    switch (skillType)
    {
    case SkillType::ROBIN_Q: return skillType, RobinChargeSkill::pod.coolTime;
    case SkillType::ROBIN_W: return RobinTauntSkill::pod.coolTime;
    case SkillType::URSULA_Q: return UrsulaBlindSkill::pod.skillCoolTime;
    case SkillType::URSULA_W: return UrsulaParalysisSkill::pod.skillCoolTime;
    case SkillType::HANSEL_Q: return HanselChargeSkill::pod.coolTime;
    case SkillType::HANSEL_W: return HanselProjectileSkill::pod.coolTime;
    }
}

float PlayerController::GetZoomFactor()
{
    return camZoomFactor;
}

float PlayerController::RequiredManaForSkill(SkillType::Enum skillType)
{
    switch (skillType)
    {
    case SkillType::ROBIN_Q: return RobinChargeSkill::pod.cost;
    case SkillType::ROBIN_W: return RobinTauntSkill::pod.skillCost;
    case SkillType::URSULA_Q: return UrsulaBlindSkill::pod.skillCost;
    case SkillType::URSULA_W: return UrsulaParalysisSkill::pod.skillCost;
    case SkillType::HANSEL_Q: return HanselChargeSkill::pod.skillCost;
    case SkillType::HANSEL_W: return HanselProjectileSkill::pod.skillCost;
    }
}

void PlayerController::ApplyHoverEffect(std::weak_ptr<Unit> unit)
{
    if (unit.lock()->GetTeamIndex() == playerTeamIndex)
    {
        enemyHoverEffect->SetSelfActive(false);
        allyHoverEffect->SetSelfActive(true);
        allyHoverEffect->GetTransform()->SetLocalPosition(unit.lock()->GetGameObject()->GetTransform()->GetWorldPosition());
        allyHoverEffectAnimator->Resume();

        switch (unit.lock()->unitTemplateData->pod.playerUnitType.enumValue)
        {
        case PlayerCharacterType::Robin:
            allyHoverEffectRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);
            break;
        case PlayerCharacterType::Ursula:
            allyHoverEffectRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
            break;
        case PlayerCharacterType::Hansel:
            allyHoverEffectRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);
            break;
        }
    }
    else
    {
        enemyHoverEffect->SetSelfActive(true);
        allyHoverEffect->SetSelfActive(false);
        enemyHoverEffect->GetTransform()->SetLocalPosition(unit.lock()->GetGameObject()->GetTransform()->GetWorldPosition());
        enemyHoverEffectAnimator->Resume();
    }
}

void PlayerController::DisableHoverEffect()
{
    if (!mouseInteractionEffectInitalized) return;
    enemyHoverEffect->SetSelfActive(false);
    allyHoverEffect->SetSelfActive(false);
}

void PlayerController::ApplySelectEffect(std::weak_ptr<Unit> unit)
{
    if (!mouseInteractionEffectInitalized) return;
    allySelectedEffect->SetSelfActive(true);
    allySelectedEffect->SetParent(unit.lock()->GetGameObject());
    switch (selectedCharacterType)
    {
    case PlayerCharacterType::Robin:
        allySelectedEffectRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ROBIN_COLOR);
        break;
    case PlayerCharacterType::Ursula:
        allySelectedEffectRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::URSULA_COLOR);
        break;
    case PlayerCharacterType::Hansel:
        allySelectedEffectRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::HANSEL_COLOR);
        break;
    }

}

void PlayerController::ApplyTargetedEffect(std::weak_ptr<Unit> unit)
{
    if (!mouseInteractionEffectInitalized) return;

    enemyTargetedEffect[selectedCharacterType]->SetSelfActive(true);
    enemyTargetedEffectAnimator[selectedCharacterType]->Play();
    enemyTargetedEffect[selectedCharacterType]->SetParent(unit.lock()->GetGameObject());
}

void PlayerController::InitUnitMouseInteractionEffects()
{
    const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    if (mouseInteractionEffectInitalized) return;
    mouseInteractionEffectInitalized = true;
    {
        allySelectedEffect = Scene::getCurrentScene()->AddGameObjectFromFBX("VFX_CharacterRange");
        allySelectedEffectRenderer = allySelectedEffect->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        allySelectedEffectRenderer->GetGI().GetMaterial()->SetPixelShader(_resourceManager->GetShader(L"VFX_PS.cso"));
        allySelectedEffect->SetSelfActive(false);
    }
    {
        int i = 0;
        for (auto& each : enemyTargetedEffect)
        {
            each = Scene::getCurrentScene()->AddGameObjectFromFBX("VFX_AttackSelected");
            each->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ENEMY_COLOR);
            each->SetSelfActive(false);
            enemyTargetedEffectAnimator[i] = each->AddComponent<VFXAnimator>();
            enemyTargetedEffectAnimator[i]->Init();
            ++i;
        }
    }
    {
        allyHoverEffect = Scene::getCurrentScene()->AddGameObjectFromFBX("VFX_CharacterSelected");
        allyHoverEffect->SetSelfActive(false);
        allyHoverEffectRenderer = allyHoverEffect->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        allyHoverEffectRenderer->GetGI().GetMaterial()->SetPixelShader(_resourceManager->GetShader(L"VFX_PS.cso"));

        allyHoverEffectAnimator = allyHoverEffect->AddComponent<VFXAnimator>();
        allyHoverEffectAnimator->Init();
        allyHoverEffectAnimator->SetLoop(true);
    }
    {
        enemyHoverEffect = Scene::getCurrentScene()->AddGameObjectFromFBX("VFX_CharacterSelected");
        enemyHoverEffect->SetSelfActive(false);
        enemyHoverEffectRenderer = enemyHoverEffect->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        enemyHoverEffectRenderer->GetGI().GetMaterial()->SetPixelShader(_resourceManager->GetShader(L"VFX_PS.cso"));
        enemyHoverEffectRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ENEMY_COLOR);

        enemyHoverEffectAnimator = enemyHoverEffect->AddComponent<VFXAnimator>();
        enemyHoverEffectAnimator->Init();
        enemyHoverEffectAnimator->SetLoop(true);
    }
}

void PlayerController::EnableErrorUI(EnqueErrorType errorType)
{
    switch (errorType)
    {
    case EnqueErrorType::NotEnoughMana:
        UIManager::Instance().GetUIElementByEnum(UIEnumID::ErrorPopup_NoMana)->EnableElement();
        break;
    case EnqueErrorType::QueueFull:
        UIManager::Instance().GetUIElementByEnum(UIEnumID::ErrorPopup_TacticQueueFull)->EnableElement();
        break;
    }
}

void PlayerController::OnPlayerUnitSkillActivation(std::weak_ptr<Unit> unit, std::weak_ptr<Skill> skill)
{
    // 전술 모드가 아니라면 기존 로직 수행
    SkillType::Enum skillType = skill.lock()->GetSkillType();
    SetCooltime(skillType, GetCooltimeForSkill(skillType));
    if (skillType != SkillType::NONE)
    {
        //if (state != State::Tactic)
        //{
        //    //SetMana(mana - RequiredManaForSkill(skillType));
        //}
        onSkillActivate[skillType]();
    }
}

void PlayerController::OnPlayerUnitSkillExpiration(std::weak_ptr<Unit> unit, std::weak_ptr<Skill> skill)
{
    if (dynamic_cast<RobinChargeSkill*>(skill.lock().get())) onSkillExpiration[SkillType::ROBIN_Q]();
    if (dynamic_cast<RobinTauntSkill*>(skill.lock().get())) onSkillExpiration[SkillType::ROBIN_W]();
    if (dynamic_cast<UrsulaBlindSkill*>(skill.lock().get())) onSkillExpiration[SkillType::URSULA_Q]();
    if (dynamic_cast<UrsulaParalysisSkill*>(skill.lock().get())) onSkillExpiration[SkillType::URSULA_W]();
    if (dynamic_cast<HanselChargeSkill*>(skill.lock().get())) onSkillExpiration[SkillType::HANSEL_Q]();
    if (dynamic_cast<HanselProjectileSkill*>(skill.lock().get())) onSkillExpiration[SkillType::HANSEL_W]();
}

std::vector<yunutyEngine::Vector3d>& PlayerController::ModifyPathForAttack(std::vector<Vector3d>& path)
{
    bool isModify = false;
    auto lastElement = path.back();

    float attackRange = selectedCharacter.lock()->GetUnitTemplateData().pod.m_atkRadius;

    for (auto it = path.rbegin(); it != path.rend(); ++it)
    {
        if (it != path.rbegin())
        {
            auto tempVec = lastElement - *it;
            float distance = tempVec.Magnitude();
            if (attackRange < distance - 0.001f)
            {
                isModify = true;
                // 조건을 만족하는 요소를 찾으면 그 위치의 다음 요소부터 제거
                path.erase((it.base() + 1), path.end());
                break;
            }
        }
    }
    if (!isModify)
    {
        path.clear();
    }
    return path;
}

std::vector<yunutyEngine::Vector3d>& PlayerController::ModifyPathForSkill(std::vector<Vector3d>& path, SkillType::Enum skillType)
{
    bool isModify = false;
    auto lastElement = path.back();

    float skillRange;

    switch (skillType)
    {
    case SkillType::ROBIN_Q:
        skillRange = RobinChargeSkill::GetMaxDistance();
        break;
    case SkillType::ROBIN_W:
        break;
    case SkillType::URSULA_Q:
        skillRange = UrsulaBlindSkill::GetSkillRange();
        break;
    case SkillType::URSULA_W:
        skillRange = UrsulaParalysisSkill::pod.skillRange;
        break;
    case SkillType::HANSEL_Q:
        skillRange = HanselChargeSkill::GetMaxRange();
        break;
    case SkillType::HANSEL_W:
        skillRange = HanselProjectileSkill::GetMaxRange();
        break;
    }

    if ((skillType != SkillType::ROBIN_W))
    {
        for (auto it = path.rbegin(); it != path.rend(); ++it)
        {
            if (it != path.rbegin())
            {
                auto tempVec = lastElement - *it;
                float distance = tempVec.Magnitude();
                if (skillRange < distance - 0.001f)
                {
                    isModify = true;
                    // 조건을 만족하는 요소를 찾으면 그 위치의 다음 요소부터 제거
                    path.erase((it.base() + 1), path.end());
                    break;
                }
            }
        }
    }

    if ((!isModify) || (skillType == SkillType::ROBIN_W))
    {
        path.clear();
    }
    return path;
}

void PlayerController::SetAttackMoveMode(bool attackMoveMode)
{
    this->attackMoveMode = attackMoveMode;
}
