#pragma once

#include "IAnimation.h"
#include "IMesh.h"
#include "Animation.h"
#include "Struct.h"

#include <set>

class NailAnimator
{
public:
    void PushAnimation(yunuGI::IAnimation* animation)
    {
        this->animationSet.insert(animation);
    };

    void Play(yunuGI::IAnimation* animation)
    {
        this->currentAnimation = animation;
        this->transitionDesc.curr.animIndex = ((Animation*)animation)->GetAnimationIndex();
        this->transitionDesc.curr.currFrame = 0;
        this->transitionDesc.curr.nextFrame = 0;
        this->transitionDesc.curr.ratio = 0;
        this->transitionDesc.curr.sumTime = 0;
    };

    void SetAnimationFrame(yunuGI::IAnimation* animation, unsigned int frame)
    {
        this->currentAnimation = animation;
        this->transitionDesc.curr.animIndex = ((Animation*)animation)->GetAnimationIndex();
        this->transitionDesc.curr.currFrame = frame;
    }

    void SetPlaySpeed(float playSpeed)
    {
        animatorSpeed = playSpeed;
        //this->transitionDesc.curr.speed = playSpeed;
    };

    float GetPlaySpeed()
    {
        return animatorSpeed;
        //return this->transitionDesc.curr.speed;
    }

    void SetMesh(yunuGI::IMesh* mesh) {};

    void SetModel(const std::wstring& modelName)
    {
        this->modelName = modelName;
    };

    const std::wstring& GetModel() { return this->modelName; }

    void SetID(unsigned int id)
    {
        this->id = id;
    }

    unsigned int GetID()
    {
        return this->id;
    }

    void SetCurrentFrame(int frame)
    {
        this->transitionDesc.curr.currFrame = frame;
    };

    void SetFrameRatio(float ratio)
    {
        this->transitionDesc.curr.ratio = ratio;
    };

    yunuGI::IAnimation* GetCurrentAnimation()
    {
        return this->currentAnimation;
    };

    int GetCurrentFrame()
    {
        return this->transitionDesc.curr.currFrame;
    }

    float GetFrameRatio()
    {
        return this->transitionDesc.curr.ratio;
    }

    TransitionDesc& GetTransitionDesc()
    {
        return this->transitionDesc;
    }
    void SetTransitionDesc(TransitionDesc& transitionDesc) { this->transitionDesc = transitionDesc; }

    void ChangeAnimation(yunuGI::IAnimation* animation, float transitionDuration, float transitionSpeed)
    {
        if (currentAnimation == animation) return;

        // 변경 요청이 들어오면 현재 재생중인 애니메이션은 다음 애니메이션이 되고 요청으로 들어온 애니메이션을 현재 애니메이션으로 설정한다.
        // 다음 애니메이션을 현재 애니메이션으로
        nextAnimation = currentAnimation;
        // 현재 애니메이션을 요청 들어온 애니메이션으로 설정
        currentAnimation = animation;

        // 다음 애니메이션에 대한 Desc를 현재 Desc로 설정
        transitionDesc.next.animIndex = ((Animation*)(nextAnimation))->GetAnimationIndex();
        transitionDesc.next.currFrame = transitionDesc.curr.currFrame;
        transitionDesc.next.nextFrame = transitionDesc.curr.currFrame;
        transitionDesc.next.ratio = transitionDesc.curr.ratio;
        transitionDesc.next.sumTime = transitionDesc.curr.sumTime;
        transitionDesc.next.speed = transitionDesc.curr.speed;
        transitionDesc.transitionDuration = transitionDuration;
        transitionDesc.transitionSpeed = transitionSpeed;

        // 현재 애니메이션에 대한 Desc를 요청 들어온 애니메이션의 Desc로 설정
        transitionDesc.curr.animIndex = ((Animation*)(currentAnimation))->GetAnimationIndex();
        transitionDesc.curr.currFrame = 0;
        transitionDesc.curr.nextFrame = 1;
        transitionDesc.curr.ratio = 0.f;
        transitionDesc.curr.sumTime = 0.f;
        transitionDesc.curr.speed = currentAnimation->GetPlaySpeed();
    };

    void SetCurrentAnimation(yunuGI::IAnimation* animation)
    {
        currentAnimation = animation;
    };

    void SetNextAnimation(yunuGI::IAnimation* animation)
    {
        nextAnimation = animation;
    };

    yunuGI::IAnimation* GetNextAnimation()
    {
        return nextAnimation;
    };

private:
    float animatorSpeed{ 1.0f };
    TransitionDesc transitionDesc;

    yunuGI::IAnimation* currentAnimation;
    yunuGI::IAnimation* nextAnimation;
    std::set<yunuGI::IAnimation*> animationSet;

    std::wstring modelName;

    unsigned __int64 id;

};

