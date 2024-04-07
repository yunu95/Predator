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

yunuGI::Vector3 ParticleRenderer::GenerateRandomDirectionInCone(float angle)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	// Cone의 반지름 및 높이 계산
	float coneRadius = tan(angle);
	float coneHeight = 1.0f;

	// 무작위 각도 및 반지름 생성
	float theta = dist(gen) * 6.283185307f;
	float r = sqrt(dist(gen));

	// 무작위로 선택된 방향 생성
	float x = coneRadius * r * cos(theta);
	float y = coneHeight * r;
	float z = coneRadius * r * sin(theta);

	return yunuGI::Vector3(x, y, z);
}

void ParticleRenderer::SetSpeed(float speed)
{
	this->speed = speed;
}

void ParticleRenderer::SetScale(float scale)
{
	this->scale = scale;
}

void ParticleRenderer::SetLifeTime(float lifeTime)
{
	this->lifeTime = lifeTime;
}

void ParticleRenderer::SetRateOverTime(float rateOverTime)
{
	this->rateOverTime = rateOverTime;
}

void ParticleRenderer::SetLoop(bool isLoop)
{
	this->isLoop = isLoop;
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
			particle.direction = this->GenerateRandomDirectionInCone(60).Normalize(this->GenerateRandomDirectionInCone(60));
			this->disableParticles.pop_front();

			this->ableParticles.push_back(particle);
		}
		// 생성된 입자들은 포지션, 스케일 업데이트
		for (auto iter = this->ableParticles.begin(); iter != this->ableParticles.end();)
		{
			auto& each = *iter;

			each.curLifeTime += Time::GetDeltaTime();

			each.lifeTime = this->lifeTime;
			each.scale = this->scale;

			// 생성된 입자의 생명주기가 다하면 다시 비활성
			if (each.curLifeTime >= each.lifeTime)
			{
				each.Reset();
				this->disableParticles.push_back(*iter);

				iter = this->ableParticles.erase(iter);

				continue;
			}

			each.position += (each.direction *this->speed * Time::GetDeltaTime());
			
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
		else
		{
			for (auto iter = this->ableParticles.begin(); iter != this->ableParticles.end();)
			{
				auto& each = *iter;

				each.curLifeTime += Time::GetDeltaTime();

				each.lifeTime = this->lifeTime;
				each.scale = this->scale;

				// 생성된 입자의 생명주기가 다하면 다시 비활성
				if (each.curLifeTime >= each.lifeTime)
				{
					each.Reset();
					this->disableParticles.push_back(*iter);

					iter = this->ableParticles.erase(iter);

					continue;
				}

				each.position += (each.direction * this->speed * Time::GetDeltaTime());
				++iter;
			}
		}
	}
}