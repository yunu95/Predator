#pragma once
#include "IResource.h"

namespace yunuGI
{
	class IAnimation : virtual public IResource
	{
	public:
		virtual void SetDuration(float duration) = 0;
		virtual float GetDuration() = 0;

		virtual void SetTotalFrame(int totalFrame) = 0;
		virtual float GetTotalFrame() = 0;

		virtual void SetLoop(bool isLoop) = 0;
		virtual bool GetLoop() = 0;

	protected:
		float duration;
		int totalFrame;
		bool isLoop;

	private:

	};
}