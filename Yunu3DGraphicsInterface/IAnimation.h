#pragma once
#include "IResource.h"
#include <functional>
#include <map>
namespace yunuGI
{
	class IAnimation : virtual public IResource
	{
	public:
		virtual void SetDuration(float duration) = 0;
		virtual float GetDuration() = 0;

		virtual void SetTotalFrame(int totalFrame) = 0;
		virtual float GetTotalFrame() = 0;

		virtual void SetPlaySpeed(float playSpeed) = 0;
		virtual float GetPlaySpeed() = 0;

		virtual void SetLoop(bool isLoop) = 0;
		virtual bool GetLoop() = 0;

		virtual void SetEventFunc(int frame, std::function<void()> func) = 0;
		virtual std::map<int, std::pair<bool,std::function<void()>>>& GetEventFuncMap() = 0;
		virtual void ClearEvent() = 0;
	private:

	};
}