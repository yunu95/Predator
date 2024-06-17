/// 2024. 05. 17 김상준
/// Chess_Rook InteractableComponent

#pragma once

#include "IInteractableComponent.h"
#include "ITacticObject.h"

class Unit;
class Interactable_ChessRook
	: public IInteractableComponent, public ITacticObject
{
	friend class Interactable_ChessRookPool;
	friend class BossSummonChessSkill;

public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnTriggerEnter(physics::Collider* collider) override;
	virtual void OnTriggerExit(physics::Collider* collider) override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override;

	virtual void SetDataFromEditorData(const application::editor::InteractableData& data) override;

	virtual void OnPause() override;
	virtual void OnResume() override;

private:
	Vector3d initPos = Vector3d(0, 0, 0);
	Quaternion initRotation = Quaternion();
	Vector3d initScale = Vector3d(1, 1, 1);
	std::unordered_set<Unit*> unitSet = std::unordered_set<Unit*>();
	float chessBlockUnitLength = 0;
	float chessBlockUnitOffset = 0;
	float damage = 0;
	float delayTime = 0;
	float particleEffectTime = 0;
	float vibeMaxOffset = 0;
	GameObject* mesh = nullptr;
	yunuGI::ITexture* orginTexture = nullptr;
	yunuGI::ITexture* flashTexture = nullptr;
	std::vector<GameObject*> bombObjList = std::vector<GameObject*>();
	float guideUp_Y = 0.01;
	bool isPause = false;
};

