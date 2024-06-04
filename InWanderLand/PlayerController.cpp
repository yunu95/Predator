#include "InWanderLand.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "SkillPreviewSystem.h"
#include "GameManager.h"
#include "UIManager.h"
#include "EnemyImpaleSkill.h"

const std::unordered_map<UIEnumID, SkillUpgradeType::Enum> PlayerController::skillByUI
{
    {UIEnumID::SkillUpgradeButtonRobin00,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonRobin11,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonRobin12,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonRobin21,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonRobin22,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonUrsula00,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonUrsula11,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonUrsula12,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonUrsula21,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonUrsula22,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonHansel00,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonHansel11,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonHansel12,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonHansel21,SkillUpgradeType::NONE},
    {UIEnumID::SkillUpgradeButtonHansel22,SkillUpgradeType::NONE},
};
void PlayerController::RegisterPlayer(std::weak_ptr<Unit> unit)
{
    if (unit.lock()->GetUnitTemplateData().pod.playerUnitType.enumValue == PlayerCharacterType::None)
        return;

    characters[unit.lock()->GetUnitTemplateData().pod.playerUnitType.enumValue] = unit;
    if (unit.lock()->GetUnitTemplateData().pod.playerUnitType.enumValue == PlayerCharacterType::Robin)
    {
        SetCameraOffset();
        selectedCharacter = unit;
    }
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
    return skillUpgraded.at(skillByUI.at(skillUpgradeUITarget));
}
void PlayerController::UpgradeSkill()
{
    SetSkillPoints(skillPointsLeft - 1);
    static constexpr float gray = 0.3f;
    UIManager::Instance().GetUIElementByEnum(skillUpgradeUITarget)->imageComponent.lock()->GetGI().SetColor({ gray,gray,gray,1 });
    skillUpgraded[skillByUI.at(skillUpgradeUITarget)] = true;
}
void PlayerController::SetSkillPoints(int points)
{
    skillPointsLeft = points;
    UIManager::Instance().GetUIElementByEnum(UIEnumID::SkillPoint_Number)->SetNumber(skillPointsLeft);
}
int PlayerController::GetSkillPoints()
{
    return skillPointsLeft;
}
void PlayerController::IncrementSkillPoint()
{
    SetSkillPoints(skillPointsLeft + 1);
}

void PlayerController::LockCamInRegion(const application::editor::RegionData* camLockRegion)
{
    this->camLockRegion = camLockRegion;
}

void PlayerController::Start()
{
}
void PlayerController::OnContentsPlay()
{
    SetActive(true);
    cursorUnitDetector = Scene::getCurrentScene()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionSphereCollider>();
}
void PlayerController::OnContentsStop()
{
    SetActive(false);
    Scene::getCurrentScene()->DestroyGameObject(cursorUnitDetector.lock()->GetGameObject());
}

void PlayerController::Update()
{
    cursorUnitDetector.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetWorldCursorPosition());
    HandleInput();
    HandleCamera();
#ifdef EDITOR
    static yunutyEngine::graphics::UIText* text_State{ nullptr };
    if (text_State == nullptr)
    {
        text_State = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::UIText>();
        text_State->GetGI().SetFontSize(30);
        text_State->GetGI().SetColor(yunuGI::Color{ 1,0,1,1 });
        text_State->GetTransform()->SetLocalScale(Vector3d{ 1200,500,0 });
        text_State->GetTransform()->SetLocalPosition(Vector3d{ 0,260,0 });
    }
    if (!selectedDebugCharacter.expired())
    {
        wstringstream wsstream;
        wsstream << L"unitState : ";
        auto& activeStates = selectedDebugCharacter.lock()->GetBehaviourTree().GetActiveNodes();
        for (const auto& each : activeStates)
        {
            wsstream << wstring(L"[") + yutility::GetWString(POD_Enum<UnitBehaviourTree::Keywords>::GetEnumNameMap().at(each->GetNodeKey())) + wstring(L"]");
        }
        wsstream << selectedDebugCharacter.lock()->GetTransform()->GetWorldRotation().Forward().x << L",";
        wsstream << selectedDebugCharacter.lock()->GetTransform()->GetWorldRotation().Forward().y << L",";
        wsstream << selectedDebugCharacter.lock()->GetTransform()->GetWorldRotation().Forward().z << L",";
        wsstream << selectedDebugCharacter.lock()->currentRotation << L",";

        text_State->GetGI().SetText(wsstream.str());
    }
