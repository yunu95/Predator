#include "YunutyEngine.h"
#include "ParticleRenderer.h"
#include "_YunuGIObjects.h"

#include <DirectXMath.h>

#include <random>
#include <chrono>
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::ParticleRenderer::ParticleRenderer() :
	Renderable<yunuGI::IParticleRenderer>(_YunuGIObjects::SingleInstance().factory->CreateParticleRenderer({})), shape{}
{
	disableParticles.assign(this->maxParticle, yunuGI::ParticleRenderInfo{});
	GetGI().SetParticleInfoList(this->ableParticles);
}

void ParticleRenderer::SetPlayAwake(bool playAwake)
{
	this->playAwake = playAwake;
	this->originPlayAwake = playAwake;
}


void ParticleRenderer::OnDisable()
{
	this->playAwake = this->originPlayAwake;

	for (auto iter = this->ableParticles.begin(); iter != this->ableParticles.end();)
	{
		auto& each = *iter;

		each.Reset();
		this->disableParticles.push_back(*iter);

		iter = this->ableParticles.erase(iter);
	}
}

void ParticleRenderer::SetParticleMode(ParticleMode particleMode)
{
	if (this->particleMode != particleMode)
	{
		// 모드가 바뀐다면 지금까지 활성화된 파티클은 모두 reset하고 disable
		for (auto iter = this->ableParticles.begin(); iter != this->ableParticles.end();)
		{
			auto& each = *iter;
			each.Reset();
			this->disableParticles.push_back(each);

			iter = this->ableParticles.erase(iter);
		}
	}

	this->particleMode = particleMode;
}

void ParticleRenderer::SetEndScale(float scale)
{
	this->endScale = scale;
}

void ParticleRenderer::SetInterval(float interval)
{
	this->interval = interval;
}

void ParticleRenderer::SetBurstsCount(int count)
{
	burstsCount = count;
}

void ParticleRenderer::SetStartScale(float scale)
{
	this->startScale = scale;
}

void ParticleRenderer::SetAngle(float angle)
{
	this->shape.cone.angle = angle;
}

void ParticleRenderer::SetRadius(float radius)
{
	radius = std::abs(radius);
	this->shape.cone.radius = radius;
}

yunuGI::Vector2 ParticleRenderer::GetRandomPointInCircle(double centerX, double centerY, double radius)
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 gen(seed);
	std::uniform_real_distribution<double> distRadius(0, radius); // 0부터 반지름까지의 랜덤한 거리
	std::uniform_real_distribution<double> distAngle(0, 2 * 3.14159265358979323846); // 0부터 2π(360도) 사이의 랜덤한 각도

	// 랜덤한 거리와 각도 생성
	float randomRadius = distRadius(gen);
	float randomAngle = distAngle(gen);

	// 랜덤한 각도와 반지름을 사용하여 원 안의 점 좌표 계산
	float x = centerX + randomRadius * cos(randomAngle);
	float y = centerY + randomRadius * sin(randomAngle);

	return yunuGI::Vector2{ x, y };
}

void ParticleRenderer::SetParticleShape(ParticleShape particleType)
{
	this->particleType = particleType;
}

void ParticleRenderer::ParticleUpdate()
{
	for (auto iter = this->ableParticles.begin(); iter != this->ableParticles.end();)
	{
		auto& each = *iter;

		each.curLifeTime += Time::GetDeltaTime();

		each.lifeTime = this->lifeTime;

		float ratio = each.curLifeTime / each.lifeTime;
		if (!this->isRandomScale)
		{
			float curScale = yunutyEngine::math::LerpF(this->startScale, this->endScale, ratio);
			each.curScale = curScale;
		}
		
		if (!this->isRandomAngle)
		{
			float curAngle = yunutyEngine::math::LerpF(this->startAngle, this->endAngle, ratio);
			each.particleAngle = curAngle;
		}
		
		if (this->isAlphaDiminish)
		{
			each.alpha = yunutyEngine::math::LerpF(1, 0, ratio);
		}

		// 생성된 입자의 생명주기가 다하면 다시 비활성
		if (each.curLifeTime >= each.lifeTime)
		{
			each.Reset();
			this->disableParticles.push_back(*iter);

			iter = this->ableParticles.erase(iter);

			continue;
		}

		yunuGI::Matrix4x4 curWorldTM;
		auto particleDirection = yunuGI::Vector3{ each.offsetTM.m31, each.offsetTM.m32 ,each.offsetTM.m33 };
		auto particlePosition = yunuGI::Vector3{ each.offsetTM.m41, each.offsetTM.m42 ,each.offsetTM.m43 };

		if (this->isApplyRoot)
		{
			curWorldTM = GetTransform()->GetWorldTM();
		}

		particlePosition += (particleDirection * this->speed * Time::GetDeltaTime());

		each.offsetTM.m41 = particlePosition.x;
		each.offsetTM.m42 = particlePosition.y;
		each.offsetTM.m43 = particlePosition.z;

		auto resultTM = each.offsetTM * curWorldTM;

		each.position = yunuGI::Vector3{ resultTM.m41,resultTM.m42,resultTM.m43 };

		++iter;
	}
}

