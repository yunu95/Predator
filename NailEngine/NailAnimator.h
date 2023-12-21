#pragma once

#include "IAnimator.h"

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
	};

	void SetPlaySpeed(float playSpeed)
	{
		this->playSpeed = playSpeed;
	};

	float GetPlaySpeed()
	{
		return this->playSpeed;
	}

	void SetMesh(yunuGI::IMesh* mesh) {};

	void SetModel(const std::wstring& modelName) 
	{
		this->modelName = modelName;
	};

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
		this->currentFrame = frame;
	};

	void SetFrameRatio(float ratio)
	{
		this->frameRatio = ratio;
	};

	yunuGI::IAnimation* GetCurrentAnimation() 
	{
		return this->currentAnimation;
	};

	int GetCurrentFrame()
	{
		return currentFrame;
	}

	float GetFrameRatio()
	{
		return frameRatio;
	}

private:
	float playSpeed = 1.f;
	int currentFrame;
	int nextFrame;
	float frameRatio;

	yunuGI::IAnimation* currentAnimation;
	std::set<yunuGI::IAnimation*> animationSet;

	std::wstring modelName;

	unsigned __int64 id;

};

