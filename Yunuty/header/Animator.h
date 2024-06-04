#pragma once
#include "Object.h"
#include "Component.h"
#include "YunuGraphicsInterface.h"
#include "YunutyRenderable.h"

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
	class YunutyCycle;

	struct AnimationEvent
	{
		unsigned int frame;
		bool isFirst;
		std::function<void()> func;
	};

	class YUNUTY_API Animator : public Renderable<yunuGI::IAnimator>
	{
	protected:
	public:
		// GI는 Graphics Interface라는 뜻임.
		yunuGI::IAnimator& GetGI() { return Renderable<yunuGI::IAnimator>::GetGI(); }
		Animator();
		virtual void Update() override;
		virtual void OnDisable() override;
		virtual void OnEnable() override;
		virtual ~Animator() {};
		void Pause();
		void Play(yunuGI::IAnimation* animation);
		void PushAnimation(yunuGI::IAnimation* animation);
		void SetAnimationFrame(yunuGI::IAnimation* animation, unsigned int frame);
		void ChangeAnimation(yunuGI::IAnimation* animation, float transitionDuration, float transitionSpeed);
		
		/// isPlay 값을 리턴합니다.
		bool IsPlaying() { return isPlay; }

		/// AnimationEvent 의 경우, Push 뿐만이 아니라 Erase 에 대한 대처도 필요합니다.
		/// 일반적인 Push 상황과 별개로 functor 를 등록하는 경우에는 Index 를 리턴받도록 합니다.
		unsigned long long PushAnimationWithFunc(yunuGI::IAnimation* animation, unsigned int frame, std::function<void()> func);
		bool EraseAnimationFunc(yunuGI::IAnimation* animation, unsigned long long index);

		/// Animation 이 플레이 중인 경우 해당 프레임을 반환하는 함수입니다.
		/// 플레이 중이지 않을 경우, 0을 return 합니다.
		float GetCurrentFrame();

	private:
		void ClearAnimationEvent(yunuGI::IAnimation* animation);

	private:
		bool isChange = false;
		bool isPlay = false;
		std::map<yunuGI::IAnimation*, std::map<unsigned long long, AnimationEvent>> animationEventMap;
		unsigned long long functorIndex = 1;
	};
}



