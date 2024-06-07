#pragma once
#include "YunutyEngine.h"
#include "Unit.h"

// 해당 클래스는 스킬프리뷰시스템에서만 사용할 오브젝트풀입니다.
class AttackPreviewPool : public SingletonClass<AttackPreviewPool>, public GameObjectPool<yunutyEngine::graphics::StaticMeshRenderer>
{
public:
	virtual void ObjectInitializer(std::weak_ptr<graphics::StaticMeshRenderer> comp) override;
};