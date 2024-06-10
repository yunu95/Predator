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
void PlayerController::RegisterUnit(std::weak_ptr<Unit> unit)
{
	auto playerType = unit.lock()->GetUnitTemplateData().pod.playerUnitType.enumValue;
	if (playerType == PlayerCharacterType::None)
		return;

	characters[unit.lock()->GetUnitTemplateData().pod.playerUnitType.enumValue] = unit;
	if (!characters[PlayerCharacterType::Robin].expired() &&
		!characters[PlayerCharacterType::Ursula].expired() &&
		!characters[PlayerCharacterType::Hansel].expired())
	{
		SetCameraOffset();
		SelectPlayerUnit(PlayerCharacterType::Robin);
	}
	unit.lock()->onStateEngage[UnitBehaviourTree::Death].AddCallback([this, unit]() { UnSelectSkill(unit); });
	unit.lock()->onStateEngage[UnitBehaviourTree::Paralysis].AddCallback([this, unit]() { UnSelectSkill(unit); });
	UnSelectSkill(unit);
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
	cursorUnitDetector = Scene::getCurrentScene()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionSphereCollider>();
	AttachDebugMesh(cursorUnitDetector.lock()->GetGameObject(), DebugMeshType::Cube, yunuGI::Color::white());
	cursorUnitDetector.lock()->SetRadius(0.01f);
	skillCooltimeNumberUI[SkillType::ROBIN_Q] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Cooltime_Number);
	skillCooltimeNumberUI[SkillType::ROBIN_W] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Cooltime_Number);
	skillCooltimeNumberUI[SkillType::URSULA_Q] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Cooltime_Number);
	skillCooltimeNumberUI[SkillType::URSULA_W] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Cooltime_Number);
	skillCooltimeNumberUI[SkillType::HANSEL_Q] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Cooltime_Number);
	skillCooltimeNumberUI[SkillType::HANSEL_W] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Cooltime_Number);
	skillCooltimeMaskUI[SkillType::ROBIN_Q] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Overlay);
	skillCooltimeMaskUI[SkillType::ROBIN_W] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Overlay);
	skillCooltimeMaskUI[SkillType::URSULA_Q] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Overlay);
	skillCooltimeMaskUI[SkillType::URSULA_W] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Overlay);
	skillCooltimeMaskUI[SkillType::HANSEL_Q] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Overlay);
	skillCooltimeMaskUI[SkillType::HANSEL_W] = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel)->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Overlay);
	InitUnitMouseInteractionEffects();
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
	HandleSkillPreview();
	HandleByState();
	HandleSkillCooltime();
	HandleManaRegen();
	HandleMouseHover();
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
		wsstream << L"\nacq : " << selectedDebugCharacter.lock()->acquisitionRange.lock()->GetEnemies().size();
		wsstream << L"\ncurrent pos : " << selectedDebugCharacter.lock()->GetTransform()->GetWorldPosition();
		wsstream << L"\nattack target pos : " << selectedDebugCharacter.lock()->attackMoveDestination;
		if (!cursorUnitDetector.lock()->GetUnits().empty())
		{
			wsstream << L"\nhovering unit : " << yutility::GetWString((*cursorUnitDetector.lock()->GetUnits().begin())->name);
		}

		text_State->GetGI().SetText(wsstream.str());
	}
#endif
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

