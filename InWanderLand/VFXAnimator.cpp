#include "VFXAnimator.h"

#include <algorithm>

void VFXAnimator::Start()
{
	if (isInit == false)
	{
		Init();
	}
}

void VFXAnimator::Update()
{
	if (this->isPlay)
	{
		for (int i = 0; i < this->frameInfoVec.size(); ++i)
		{
			if (this->frameInfoVec[i].empty())
			{
				this->curFrameVec[i].isDone = true;
				this->curFrameVec[i].sumTime = 0.f;
				continue;
			}

			int totalFrame = this->frameInfoVec[i].size();
			float duration = static_cast<float>(totalFrame) / this->frameRate;
			float frameDuration = duration / static_cast<float>(totalFrame);

			this->curFrameVec[i].sumTime += Time::GetDeltaTime() * this->speed;

			if (this->curFrameVec[i].sumTime >= duration)
			{
				if (this->isLoop == false)
				{
					this->curFrameVec[i].isDone = true;
					continue;
				}
				else
				{
					this->curFrameVec[i].sumTime = 0.f;
				}
			}

			this->curFrameVec[i].curFrame = static_cast<__int32>(this->curFrameVec[i].sumTime / frameDuration);
			this->curFrameVec[i].curFrame = min(static_cast<int>(this->curFrameVec[i].curFrame), totalFrame - 1);
			this->curFrameVec[i].nextFrame = min(static_cast<int>(this->curFrameVec[i].curFrame + 1), totalFrame - 1);

			float lerpratio = (this->curFrameVec[i].sumTime - (this->curFrameVec[i].curFrame * frameDuration)) / frameDuration;

			auto lerLocation = yunuGI::Vector2::Lerp(
				this->frameInfoVec[i][this->curFrameVec[i].curFrame].location,
				this->frameInfoVec[i][this->curFrameVec[i].nextFrame].location,
				lerpratio
			);

			this->renderer->GetMaterial(i)->SetFloat(0, lerLocation.x);
			this->renderer->GetMaterial(i)->SetFloat(1, lerLocation.y);
		}

		for (auto& each : this->curFrameVec)
		{
			if (each.isDone == false)
			{
				continue;
			}
			this->isPlayDone = true;
			if (this->isAutoActivFalse == true)
			{
				GetGameObject()->SetSelfActive(false);
			}
		}
	}
}

void VFXAnimator::OnEnable()
{
	if (isInit == false)
	{
		Init();
	}
	Reset();
}

void VFXAnimator::OnDisable()
{

}

void VFXAnimator::Init()
{
	if (isInit)
	{
		return;
	}

	isInit = true;

	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto child = GetGameObject()->GetChildren();
	for (auto& each : child)
	{
		if (auto staticMesh = each->GetComponent<graphics::StaticMeshRenderer>(); staticMesh)
		{
			renderer = &staticMesh->GetGI();
		}
		else if (auto skinnedMesh = each->GetComponent<graphics::SkinnedMesh>(); skinnedMesh)
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
				if (this->frameRate == 0.f)
				{
					this->frameRate = temp.first;
				}
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

	for (int j = 0; j < this->frameInfoVec.size(); ++j)
	{
		if (frameInfoVec[j].empty())
		{
			continue;
		}
		this->renderer->GetMaterial(j)->SetFloat(0, frameInfoVec[j][0].location.x);
		this->renderer->GetMaterial(j)->SetFloat(1, frameInfoVec[j][0].location.y);
	}
}

void VFXAnimator::SetAutoActiveFalse()
{
	this->isAutoActivFalse = false;
}

bool VFXAnimator::IsDone()
{
	return this->isPlayDone;
}

void VFXAnimator::Play()
{
	this->isPlay = true;
	this->Reset();
}

void VFXAnimator::Pause()
{
	this->isPlay = false;
}

void VFXAnimator::Resume()
{
	this->isPlay = true;
}

float VFXAnimator::GetDuration()
{
	float duration = 0.f;
	for (int i = 0; i < this->frameInfoVec.size(); ++i)
	{
		int totalframe = this->frameInfoVec[i].size();

		float durationgh = totalframe / this->frameRate;

		duration = std::max(duration, durationgh);
	}

	return duration;
}

void VFXAnimator::Reset()
{
	for (auto& each : this->curFrameVec)
	{
		each.curFrame = 0;
		each.nextFrame = 0;
		each.sumTime = 0.f;
		each.isDone = false;
	}
	this->isPlayDone = false;

	for (int j = 0; j < this->frameInfoVec.size(); ++j)
	{
		if (frameInfoVec[j].empty())
		{
			continue;
		}
		this->renderer->GetMaterial(j)->SetFloat(0, frameInfoVec[j][0].location.x);
		this->renderer->GetMaterial(j)->SetFloat(1, frameInfoVec[j][0].location.y);
	}
}
