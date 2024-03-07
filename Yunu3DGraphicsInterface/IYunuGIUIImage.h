#pragma once
#include <windows.h>
#include "IYunuGIUIObject.h"

namespace yunuGI
{
	class ITexture;
	class IUIImage : public yunuGI::IUIObject
	{
	public:
		virtual void SetImage(ITexture* texture) = 0;
		virtual float GetWidth() = 0;
		virtual float GetHeight() = 0;
		virtual int GetLayer() = 0;
	};
	struct IUIImageDesc
	{
		
	};
}