void PlayerController::HandleSkillPreview()
{
	switch (selectedSkill)
	{
		case SkillType::ROBIN_Q:
			SkillPreviewSystem::Instance().ShowRobinQSkill(characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition());
			SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Robin, characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition(), RobinChargeSkill::pod.maxDistance);
			break;
		case SkillType::URSULA_Q:
		{
			auto pos1 = UrsulaBlindSkill::GetSkillObjectPos_Left(GetWorldCursorPosition());
			auto pos2 = UrsulaBlindSkill::GetSkillObjectPos_Right(GetWorldCursorPosition());
			auto pos3 = UrsulaBlindSkill::GetSkillObjectPos_Top(GetWorldCursorPosition());
			SkillPreviewSystem::Instance().ShowUrsulaQSkill(pos1, pos2, pos3, Vector3d::one * UrsulaBlindSkill::pod.skillRadius);
			SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), UrsulaBlindSkill::pod.skillRange);
			break;
		}
		case SkillType::URSULA_W:
			SkillPreviewSystem::Instance().ShowUrsulaWSkill(GetWorldCursorPosition(), UrsulaParalysisSkill::pod.skillRadius);
			SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), UrsulaParalysisSkill::pod.skillRange);
			break;
		case SkillType::HANSEL_Q:
			SkillPreviewSystem::Instance().ShowHanselQSkill(GetWorldCursorPosition(), HanselChargeSkill::pod.stompRadius);
			SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition(), HanselChargeSkill::pod.maxRange);
			break;
		case SkillType::HANSEL_W:
			SkillPreviewSystem::Instance().ShowHanselWSkill(characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition());
			SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition(), HanselProjectileSkill::pod.maxRange);
			break;
	}
}

void PlayerController::HandleSkillCooltime()
{
	for (int skillType = SkillType::ROBIN_Q; skillType <= SkillType::HANSEL_W; skillType++)
	{
		SetCooltime((SkillType::Enum)skillType, skillCooltimeLeft[skillType] - Time::GetDeltaTime());
	}
}

void PlayerController::HandleManaRegen()
{
	SetMana(mana + GlobalConstant::GetSingletonInstance().pod.manaRegen * Time::GetDeltaTime());
}

void PlayerController::HandleMouseHover()
{
	if (!cursorUnitDetector.lock()->GetUnits().empty())
	{
		ApplyHoverEffect((*cursorUnitDetector.lock()->GetUnits().begin())->GetWeakPtr<Unit>());
	}
	else
	{
		DisableHoverEffect();
	}
}

void PlayerController::SelectPlayerUnit(PlayerCharacterType::Enum charType)
{
	UnSelectSkill();
	if (charType == selectedCharacterType || charType == PlayerCharacterType::None)
	{
		return;
	}
	selectedCharacterType = charType;
	selectedCharacter = characters[charType];
	selectedDebugCharacter = characters[charType];
	ApplySelectEffect(characters[charType]);
	switch (selectedCharacterType)
	{
		case PlayerCharacterType::Robin:
			peaceFollowingUnits[0] = characters[PlayerCharacterType::Ursula];
			peaceFollowingUnits[1] = characters[PlayerCharacterType::Hansel];
			UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->
				GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
			break;
		case PlayerCharacterType::Ursula:
			peaceFollowingUnits[0] = characters[PlayerCharacterType::Robin];
			peaceFollowingUnits[1] = characters[PlayerCharacterType::Hansel];
			UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->
				GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeInternalButtonClickEvent();
			break;
		case PlayerCharacterType::Hansel:
			peaceFollowingUnits[0] = characters[PlayerCharacterType::Robin];
			peaceFollowingUnits[1] = characters[PlayerCharacterType::Ursula];
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
		selectedSkill = SkillType::NONE;
	}
}

void PlayerController::OnRightClick()
{
	if (selectedSkill == SkillType::NONE)
	{
		if (!cursorUnitDetector.lock()->GetUnits().empty() && (*cursorUnitDetector.lock()->GetUnits().begin())->teamIndex != playerTeamIndex)
		{
			OrderAttack((*cursorUnitDetector.lock()->GetUnits().begin())->GetWeakPtr<Unit>());
		}
		else
		{
			OrderMove(GetWorldCursorPosition());
		}
	}
	else if (selectedSkill != SkillType::NONE)
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
	ApplyTargetedEffect(unit);
	selectedCharacter.lock()->OrderAttack(unit);
}

void PlayerController::OrderInteraction(std::weak_ptr<IInteractableComponent> interactable)
{
	selectedCharacter.lock()->OrderMove(interactable.lock()->GetTransform()->GetWorldPosition());
}

