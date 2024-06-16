/// 2024. 06. 08 김상준
/// Boss Skill 로 소환하는 대상을 위한 공통 인터페이스

#pragma once

#include "YunutyEngine.h"
#include "ITacticObject.h"

#include <memory>

namespace application
{
	namespace editor
	{
		class ITemplateData;
	}
}

namespace BossSummon
{
	class BossSummonObject
		: public ITacticObject
	{
	public:
		/// Pool 에서 templateData 를 기준으로 필요한 데이터를 초기화하는 함수입니다.
		/// 한 번만 불리기 때문에, Component 를 부착하는 등의 작업만 수행하길 권장하며,
		/// 갱신될 데이터들은 OnSummon 에서 다시 정리하길 권장합니다.
		virtual void Init(application::editor::ITemplateData* templateData) = 0;
		/// 소환이 되었을 때, 특별한 작업이 필요한 경우 호출하는 함수입니다.
		virtual void OnSummon() = 0;
		/// Pool 을 사용하기 때문에, Pool 에 반납할 때 호출할 함수입니다.
		virtual void OnReturn() = 0;
		/// Boss 가 등장할 때, 호출할 함수입니다.
		virtual void OnBossAppear() {};
		/// Boss 가 죽게 되었을 때, 호출할 함수입니다.
		virtual void OnBossDie() {};

		/// 해당 BossSummonObject 를 상속받는 구체화된 Component 에서
		/// 자기 자신을 반환하여 이를 통한 작업이 가능하도록 해야합니다.
		virtual Component* GetSummonComponent() = 0;

		virtual void OnPause() = 0;
		virtual void OnResume() = 0;

	protected:
		/// Pool 로 관리할 때, 사용하기 위한 weak_ptr 입니다.
		std::weak_ptr<BossSummonObject> myWeakPtr = std::weak_ptr<BossSummonObject>();
	};
}
