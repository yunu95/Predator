#include "NailAnimatorManager.h"

#include "NailAnimator.h"

LazyObjects<NailAnimatorManager> NailAnimatorManager::Instance;
unsigned int NailAnimatorManager::animatorID = 0;

void NailAnimatorManager::PushAnimator(std::shared_ptr<NailAnimator> animator)
{
	animator->SetID(animatorID++);
	this->animatorVec.emplace_back(animator);
}
