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

        nextAnimation = animation;
        transitionDesc.next.animIndex = ((Animation*)(animation))->GetAnimationIndex();
        //transitionDesc.next.speed = transitionSpeed;
        transitionDesc.transitionDuration = transitionDuration;
        transitionDesc.transitionSpeed = transitionSpeed;
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