void yunutyEngine::graphics::ParticleRenderer::SetDuration(float duration)
{
	GetGI().SetDuration(duration);
}

float yunutyEngine::graphics::ParticleRenderer::GetDuration()
{
	return GetGI().GetDuration();
}

void ParticleRenderer::SetIsAlphaDiminish(bool val)
{
	this->isAlphaDiminish = val;
}

void ParticleRenderer::SetIsRandomAngle(bool val)
{
	this->isRandomAngle = val;
}

void ParticleRenderer::SetIsRandomScale(bool val)
{
	this->isRandomScale = val;
}

void ParticleRenderer::SetEndAngle(float angle)
{
	this->endAngle = angle;
}

void ParticleRenderer::SetStartAngle(float angle)
{
	this->startAngle = angle;
}

void ParticleRenderer::Resume()
{
	this->isPause = false;
}

void ParticleRenderer::Pause()
{
	this->isPause = true;
}

void ParticleRenderer::Reset()
{
	if (isLoop) return;

	this->isPlay = true;

	this->accTime = 0.f;
	this->curCreationCycle -= (1.f / rateOverTime);


	if (this->particleMode == ParticleMode::Bursts)
	{
		this->curCreationCycle = 0.f;
		this->playAwake = this->originPlayAwake;
	}
	else
	{
		this->accTime = 0.f;
		this->curCreationCycle -= (1.f / rateOverTime);
	}


}

void ParticleRenderer::SetTexture(yunuGI::ITexture* texture)
{
	GetGI().SetTexture(texture);
}

yunuGI::Matrix4x4 ParticleRenderer::GenerateRandomOffsetMatInCone()
{
	auto randomPoint = GetRandomPointInCircle(0, 0, this->shape.cone.radius);

	float height = 4.f;

	float theta = this->shape.cone.angle * (3.14159265358979323846 / 180);

	yunuGI::Vector3 bottomPos{ randomPoint.x, 0, randomPoint.y };

	float bottomHeight = this->shape.cone.radius / tan(theta);

	float upCircleRadius = tan(theta) * (height + (this->shape.cone.radius / tan(theta)));

	yunuGI::Vector3 topPos{ upCircleRadius * bottomPos.x / this->shape.cone.radius ,
		height,
		upCircleRadius * bottomPos.z / this->shape.cone.radius
	};

	auto direction = yunuGI::Vector3(topPos.x - bottomPos.x, topPos.y - bottomPos.y, topPos.z - bottomPos.z).Normalize(
		yunuGI::Vector3(topPos.x - bottomPos.x, topPos.y - bottomPos.y, topPos.z - bottomPos.z));

	yunuGI::Matrix4x4 offsetMatrix;
	offsetMatrix.m31 = direction.x;
	offsetMatrix.m32 = direction.y;
	offsetMatrix.m33 = direction.z;

	offsetMatrix.m41 = bottomPos.x;
	offsetMatrix.m42 = bottomPos.y;
	offsetMatrix.m43 = bottomPos.z;

	return (offsetMatrix);
}

void ParticleRenderer::SetSpeed(float speed)
{
	this->speed = speed;
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
	if (maxParticle < 1)
	{
		maxParticle = 1;
	}

	// 항상 able + disable 갯수가 max가 되어야함

	// 현재 활성화 되어 있는 입자보다 max가 작으면 able에서 max만큼만 남기고 다 비활성
	if (this->ableParticles.size() > maxParticle)
	{
		for (auto iter = this->ableParticles.begin(); iter != this->ableParticles.end();)
		{
			auto& each = *iter;
			each.Reset();
			this->disableParticles.push_back(each);

			iter = this->ableParticles.erase(iter);

			if (this->ableParticles.size() == maxParticle)
			{
				break;
			}
		}

		this->disableParticles.clear();
	}
	else
	{
		// 만일 활성화된 입자보다 max가 크다면 disable size만 조정
		this->disableParticles.resize(maxParticle - this->ableParticles.size());
	}


	this->maxParticle = maxParticle;
}

void ParticleRenderer::Play()
{
	this->playAwake = this->originPlayAwake;

	for (auto iter = this->ableParticles.begin(); iter != this->ableParticles.end();)
	{
		auto& each = *iter;

		each.Reset();
		this->disableParticles.push_back(*iter);

		iter = this->ableParticles.erase(iter);
	}

	this->isPlay = true;

	this->accTime = 0.f;
	this->curCreationCycle -= (1.f / rateOverTime);


	if (this->particleMode == ParticleMode::Bursts)
	{
		this->curCreationCycle = 0.f;
		this->playAwake = this->originPlayAwake;
	}
	else
	{
		this->accTime = 0.f;
		this->curCreationCycle -= (1.f / rateOverTime);
	}
}

