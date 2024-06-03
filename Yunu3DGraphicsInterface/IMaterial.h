#pragma once
#include "IResource.h"

#include "YunuGIColor.h"

namespace yunuGI
{
	class IShader;
	class ITexture;

	enum class Texture_Type
	{
		ALBEDO,
		NORMAL,
		ARM,
		HEIGHT,
		EMISSION,
		OPACITY,
		Temp0,
		Temp1,
		Temp2,
		Temp3,
		Temp4,
		Temp5,
		Temp6,
		Temp7,
		Temp8,
		Temp9,
		Temp10,
		Temp11,
	};

	class IMaterial : virtual public IResource
	{
	public:
		virtual void SetVertexShader(const yunuGI::IShader* shader) = 0;
		virtual void SetPixelShader(const yunuGI::IShader* shader) = 0;
		virtual void SetTexture(Texture_Type textureType, const yunuGI::ITexture* texture) = 0;
		virtual void SetColor(const yunuGI::Color& color) = 0;
		virtual void SetInt(int index, int val) = 0;
		virtual void SetFloat(int index, float val) = 0;
		virtual yunuGI::ITexture* GetTexture(Texture_Type textureType) = 0;

		virtual const yunuGI::IShader* GetPixelShader() const = 0;
		virtual const yunuGI::IShader* GetVertexShader() const = 0;

		virtual IMaterial* GetMaterial() = 0;

	private:

	};
}