#endif
}

void PlayerController::HandleInput()
{
    if (state == State::Cinematic) return;
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

void PlayerController::HandleCamera()
{
    static constexpr float tacticZoomoutDistanceFactor = 1.2f;
    // 영웅이 선택되어 있고, 카메라가 선택된 영웅을 따라가는 경우 targetPos는 영웅의 위치로 설정됩니다.
    Vector3d targetPos;
    if (!selectedCharacter.expired())
    {
        Vector3d selectedCharPos = selectedCharacter.lock()->GetTransform()->GetWorldPosition();
        targetPos = selectedCharPos + camOffset;
    }
    // 카메라가 지역 제한에 걸렸을 경우, targetPos를 지역 안으로 정의합니다.
    if (camLockRegion)
    {
        targetPos.x = std::clamp(targetPos.x, camLockRegion->pod.x - camLockRegion->pod.width * 0.5, camLockRegion->pod.x + camLockRegion->pod.width * 0.5);
        targetPos.z = std::clamp(targetPos.z, camLockRegion->pod.z - camLockRegion->pod.height * 0.5, camLockRegion->pod.z + camLockRegion->pod.height * 0.5);
    }
    RTSCam::Instance().SetIdealPosition(targetPos);
    RTSCam::Instance().SetIdealRotation(camRotation);
}

void PlayerController::SelectPlayerUnit(PlayerCharacterType::Enum charType)
{
    if (charType == selectedCharacterType || charType == PlayerCharacterType::None)
    {
        return;
    }
    selectedCharacterType = charType;
    selectedCharacter = characters[charType];
    selectedDebugCharacter = characters[charType];
    switch (selectedCharacterType)
    {
    case PlayerCharacterType::Robin:
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->
            GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
        break;
    case PlayerCharacterType::Ursula:
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->
            GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
        break;
    case PlayerCharacterType::Hansel:
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel)->
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
            SelectUnit((*cursorUnitDetector.lock()->GetUnits().begin())->GetWeakPtr<Unit>());
        }
    }
    else
    {
        ActivateSkill(selectedSkill, GetWorldCursorPosition());
    }
}

void PlayerController::OnRightClick()
{
    if (selectedSkill == SkillType::NONE)
    {
        OrderMove(GetWorldCursorPosition());
    }
    else
    {
        UnSelectSkill();
    }
}

void PlayerController::SelectUnit(std::weak_ptr<Unit> unit)
{
    SelectPlayerUnit(static_cast<PlayerCharacterType::Enum>(unit.lock()->GetUnitTemplateData().pod.playerUnitType.enumValue));
    selectedDebugCharacter = unit;
}

void PlayerController::OrderMove(Vector3d position)
{
    selectedCharacter.lock()->OrderMove(position);
    UIManager::Instance().SummonMoveToFeedback(GetWorldCursorPosition());
}

void PlayerController::OrderAttackMove(Vector3d position)
{
    selectedCharacter.lock()->OrderAttackMove(position);
    UIManager::Instance().SummonMoveToFeedback(GetWorldCursorPosition());
}

void PlayerController::OrderAttack(std::weak_ptr<Unit> unit)
{
    selectedCharacter.lock()->OrderAttack(unit);
}

void PlayerController::OrderInteraction(std::weak_ptr<IInteractableComponent> interactable)
{
    selectedCharacter.lock()->OrderMove(interactable.lock()->GetTransform()->GetWorldPosition());
}

