/// 2024. 06. 08 김상준
/// 소환용 Rook

#pragma once

#include "YunutyEngine.h"
#include "PermanentObservee.h"
#include "ChessObject.h"

class Unit;
class BossSummonChessSkill;

namespace BossSummon
{
	class ChessRook
		: public Component, public PermanentObservee, public ChessObject
	{
		friend class ChessPool;
		friend class BossSummonChessSkill;

	public:
		virtual void Init(application::editor::ITemplateData* templateData) override;
		virtual void OnSummon() override;
		virtual void OnReturn() override;
		virtual void OnBossDie() override;
		virtual void SetReady() override;

		virtual void Update() override;

		virtual void OnTriggerEnter(physics::Collider* collider) override;
		virtual void OnTriggerExit(physics::Collider* collider) override;

		yunutyEngine::coroutine::Coroutine DoInteraction();

		virtual Component* GetComponent() override { return this; }
		virtual Component* GetSummonComponent() override { return this; }
	};

}
