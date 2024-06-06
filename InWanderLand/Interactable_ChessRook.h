/// 2024. 05. 17 김상준
/// Chess_Rook InteractableComponent

#pragma once

#include "IInteractableComponent.h"

class Interactable_ChessRook
	: public IInteractableComponent
{
	friend class Interactable_ChessRookPool;
	friend class BossSummonChessSkill;

public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnTriggerEnter(physics::Collider* collider) override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override;

	virtual void SetDataFromEditorData(const application::editor::InteractableData& data) override;

private:
	Vector3d initPos = Vector3d(0, 0, 0);
	Quaternion initRotation = Quaternion();
	Vector3d initScale = Vector3d(1, 1, 1);
	bool isSummoned = false;
	float chessSummonedExplosionDelay = 0;
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
	float localSummonedTime = 0;
};

