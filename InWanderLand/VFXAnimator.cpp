#include "VFXAnimator.h"

void VFXAnimator::Start()
{
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto child = GetGameObject()->GetChildren();
	for (auto& each : child)
	{
		if (auto staticMesh = each->GetComponent<graphics::StaticMeshRenderer>(); staticMesh)
		{
			renderer = &staticMesh->GetGI();
		}
		else if(auto skinnedMesh = each->GetComponent<graphics::SkinnedMesh>(); skinnedMesh)
		{
			renderer = &each->GetComponent<graphics::SkinnedMesh>()->GetGI();
		}
		if (renderer)
		{
			this->renderer = renderer;

			for (int i = 0; i < renderer->GetMaterialCount(); ++i)
			{
				auto name = renderer->GetMaterial(i)->GetName();
				auto temp = _resourceManager->GetVFXInfo(name);
				this->frameRate = temp.first;
				this->frameInfoVec.push_back(temp.second);



				if (auto staticMesh = each->GetComponent<graphics::StaticMeshRenderer>(); staticMesh)
				{
					renderer->GetMaterial(i)->SetVertexShader(_resourceManager->GetShader(L"TextureAnimVS.cso"));
				}
				else if (auto skinnedMesh = each->GetComponent<graphics::SkinnedMesh>(); skinnedMesh)
				{
					renderer->GetMaterial(i)->SetVertexShader(_resourceManager->GetShader(L"SkinnedVFX_VS.cso"));
				}
				
				renderer->GetMaterial(i)->SetPixelShader(_resourceManager->GetShader(L"VFX_PS.cso"));
			}
		}
		this->curFrameVec.resize(this->frameInfoVec.size());
		break;
	}
}

void VFXAnimator::Update()
{
	for (int i = 0; i < this->frameInfoVec.size(); ++i)
	{
		int totalframe = this->frameInfoVec[i].size();
		float duration = totalframe / this->frameRate;
		__int32 ratio = static_cast<__int32>(totalframe / duration);

		this->curFrameVec[i].sumTime += Time::GetDeltaTime();

		for (int j = 0; j < this->frameInfoVec[i].size(); ++j)
		{
			if (this->curFrameVec[i].sumTime >= duration)
			{
				continue;
				/*if (this->isLoop)
				{
					this->Reset();
				}*/
			}

			this->curFrameVec[i].curFrame = static_cast<__int32>(this->curFrameVec[i].sumTime * ratio);
			this->curFrameVec[i].curFrame = min(static_cast<int>(this->curFrameVec[i].curFrame), totalframe - 1);
			this->curFrameVec[i].nextFrame = min(static_cast<int>(this->curFrameVec[i].curFrame + 1), totalframe - 1);
			float lerpratio = static_cast<float>(this->curFrameVec[i].sumTime - static_cast<float>(this->curFrameVec[i].curFrame) / ratio);


			auto lerLocation = yunuGI::Vector2::Lerp(this->frameInfoVec[i][this->curFrameVec[i].curFrame].location, this->frameInfoVec[i][this->curFrameVec[i].nextFrame].location, lerpratio);
			this->renderer->GetMaterial(i)->SetFloat(0, lerLocation.x);
			this->renderer->GetMaterial(i)->SetFloat(1, lerLocation.y);
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
	Reset();
}

void VFXAnimator::OnDisable()
{

}

void VFXAnimator::Reset()
{
	for (auto& each : this->curFrameVec)
	{
		each.curFrame = 0;
		each.nextFrame = 0;
		each.sumTime = 0.f;
	}
}
