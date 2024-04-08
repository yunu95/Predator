#pragma once
#include "Object.h"
#include "Component.h"
#include "Rect.h"
#include "YunuGraphicsInterface.h"
#include "IYunuGIParticleRenderer.h"
#include "YunutyRenderable.h"

#include <deque>
#include <list>

// 카메라는 화면에 게임상에 존재하는 모든 그래픽 요소들을 출력하는 데에 쓰이는 객체입니다. 
// 카메라의 구현은 엔진의 동작환경,혹은 카메라의 특성에 따라 달라질 수 있기 때문에,
// 화면을 그려내기 위해 쓰이는 함수 Render는 추상 메소드로 정의됩니다.
#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine::graphics
{
	enum class ParticleType
	{
		Cone,
		Circle,
	};

	enum class ParticleMode
	{
		Default,
		Bursts,
	};

	struct Cone
	{
		float radius = 1.f;
		float angle = 25.f;
	};

	struct Circle
	{
		float radius = 1.f;
	};

	union Shape
	{
		Cone cone;
		Circle circle;
	};

	class YunutyCycle;
	class YUNUTY_API ParticleRenderer : public Renderable<yunuGI::IParticleRenderer>
	{
	protected:
	public:
		// GI는 Graphics Interface라는 뜻임.
		yunuGI::IParticleRenderer& GetGI() { return Renderable<yunuGI::IParticleRenderer>::GetGI(); }
		ParticleRenderer();

		virtual void Update() override;

		void ParticleUpdate();

		void SetParticleShape(ParticleType particleType);
		void SetMaxParticle(unsigned int maxParticle);
		void SetRateOverTime(float rateOverTime);
		void Play();
		void SetLoop(bool isLoop);
		void SetLifeTime(float lifeTime);
		void SetSpeed(float speed);

		void SetRadius(float radius);
		void SetAngle(float angle);

		void SetStartScale(float scale);
		void SetEndScale(float scale);

	private:
		yunuGI::Vector3 GenerateRandomDirectionInCone(yunuGI::ParticleRenderInfo& particle);
		yunuGI::Vector2 getRandomPointInCircle(double centerX, double centerY, double radius);

	private:
		ParticleType particleType = ParticleType::Cone;

		bool isPlay = false;

		double accTime = 0.f;
		bool isLoop = false;

		double curCreationCycle = 0.f;
		float rateOverTime = 10.f;

		float lifeTime = 5.f;
		float speed = 1.f;

		float startScale = 1.f;
		float curScale = 0.f;
		float endScale = 1.f;

		unsigned int maxParticle = 500;

		std::deque<yunuGI::ParticleRenderInfo> disableParticles;
		std::list<yunuGI::ParticleRenderInfo> ableParticles;

		Shape shape;
		ParticleMode mode = ParticleMode::Default;
	};
}
