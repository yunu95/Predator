#pragma once
#include "ContentsObservee.h"

/// <summary>
/// 해당 클래스를 상속받는 컴포넌트는
/// 에디터 실행 정지 및 인게임 Restart 시에도 사라지지 않습니다.
/// </summary>
class PermanentObservee
	: public ContentsObservee
{
public:
	virtual ~PermanentObservee() = default;
	PermanentObservee();

	/// Editor 상에서 Contents 가 Play 될 때, 호출하는 함수입니다.
	/// 기본적으로는 해당 Component 의 Active 를 활성화 한 뒤,
	/// Start 함수를 한 번 호출하게 됩니다.
	/// Show 등의 특별한 동작을 위해서는 override 하여 재정의 합니다.
	virtual void OnContentsPlay() { GetComponent()->SetActive(true); GetComponent()->Start(); }

	/// Editor 상에서 Contents 가 Stop 될 때, 호출하는 함수입니다.
	/// 기본적으로는 해당 Component 의 Active 에만 관여하며,
	/// Hide 등의 특별한 동작을 위해서는 override 하여 재정의 합니다.
	virtual void OnContentsStop() { GetComponent()->SetActive(false); }
};

