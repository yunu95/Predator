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
		class UnitData;
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
		virtual void OnBossDie() override;

		virtual void Update() override;

		void SetFrameData(application::editor::UnitData* frameData);
		/// Unit Frame 으로 변경합니다.
		void ChangeUnit();
		void SummonUnit();

		virtual Component* GetComponent() override { return this; }
		virtual Component* GetSummonComponent() override { return this; }

		static void RegisterUnitData(application::editor::UnitData* unitData);

		bool HasChangedUnit() const { return !unitFrame.expired(); }
		bool IsAlive() const;

	private:
		coroutine::Coroutine SummonMoldUnit();

		struct CustomCompUnit
		{
			bool operator()(const std::weak_ptr<Unit>& lp, const std::weak_ptr<Unit>& rp) const
			{
				return lp.lock().get() > rp.lock().get();
			}
		};

		GameObject* mesh = nullptr;
		application::editor::UnitData* frameData = nullptr;
		std::weak_ptr<Unit> unitFrame = std::weak_ptr<Unit>();
		std::set<std::weak_ptr<Unit>, CustomCompUnit> summonUnit = std::set<std::weak_ptr<Unit>, CustomCompUnit>();
		static std::vector<application::editor::UnitData*> mold;
	};

}