void PlayerController::ActivateSkill(SkillType::Enum skillType, Vector3d pos)
{
	if (skillCooltimeLeft[skillType] > 0) return;
	if (state == State::Cinematic) return;
	SetMana(mana - RequiredManaForSkill(skillType));
	onSkillActivate[skillType]();
	SetCooltime(skillType, GetCooltimeForSkill(skillType));
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
	UnSelectSkill();
	// 스킬 프리뷰를 비활성화시킨다.
}

void PlayerController::SelectSkill(SkillType::Enum skillType)
{
	UnSelectSkill();
	if (skillCooltimeLeft[skillType] > 0)
	{
		UIManager::Instance().GetUIElementByEnum(UIEnumID::ErrorPopup_Cooltime)->EnableElement();
		return;
	}
	if (mana < RequiredManaForSkill(skillType))
	{
		UIManager::Instance().GetUIElementByEnum(UIEnumID::ErrorPopup_Cooltime)->EnableElement();
		return;
	}
	if (state != State::Battle) return;
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
		case SkillType::ROBIN_W: ActivateSkill(SkillType::ROBIN_W, characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition()); break;
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
			for (auto& each : characters)
			{
				each.lock()->playingBattleAnim = false;
			}
			break;
		case State::Battle:
			for (auto& each : characters)
			{
				each.lock()->playingBattleAnim = true;
			}
			break;
		case State::Tactic:
			UnSelectSkill();
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
	SetState(State::Battle);
}

void PlayerController::OnWaveEnd(std::weak_ptr<PlaytimeWave> p_wave)
{
	SetState(State::Peace);
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
	if (selectedSkill != SkillType::NONE)
		SkillPreviewSystem::Instance().HideSkillMaxRange();
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

void PlayerController::ResetCombo()
{
}
void PlayerController::SetMana(float mana)
{
	const auto& gc = GlobalConstant::GetSingletonInstance().pod;
	this->mana = std::fmin(gc.maxMana, mana);
	UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaFill)->adjuster->SetTargetFloat(1 - mana / gc.maxMana);
}

void PlayerController::SetCooltime(SkillType::Enum skillType, float cooltime)
{
	skillCooltimeLeft[skillType] = std::fmax(0.0f, cooltime);
	skillCooltimeNumberUI[skillType]->SetNumber(cooltime);
	skillCooltimeMaskUI[skillType]->adjuster->SetTargetFloat(skillCooltimeLeft[skillType] / GetCooltimeForSkill(skillType));
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
		allyHoverEffect->SetParent(unit.lock()->GetGameObject());

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
		enemyHoverEffect->SetParent(unit.lock()->GetGameObject());
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
	}
	{
		for (auto& each : enemyTargetedEffect)
		{
			each = Scene::getCurrentScene()->AddGameObjectFromFBX("VFX_AttackSelected");
			each->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ENEMY_COLOR);
			each->AddComponent<VFXAnimator>()->Init();
		}
	}
	{
		allyHoverEffect = Scene::getCurrentScene()->AddGameObjectFromFBX("VFX_CharacterSelected");
		allyHoverEffectRenderer = allyHoverEffect->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		allyHoverEffectRenderer->GetGI().GetMaterial()->SetPixelShader(_resourceManager->GetShader(L"VFX_PS.cso"));

		auto vfx = allyHoverEffect->AddComponent<VFXAnimator>();
		vfx->Init();
		vfx->SetLoop(true);
	}
	{
		enemyHoverEffect = Scene::getCurrentScene()->AddGameObjectFromFBX("VFX_CharacterSelected");
		enemyHoverEffectRenderer = enemyHoverEffect->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		enemyHoverEffectRenderer->GetGI().GetMaterial()->SetPixelShader(_resourceManager->GetShader(L"VFX_PS.cso"));
		enemyHoverEffectRenderer->GetGI().GetMaterial()->SetColor(wanderResources::unitColor::ENEMY_COLOR);

		auto vfx = enemyHoverEffect->AddComponent<VFXAnimator>();
		vfx->Init();
		vfx->SetLoop(true);
	}
}
