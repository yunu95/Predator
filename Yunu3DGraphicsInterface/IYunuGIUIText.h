#pragma once
#include <windows.h>
#include <string>
#include "IYunuGIUIObject.h"

namespace yunuGI
{
	class IUIText : public yunuGI::IUIObject
	{
	public:
		virtual void SetText(std::wstring str) = 0;
		virtual const std::wstring& GetText() = 0;
		virtual void SetLayer(int layer) {};
		virtual void SetFontSize(int size) =0;
		virtual void SetFont(std::wstring font) =0;
		virtual void SetColor(yunuGI::Color color) =0;
	};
	struct IUITextDesc
	{

	};
}
