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
	};
	struct IUIImageDesc
	{
		
	};
}
