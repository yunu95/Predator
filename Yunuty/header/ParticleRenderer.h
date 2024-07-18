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

/// <summary>
/// 만일 Scale을 Random값으로 하겠다고 하면 startScale과 endScale 값 사이 중 랜덤한 값으로 렌더링됨.
/// 만일 Angle을 Random값으로 하겠다고 하면 startAngle과 endAngle 값 사이 중 랜덤한 값으로 렌더링됨.
/// 만일 AlphaDiminish가 true면 생명주기가 끝나갈수록 알파값이 빠짐.
/// </summary>
 
namespace yunutyEngine::graphics
{
	enum class ParticleShape
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
		virtual void OnDisable() override;

		void Play();
		void Pause();
		void Resume();

		void SetIsApplyRoot(bool val) { this->isApplyRoot = val; }
		bool IsApplyRoot() { return this->isApplyRoot; }
		void Reset();

		void ParticleUpdate();

		void SetDuration(float duration);
		float GetDuration();

		void SetParticleShape(ParticleShape particleType);
		void SetParticleMode(ParticleMode particleMode);
		void SetMaxParticle(unsigned int maxParticle);
		void SetRateOverTime(float rateOverTime);
	
		void SetLoop(bool isLoop);
		void SetLifeTime(float lifeTime);
		void SetSpeed(float speed);

		void SetRadius(float radius);
		void SetAngle(float angle);

		void SetStartScale(float scale);
		void SetEndScale(float scale);

		/// 새로 추가된 함수들
		void SetStartAngle(float angle);
		void SetEndAngle(float angle);

		void SetIsRandomScale(bool val);
		void SetIsRandomAngle(bool val);

		void SetIsAlphaDiminish(bool val);
		/// 

		void SetInterval(float interval);
		void SetBurstsCount(int count);

		void SetPlayAwake(bool playAwake);

		void SetTexture(yunuGI::ITexture* texture);

	private:
		yunuGI::Matrix4x4 GenerateRandomOffsetMatInCone();
		yunuGI::Vector2 GetRandomPointInCircle(double centerX, double centerY, double radius);

	private:
		bool isApplyRoot = true;
		bool isPause = false;
		bool isPlay = false;
		ParticleShape particleType = ParticleShape::Cone;

		double accTime = 0.f;
		bool isLoop = false;

		double curCreationCycle = 0.f;
		float rateOverTime = 10.f;

		float lifeTime = 5.f;
		float speed = 1.f;

		float startScale = 1.f;
		float curScale = 0.f;
		float endScale = 1.f;

		float startAngle = 0.f;
		float curAngle = 0.f;
		float endAngle = 0.f;

		unsigned int maxParticle = 500;

		float interval = 5.f;
		int burstsCount = 30;
		float burstsAngle = 12 * (3.14159265358979323846 / 180.f);
		bool playAwake = true;
		bool originPlayAwake = true;
		bool isRandomScale = false;
		bool isRandomAngle = false;
		bool isAlphaDiminish = false;

		std::deque<yunuGI::ParticleRenderInfo> disableParticles;
		std::list<yunuGI::ParticleRenderInfo> ableParticles;

		Shape shape;
		ParticleMode particleMode = ParticleMode::Default;
	};
}
