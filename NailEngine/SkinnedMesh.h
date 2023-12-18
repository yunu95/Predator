#pragma once

#include "IRenderable.h"

class SkinnedMesh : public IRenderable
{
public:
	void SetBone(std::wstring fbxName)
	{
		this->fbxName = fbxName;
	}

	const std::wstring& GetBone()
	{
		return this->fbxName;
	}

	void SetAnimatorIndex(unsigned int index)
	{
		this->animatorIndex = index;
	}

	unsigned int GetAnimatorIndex()
	{
		return this->animatorIndex;
	}


private:
	std::wstring fbxName;
	unsigned int animatorIndex;
};

