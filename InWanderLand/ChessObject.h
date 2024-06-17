/// 2024. 06. 08 김상준
/// 소환용 ChessObject 들만의 Interface 제공을 위한 중간 계층 클래스

#pragma once

#include "BossSummonObject.h"

class Unit;
namespace BossSummon
{
	class ChessObject
		: public BossSummonObject
	{
	public:
		virtual void Init(application::editor::ITemplateData* templateData) = 0;
		virtual void OnSummon() = 0;
		virtual void OnReturn() = 0;
		virtual void OnBossDie() = 0;
		virtual void SetReady() = 0;
		virtual void StartTimer() { timerStart = true; }

		virtual void OnPause() = 0;
		virtual void OnResume() = 0;

	protected:
		bool ready = false;
		bool timerStart = false;
		bool triggerOn = false;
		bool isInteracting = false;
		std::weak_ptr<yunutyEngine::coroutine::Coroutine> lastCoroutine = std::weak_ptr<yunutyEngine::coroutine::Coroutine>();
		std::unordered_set<Unit*> unitSet = std::unordered_set<Unit*>();
		float chessSummonedExplosionDelay = 0;
		float damage = 0;
		float delayTime = 0;
		float particleEffectTime = 0;
		GameObject* mesh = nullptr;
		yunuGI::ITexture* orginTexture = nullptr;
		yunuGI::ITexture* flashTexture = nullptr;
		GameObject* particleObj = nullptr;
		std::vector<GameObject*> bombObjList = std::vector<GameObject*>();
		float guideUp_Y = 0.01;
		float localSummonedTime = 0;
		bool isPause = false;
	};
}
