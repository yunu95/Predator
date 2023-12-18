#pragma once

#include "Utils.h"

#include <memory>
#include <vector>
#include <cassert>

class NailAnimator;

class NailAnimatorManager
{
public:
	static LazyObjects<NailAnimatorManager> Instance;
	friend LazyObjects<NailAnimatorManager>;

public:
	void PushAnimator(std::shared_ptr<NailAnimator> animator);

	std::shared_ptr<NailAnimator> GetAnimator(unsigned int index)
	{
		if (index >= animatorVec.size())
		{
			assert(FALSE);
		}

		return animatorVec[index];
	}
public:
	static unsigned int animatorID;

private:
	std::vector<std::shared_ptr<NailAnimator>> animatorVec;
};

