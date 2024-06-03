///// 2024. 04. 19 김상준
///// Tutorial 관련 처리를 위한 Singleton Manager 입니다.
//
//#pragma once
//
//#include "YunutyEngine.h"
//#include "SingletonComponent.h"
//
//#include "PlayableComponent.h"
//
//#include "Unit.h"
//
//namespace application
//{
//	class TutorialManager
//		: public PlayableComponent, public Component, public SingletonComponent<TutorialManager>
//	{
//		/// friend class 전술모드?
//
//	public:
//		virtual void SingletonInitializer() override;
//
//		virtual void Update() override;
//
//		virtual void PreMapLoad() override;
//		virtual void OnGameStart() override;
//		virtual void OnGameStop() override;
//
//		bool IsTutorialMode() { return isTutorialMode; }
//		void SetTutorialMode(bool mode);
//
//	private:
//		/// 모든 Unit 에 대한 정지 명령을 가하는 함수입니다.
//		void AllStop();
//
//		/// 모든 Unit 들을 다시 활성화하는 함수입니다.
//		/// Push 한 Target 의 이벤트가 발생했을 때, 호출해줄 함수입니다.
//		void AllResume();
//
//		/// 특정 유닛의 특정 스킬에 대한 활성화를 제공하는 함수입니다.
//		/// switch 문에서 제공하지 않는 Target 의 경우 아무 일도 발생하지 않습니다.
//		void PushTutorialTarget(Unit::UnitType targetUnit, Unit::SkillEnum targetSkill);
//
//		/// 특정 유닛의 특정 스킬에 대한 활성화를 리스트에서 빼는 함수입니다.
//		/// 굳이 제공할 필요가 없어 보입니다.
//		// void RemoveTutorialTarget(Unit::UnitType targetUnit, Unit::SkillEnum targetSkill);
//
//		/// UI 관련... 고민 중입니다...
//		void PopTutorialUI(unsigned long index);
//
//
//		bool isTutorialMode = false;
//		unsigned long tutorialIndex = 0;
//	};
//}

