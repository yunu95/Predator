#pragma once
#include "IUnknown.h"

#include "YunuGIColor.h"

#include <string>

namespace yunuGI
{
	enum class Texture_Type
	{
		ALBEDO,
		NORMAL,
		ARM,
		HEIGHT,
		EMISSION,
		Temp0,
		Temp1,
		Temp2,
	};

	struct MaterialDesc
	{
		std::wstring materialName;
	};

	class IMaterial : public IUnknown
	{
	public:
		virtual void SetMaterialName(const std::wstring& materialName) = 0;
		virtual void SetVertexShader(const std::wstring& vertexShader) = 0;
		virtual void SetPixelShader(const std::wstring& pixelShader) = 0;
		virtual void SetTexture(Texture_Type textureType, const std::wstring& texture) = 0;
		virtual void SetColor(const yunuGI::Color& color) = 0;
		virtual void SetInt(int index, int val) = 0;

		virtual const std::wstring& GetMaterialName()const = 0;

	private:

	};
}