void PlayerController::ActivateSkill(SkillType::Enum skillType, Vector3d pos)
{
    if (state == State::Cinematic) return;
    onSkillActivate[skillType]();
    switch (skillType)
    {
    case SkillType::ROBIN_Q: selectedCharacter.lock()->OrderSkill(RobinChargeSkill{  }, pos); break;
    case SkillType::ROBIN_W: selectedCharacter.lock()->OrderSkill(RobinChargeSkill{  }, pos); break;
    case SkillType::URSULA_Q: selectedCharacter.lock()->OrderSkill(EnemyImpaleSkill{ }, pos); break;
    case SkillType::URSULA_W: selectedCharacter.lock()->OrderSkill(RobinChargeSkill{ }, pos); break;
    case SkillType::HANSEL_Q: selectedCharacter.lock()->OrderSkill(RobinChargeSkill{ }, pos); break;
    case SkillType::HANSEL_W: selectedCharacter.lock()->OrderSkill(RobinChargeSkill{ }, pos); break;
    }
    // 스킬 프리뷰를 비활성화시킨다.
}

void PlayerController::SelectSkill(SkillType::Enum skillType)
{
    //if (!GameManager::Instance().IsBattleSystemOperating()) return;
    UnSelectSkill();
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
    case SkillType::ROBIN_W: ActivateSkill(SkillType::ROBIN_W, Vector3d::zero); break;
    default:
        selectedSkill = skillType;
        // 스킬 프리뷰를 활성화시킨다.
        break;
    }
}
void PlayerController::SetState(State::Enum newState)
{
    state = newState;
    switch (state)
    {
    case State::Peace:
    case State::Cinematic:
        UnSelectSkill();
        break;
    }
}
// 필요한 것들을 다 초기화한다.
void PlayerController::Reset()
{
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Number)->DisableElement();
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Text)->DisableElement();
    for (auto& each : onSkillActivate) each.Clear();
    for (auto& each : onSkillSelect) each.Clear();
    for (auto& each : blockSkillSelection) each = false;
    if (cursorUnitDetector.expired())
        cursorUnitDetector = Scene::getCurrentScene()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionSphereCollider>();
    std::for_each(skillByUI.begin(), skillByUI.end(), [&](auto& pair) {
        auto& [ui, upgrade] = pair;
        UIManager::Instance().GetUIElementByEnum(ui)->imageComponent.lock()->GetGI().SetColor({ 1,1,1,1 });
        skillUpgraded[upgrade] = false;
        });
    skillPointsLeft = 0;
}

// 현재 카메라의 위치에 따라 카메라의 플레이어 기준 오프셋 위치와 회전각을 결정합니다.
void PlayerController::SetCameraOffset()
{
    if (characters[PlayerCharacterType::Robin].expired())
        return;
    auto camPos = graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition();
    camOffset = camPos - characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition();
    camRotation = graphics::Camera::GetMainCamera()->GetTransform()->GetWorldRotation();
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
        UIManager::Instance().GetUIElementByEnum(UIManager::comboUnFinishedImgs[i])->EnableElement();
        UIManager::Instance().GetUIElementByEnum(UIManager::comboCheckImgs[i])->DisableElement();
    }
}

void PlayerController::AddCombo()
{
    currentCombo++;
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

void PlayerController::OnWaveStart(std::weak_ptr<PlaytimeWave> p_wave)
{
}

void PlayerController::OnWaveEnd(std::weak_ptr<PlaytimeWave> p_wave)
{
}

void PlayerController::UnSelectSkill()
{
    switch (selectedSkill)
    {
    case SkillType::ROBIN_Q: SkillPreviewSystem::Instance().HideRobinQSkill(); break;
    case SkillType::URSULA_Q: SkillPreviewSystem::Instance().HideUrsulaQSkill(); break;
    case SkillType::URSULA_W: SkillPreviewSystem::Instance().HideUrsulaWSkill(); break;
    case SkillType::HANSEL_Q: SkillPreviewSystem::Instance().HideHanselQSkill(); break;
    case SkillType::HANSEL_W: SkillPreviewSystem::Instance().HideHanselWSkill(); break;
    }
    selectedSkill = SkillType::NONE;
}

// xz평면에 사영된 마우스 위치를 반환한다.
Vector3d PlayerController::GetWorldCursorPosition()
{
    auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
    auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
    auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));
    return projectedPoint;
}

void PlayerController::ResetCombo()
{
}
