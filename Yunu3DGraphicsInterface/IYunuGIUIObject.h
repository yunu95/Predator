#pragma once
#include <array>
#include "IYunuGIRenderable.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"
#include "YunuGIColor.h"

namespace yunuGI
{
	class IUIObject :  public virtual yunuGI::IRenderable
	{
	public:
		virtual void SetPickingMode(bool isPickingModeOn) = 0;
		virtual void SetLayer(int layer) = 0;
	};
}