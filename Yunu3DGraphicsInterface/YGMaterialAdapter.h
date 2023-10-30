#pragma once
#include "IMaterial.h"

namespace yunuGIAdapter
{
	class MaterialAdapter : public yunuGI::IMaterial
	{
	public:
		virtual void SetMaterialName(const std::wstring& materialName) override {};
		virtual void SetVertexShader(const std::wstring& vertexShader) override {};
		virtual void SetPixelShader(const std::wstring& pixelShader) override {};
		virtual void SetTexture(const int index, const std::wstring& texture) override {};
		virtual void SetColor(const yunuGI::Color& color) override {};
	};
}