#include "InWanderLand.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "Dotween.h"
#include "SkillPreviewSystem.h"
#include "GameManager.h"
#include "CursorDetector.h"
#include "UIManager.h"

void PlayerController::RegisterPlayer(std::weak_ptr<Unit> unit)
{
    characters[unit.lock()->GetUnitTemplateData()->pod.playerUnitType] = unit;
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
}

void PlayerController::HandleInput()
{
    GameManager::Instance().IsBattleSystemOperating();
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
    if (Input::isKeyPushed(KeyCode::MouseLeftClick) && !UIManager::Instance().IsMouseOnButton())
    {
        OnLeftClick();
    }
    if (Input::isKeyPushed(KeyCode::MouseRightClick) && !UIManager::Instance().IsMouseOnButton())
    {
        OnRightClick();
    }
}

void PlayerController::SelectPlayerUnit(PlayerCharacterType charType)
{
    if (charType == selectedCharacterType)
    {
        return;
    }
    selectedCharacterType = charType;
    selectedCharacter = characters[(int)charType];
    RTSCam* mainCam = static_cast<RTSCam*>(yunutyEngine::graphics::Camera::GetMainCamera());
    if (mainCam)
    {
        mainCam->SetTarget(selectedCharacter.lock()->GetGameObject());
    }
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
        break;
    }
}

void PlayerController::OnLeftClick()
{
    if (selectedSkill == SkillType::NONE)
    {
        SelectUnit(*cursorUnitDetector.lock()->GetUnits().begin());
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
    SelectPlayerUnit(unit.lock()->GetUnitTemplateData()->pod.playerUnitType);
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
    selectedCharacter.lock()->OrderAttackMove(unit.lock());
}

void PlayerController::OrderInteraction(std::weak_ptr<IInteractableComponent> interactable)
{
    selectedCharacter.lock()->OrderMove(interactable.lock()->GetTransform()->GetWorldPosition());
}

void PlayerController::ActivateSkill(SkillType skillType, Vector3d pos)
{
    if (!GameManager::Instance().IsBattleSystemOperating()) return;
    onSkillActivate[(int)skillType]();
    switch (skillType)
    {
    case SkillType::ROBIN_Q: selectedCharacter.lock()->ApplySkill(RobinChargeSkill{ pos }); break;
    case SkillType::ROBIN_W: selectedCharacter.lock()->ApplySkill(RobinChargeSkill{ pos }); break;
    case SkillType::URSULA_Q: selectedCharacter.lock()->ApplySkill(RobinChargeSkill{ pos }); break;
    case SkillType::URSULA_W: selectedCharacter.lock()->ApplySkill(RobinChargeSkill{ pos }); break;
    case SkillType::HANSEL_Q: selectedCharacter.lock()->ApplySkill(RobinChargeSkill{ pos }); break;
    case SkillType::HANSEL_W: selectedCharacter.lock()->ApplySkill(RobinChargeSkill{ pos }); break;
    }
}

void PlayerController::SelectSkill(SkillType skillType)
{
    if (!GameManager::Instance().IsBattleSystemOperating()) return;
    UnSelectSkill();
    onSkillSelect[(int)skillType]();
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
        break;
    }
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

void PlayerController::ShowSkillPreview()
{
}

// xz평면에 사영된 마우스 위치를 반환한다.
Vector3d PlayerController::GetWorldCursorPosition()
{
    auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
    auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
    auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));
    return projectedPoint;
}
