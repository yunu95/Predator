/// 2024. 06. 09 김상준
/// 소환용 Frame
/// 조건이 충족 되었을 때, Unit 으로 변경합니다.

#pragma once

#include "YunutyEngine.h"
#include "ContentsObservee.h"
#include "BossSummonObject.h"

namespace application
{
	namespace editor
	{
		class Unit_TemplateData;
	}
}

class Unit;
class BossSummonMobSkill; 
namespace BossSummon
{
	class RightFrame
		: public Component, public ContentsObservee, public BossSummonObject
	{
		friend class BossSummonMobSkill;

	public:
		virtual ~RightFrame();
		virtual void Init(application::editor::ITemplateData* templateData) override;
		virtual void OnSummon() override;
		virtual void OnReturn() override {}
		virtual void OnBossAppear();
		virtual void OnBossDie() override;

		virtual void Update() override;

		void SetFrameData(application::editor::UnitData* frameData);
		/// Unit Frame 으로 변경합니다.
		void ChangeUnit();
		void SummonUnit();

		virtual Component* GetComponent() override { return this; }
		virtual Component* GetSummonComponent() override { return this; }

		virtual void OnPause() override;
		virtual void OnResume() override;

		virtual void Recovery() override;

		bool HasChangedUnit() const { return !unitFrame.expired(); }
		bool IsAlive() const;

	private:
		coroutine::Coroutine OnAppear();
		coroutine::Coroutine SummonMoldUnit();

		struct CustomCompUnit
		{
			bool operator()(const std::weak_ptr<Unit>& lp, const std::weak_ptr<Unit>& rp) const
			{
				return lp.lock().get() > rp.lock().get();
			}
		};

		std::weak_ptr<coroutine::Coroutine> summonCorountine = std::weak_ptr<coroutine::Coroutine>();
		static std::weak_ptr<ManagedFBX> summonEffect;
		static std::weak_ptr<ManagedFBX> summoningEffect;
		std::weak_ptr<VFXAnimator> summonEffectAnimator;
		std::weak_ptr<VFXAnimator> summoningEffectAnimator;
		GameObject* mesh = nullptr;
		application::editor::UnitData* frameData = nullptr;
		std::weak_ptr<Unit> unitFrame = std::weak_ptr<Unit>();
		std::set<std::weak_ptr<Unit>, CustomCompUnit> summonUnit = std::set<std::weak_ptr<Unit>, CustomCompUnit>();
		static application::editor::Unit_TemplateData* meleeUnitMold;
		static application::editor::Unit_TemplateData* weakMeleeUnitMold;
		static application::editor::Unit_TemplateData* eliteMeleeUnitMold;

		static application::editor::Unit_TemplateData* projectileUnitMold;
		static application::editor::Unit_TemplateData* weakProjectileUnitMold;
		static application::editor::Unit_TemplateData* weakApproachProjectileUnitMold;
		static application::editor::Unit_TemplateData* kitingProjectileUnitMold;
		static application::editor::Unit_TemplateData* siegeProjectileUnitMold;
		static application::editor::Unit_TemplateData* eliteProjectileUnitMold;
		bool isPause = false;
	};

}
