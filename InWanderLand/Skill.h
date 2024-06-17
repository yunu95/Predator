#pragma once
#include "YunutyEngine.h"
#include "SkillType.h"
#include "ITacticObject.h"

class VFXAnimator;
class Unit;
class Skill
    : public ITacticObject
{
protected:
    Vector3d targetPos;
    std::weak_ptr<Unit> owner;
    std::weak_ptr<Skill> selfWeakPtr;
public:
    virtual ~Skill() = default;
    virtual coroutine::Coroutine operator()() = 0;
    virtual SkillType::Enum GetSkillType() { return SkillType::NONE; };
    virtual float GetCastRange() { return 8; };

    virtual void OnInterruption() {};

    virtual void OnPause() override {}
    virtual void OnResume() override {}

    static bool SkillPodFieldPreEncoding(SkillType::Enum type, json& data);
    static bool SkillPodFieldPostEncoding(SkillType::Enum type, json& data);
    static bool SkillPodFieldPreDecoding(SkillType::Enum type, const json& data);
    static bool SkillPodFieldPostDecoding(SkillType::Enum type, const json& data);
    static void DrawSkillPod(SkillType::Enum type);

    friend Unit;
};

/// POD 파일을 통해 Field Reflection 을 지원하는 템플릿 함수입니다.
/// Skill 을 작성한 사람이 해당 내용들을 특수화 합니다.
template <SkillType::Enum skillType>
bool SkillPodFieldPreEncoding(json& data)
{
	return true;
}

template <SkillType::Enum skillType>
bool SkillPodFieldPostEncoding(json& data)
{
	return true;
}

template <SkillType::Enum skillType>
bool SkillPodFieldPreDecoding(const json& data)
{
	return true;
}

template <SkillType::Enum skillType>
bool SkillPodFieldPostDecoding(const json& data)
{
	return true;
}

