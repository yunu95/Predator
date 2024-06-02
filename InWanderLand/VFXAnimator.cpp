#include "VFXAnimator.h"

void VFXAnimator::Start()
{
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto child = GetGameObject()->GetChildren();
	for (auto& each : child)
	{
		auto renderer = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		if (renderer)
		{
			this->renderer = renderer;

			for (int i = 0; i < renderer->GetGI().GetMaterialCount(); ++i)
			{
				auto temp = _resourceManager->GetVFXInfo(renderer->GetGI().GetMaterial(i)->GetName());
				this->frameRate = temp.first;
				this->frameInfo.push_back(temp.second);
			}
		}
	}

	this->curFrameVec.resize(this->frameInfo.size());
}

void VFXAnimator::Update()
{
	for (int i = 0; i < this->frameInfo.size(); ++i)
	{
		int totalframe = this->frameInfo[i].back().frame + 1;
		float duration = totalframe / this->frameRate;

		for (auto& each : this->frameInfo[i])
		{

		}
	}


	//auto currentAnimation = gi.GetCurrentAnimation();
	//float duration = currentAnimation->GetDuration();
	//int totalFrame = currentAnimation->GetTotalFrame();
	//desc.curr.speed = currentAnimation->GetPlaySpeed();
	//__int32 ratio = static_cast<__int32>(totalFrame / duration);

	//desc.curr.sumTime += (desc.curr.speed * Time::GetDeltaTime());
	//if (desc.curr.sumTime >= currentAnimation->GetDuration())
	//{
	//	if (currentAnimation->GetLoop())
	//	{
	//		desc.curr.sumTime -= currentAnimation->GetDuration();
	//	}
	//}

	//desc.curr.currFrame = static_cast<__int32>(desc.curr.sumTime * ratio);
	//desc.curr.currFrame = min(static_cast<int>(desc.curr.currFrame), totalFrame - 1);
	//desc.curr.nextFrame = min(static_cast<int>(desc.curr.currFrame + 1), totalFrame - 1);
	//desc.curr.ratio = static_cast<float>(desc.curr.sumTime - static_cast<float>(desc.curr.currFrame) / ratio);
}

void VFXAnimator::OnEnable()
{

}

void VFXAnimator::OnDisable()
{

}

void VFXAnimator::Reset()
{

}