void ParticleRenderer::Update()
{
	auto& gi = GetGI();

	// 현재 파티클이 재생된 총 시간량이 Duration보다 작을 때 만 Update 돌기
	if ((accTime <= gi.GetDuration()) && isPlay && !isPause)
	{
		accTime += Time::GetDeltaTime();
		curCreationCycle += Time::GetDeltaTime();

		// 만일 입자를 생성할 시기가 되면 설정해둔 입자 생성 갯수만큼 활성
		if (this->particleMode == ParticleMode::Default)
		{
			if (curCreationCycle >= (1.f / rateOverTime))
			{
				curCreationCycle -= (1.f / rateOverTime);

				// 이제 입자 활성화
				if (!disableParticles.empty())
				{
					auto& particle = this->disableParticles.front();
					if (this->isRandomScale)
					{
						particle.curScale = yunutyEngine::math::Random::GetRandomFloat(this->startScale, this->endScale);
					}
					if (this->isRandomAngle)
					{
						particle.particleAngle = yunutyEngine::math::Random::GetRandomFloat(this->startAngle, this->endAngle);
					}

					if (this->particleType == ParticleShape::Cone)
					{
						particle.offsetTM = this->GenerateRandomOffsetMatInCone();

						if (!this->isApplyRoot)
						{
							auto tempPos = GetTransform()->GetWorldPosition();
							particle.offsetTM.m41 += tempPos.x;
							particle.offsetTM.m42 += tempPos.y;
							particle.offsetTM.m43 += tempPos.z;
						}
					}
					else if (this->particleType == ParticleShape::Circle)
					{
						auto tempPos = GetRandomPointInCircle(0, 0, this->shape.circle.radius);

						auto particleDirection = yunuGI::Vector3{ tempPos.x,0,tempPos.y }.Normalize(yunuGI::Vector3{ tempPos.x,0,tempPos.y });

						if (!this->isApplyRoot)
						{
							auto tempPos = GetTransform()->GetWorldPosition();
							particle.offsetTM.m41 += tempPos.x;
							particle.offsetTM.m42 += tempPos.y;
							particle.offsetTM.m43 += tempPos.z;
						}
						else
						{
							particle.offsetTM.m41 = tempPos.x;
							particle.offsetTM.m42 = 0.f;
							particle.offsetTM.m43 = tempPos.y;
						}

						particle.offsetTM.m31 = particleDirection.x;
						particle.offsetTM.m32 = particleDirection.y;
						particle.offsetTM.m33 = particleDirection.z;
					}

					this->disableParticles.pop_front();
					this->ableParticles.push_back(particle);
				}
			}
		}
		else
		{
			if ((curCreationCycle >= interval) || (this->playAwake))
			{
				if (this->playAwake)
				{
					this->playAwake = false;
				}
				else
				{
					curCreationCycle -= interval;
				}

				if (!disableParticles.empty())
				{
					for (int i = 0; i < burstsCount; ++i)
					{
						if (!this->disableParticles.empty())
						{
							auto& particle = this->disableParticles.front();
							if (this->isRandomScale)
							{
								particle.curScale = yunutyEngine::math::Random::GetRandomFloat(this->startScale, this->endScale);
							}
							if (this->isRandomAngle)
							{
								particle.particleAngle = yunutyEngine::math::Random::GetRandomFloat(this->startAngle, this->endAngle);
							}

							if (this->particleType == ParticleShape::Cone)
							{
								particle.offsetTM = this->GenerateRandomOffsetMatInCone();

								if (!this->isApplyRoot)
								{
									auto tempPos = GetTransform()->GetWorldPosition();
									particle.offsetTM.m41 += tempPos.x;
									particle.offsetTM.m42 += tempPos.y;
									particle.offsetTM.m43 += tempPos.z;
								}
							}
							else if (this->particleType == ParticleShape::Circle)
							{
								auto tempPos = GetRandomPointInCircle(0, 0, this->shape.circle.radius);

								auto particleDirection = yunuGI::Vector3{ tempPos.x,0,tempPos.y }.Normalize(yunuGI::Vector3{ tempPos.x,0,tempPos.y });

								if (!this->isApplyRoot)
								{
									auto tempPos = GetTransform()->GetWorldPosition();
									particle.offsetTM.m41 += tempPos.x;
									particle.offsetTM.m42 += tempPos.y;
									particle.offsetTM.m43 += tempPos.z;
								}
								else
								{
									particle.offsetTM.m41 = tempPos.x;
									particle.offsetTM.m42 = 0.f;
									particle.offsetTM.m43 = tempPos.y;
								}

								particle.offsetTM.m31 = particleDirection.x;
								particle.offsetTM.m32 = particleDirection.y;
								particle.offsetTM.m33 = particleDirection.z;
							}

							this->disableParticles.pop_front();
							this->ableParticles.push_back(particle);
						}
					}
				}
			}
		}
		// 생성된 입자들은 포지션, 스케일 업데이트
		ParticleUpdate();
	}
	else if (!isPause)
	{
		isPlay = false;

		if (isLoop)
		{
			isPlay = true;
			accTime = 0.f;
		}
		else
		{
			ParticleUpdate();
			accTime = 0.f;
		}
	}
}