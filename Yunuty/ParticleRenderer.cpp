#include "YunutyEngine.h"
#include "ParticleRenderer.h"
#include "_YunuGIObjects.h"

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
    this->particleMode = particleMode;
}

void ParticleRenderer::DirectionUpate()
{
    for (auto iter = this->ableParticles.begin(); iter != this->ableParticles.end();)
    {
        auto& each = *iter;

        auto tempDir = yunuGI::Vector4(each.direction.x, each.direction.y, each.direction.z, 0.f)
            * GetTransform()->GetWorldTM();
        each.direction = yunuGI::Vector3{ tempDir.x,tempDir.y, tempDir.z };

        ++iter;
    }
}

void ParticleRenderer::OnTransformUpdate()
{
    Renderable<yunuGI::IParticleRenderer>::OnTransformUpdate();

    DirectionUpate();
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
    this->shape.cone.radius = radius;
}

yunuGI::Vector2 ParticleRenderer::getRandomPointInCircle(double centerX, double centerY, double radius)
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
        float curScale = yunutyEngine::math::LerpF(this->startScale, this->endScale, ratio);
        each.curScale = curScale;

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

yunuGI::Vector3 ParticleRenderer::GenerateRandomDirectionInCone(yunuGI::ParticleRenderInfo& particle)
{
    auto randomPoint = getRandomPointInCircle(0, 0, this->shape.cone.radius);

    float height = 4.f;

    float theta = this->shape.cone.angle * (3.14159265358979323846 / 180);

    yunuGI::Vector3 bottomPos{ randomPoint.x, 0, randomPoint.y };
    particle.position = bottomPos;

    float bottomHeight = this->shape.cone.radius / tan(theta);

    float upCircleRadius = tan(theta) * (height + (this->shape.cone.radius / tan(theta)));

    yunuGI::Vector3 topPos{ upCircleRadius * bottomPos.x / this->shape.cone.radius ,
        height,
        upCircleRadius * bottomPos.z / this->shape.cone.radius
    };

    return yunuGI::Vector3(topPos.x - bottomPos.x, topPos.y - bottomPos.y, topPos.z - bottomPos.z).Normalize(
        yunuGI::Vector3(topPos.x - bottomPos.x, topPos.y - bottomPos.y, topPos.z - bottomPos.z));
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
        if (this->particleMode == ParticleMode::Default)
        {
            if (curCreationCycle >= (1.f / rateOverTime))
            {
                curCreationCycle -= (1.f / rateOverTime);

                // 이제 입자 활성화
                if (!disableParticles.empty())
                {
                    auto& particle = this->disableParticles.front();

                    if (this->particleType == ParticleShape::Cone)
                    {

                        particle.direction = this->GenerateRandomDirectionInCone(particle);

                        auto worldPos = yunuGI::Vector4(particle.position.x, particle.position.y, particle.position.z, 0.f)
                            * GetTransform()->GetWorldTM();

                        particle.position = yunuGI::Vector3{ worldPos.x,worldPos.y,worldPos.z };


                        auto tempDir = yunuGI::Vector4(particle.direction.x, particle.direction.y, particle.direction.z, 0.f)
                            * GetTransform()->GetWorldTM();

                        particle.direction = yunuGI::Vector3{ tempDir.x,tempDir.y, tempDir.z };
                    }
                    else if (this->particleType == ParticleShape::Circle)
                    {
                        auto tempPos = getRandomPointInCircle(0, 0, this->shape.circle.radius);

                        particle.position = yunuGI::Vector3{ tempPos.x,0,tempPos.y };

                        auto worldPos = yunuGI::Vector4(particle.position.x, particle.position.y, particle.position.z, 0.f)
                            * GetTransform()->GetWorldTM();

                        particle.position = yunuGI::Vector3{ worldPos.x,worldPos.y,worldPos.z };

                        particle.direction = yunuGI::Vector3{ tempPos.x,0,tempPos.y }.Normalize(yunuGI::Vector3{ tempPos.x,0,tempPos.y });;

                        auto tempDir = yunuGI::Vector4(particle.direction.x, particle.direction.y, particle.direction.z, 0.f)
                            * GetTransform()->GetWorldTM();
                        particle.direction = yunuGI::Vector3{ tempDir.x,tempDir.y, tempDir.z };
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
                        auto& particle = this->disableParticles.front();

                        if (this->particleType == ParticleShape::Cone)
                        {
                            particle.direction = this->GenerateRandomDirectionInCone(particle);

                            auto worldPos = yunuGI::Vector4(particle.position.x, particle.position.y, particle.position.z, 0.f)
                                * GetTransform()->GetWorldTM();

                            particle.position = yunuGI::Vector3{ worldPos.x,worldPos.y,worldPos.z };


                            auto tempDir = yunuGI::Vector4(particle.direction.x, particle.direction.y, particle.direction.z, 0.f)
                                * GetTransform()->GetWorldTM();

                            particle.direction = yunuGI::Vector3{ tempDir.x,tempDir.y, tempDir.z };
                        }
                        else if (this->particleType == ParticleShape::Circle)
                        {
                            auto tempPos = getRandomPointInCircle(0, 0, this->shape.circle.radius);

                            particle.position = yunuGI::Vector3{ tempPos.x,0,tempPos.y };

                            auto worldPos = yunuGI::Vector4(particle.position.x, particle.position.y, particle.position.z, 0.f)
                                * GetTransform()->GetWorldTM();

                            particle.position = yunuGI::Vector3{ worldPos.x,worldPos.y,worldPos.z };

                            particle.direction = yunuGI::Vector3{ tempPos.x,0,tempPos.y }.Normalize(yunuGI::Vector3{ tempPos.x,0,tempPos.y });;

                            auto tempDir = yunuGI::Vector4(particle.direction.x, particle.direction.y, particle.direction.z, 0.f)
                                * GetTransform()->GetWorldTM();
                            particle.direction = yunuGI::Vector3{ tempDir.x,tempDir.y, tempDir.z };
                        }

                        this->disableParticles.pop_front();
                        this->ableParticles.push_back(particle);
                    }
                }
            }
        }
        // 생성된 입자들은 포지션, 스케일 업데이트
        ParticleUpdate();
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
            ParticleUpdate();
        }
    }
}