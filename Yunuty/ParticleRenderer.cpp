#include "YunutyEngine.h"
#include "ParticleRenderer.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::ParticleRenderer::ParticleRenderer() :
	Renderable<yunuGI::IParticleRenderer>(_YunuGIObjects::SingleInstance().factory->CreateParticleRenderer({}))
{
	disableParticles.assign(this->maxParticle, yunuGI::ParticleRenderInfo{});
	GetGI().SetParticleInfoList(this->ableParticles);
}

void ParticleRenderer::SetMaxParticle(unsigned int maxParticle)
{
	this->maxParticle = maxParticle;
}

void ParticleRenderer::Play()
{
	isPlay = true;
}

void ParticleRenderer::Update()
{
	auto& gi = GetGI();

	// 현재 파티클이 재생된 총 시간량이 Duration보다 작을 때 만 Update 돌기
	if ((accTime <= gi.GetDuration()) && isPlay)
	{
		accTime += Time::GetDeltaTime();
		curCreationCycle += Time::GetDeltaTime();

		// 만일 입자를 생성할 시기가 되면 설정해둔 입자 생성 갯수만큼 활성
		if (curCreationCycle >= (1.f / rateOverTime))
		{
			curCreationCycle -= (1.f / rateOverTime);

			// 이제 입자 활성화
			auto& particle = this->disableParticles.front();
			particle.position = GetTransform()->GetLocalPosition();
			this->disableParticles.pop_front();

			this->ableParticles.push_back(particle);
		}
		// 생성된 입자들은 포지션, 스케일 업데이트
		for (auto iter = this->ableParticles.begin(); iter != this->ableParticles.end();)
		{
			auto& each = *iter;

			each.curLifeTime += Time::GetDeltaTime();

			// 생성된 입자의 생명주기가 다하면 다시 비활성
			if (each.curLifeTime >= each.lifeTime)
			{
				each.curLifeTime = 0;
				each.Reset();
				this->disableParticles.push_back(*iter);

				iter = this->ableParticles.erase(iter);

				continue;
			}

			each.position += (yunuGI::Vector3{ 0,1,0 }*Time::GetDeltaTime());
			each.scale = 1.f;
			++iter;
		}
	}
	else
	{
		isPlay = false;

		if (isLoop)
		{
			isPlay = true;
			accTime = 0.f;
		}
	}
}