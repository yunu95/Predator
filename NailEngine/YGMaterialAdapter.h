#pragma once
#include "IMaterial.h"

#include <string>
#include <array>

#include "Struct.h"

namespace yunuGIAdapter
{
	class MaterialAdapter : public yunuGI::IMaterial
	{
	public:
		MaterialAdapter();
		MaterialAdapter(const MaterialAdapter& rhs);
		virtual void SetMaterialName(const std::wstring& materialName) override;
		virtual void SetVertexShader(const std::wstring& vertexShader) override;
		virtual void SetPixelShader(const std::wstring& pixelShader) override;
		virtual void SetTexture(yunuGI::Texture_Type textureType, const std::wstring& texture) override;
		virtual void SetColor(const yunuGI::Color& color) override;
		virtual void SetInt(int index, int val) override;
		virtual const std::wstring& GetMaterialName()const override;

		void SetMaterialDesc(const yunuGI::MaterialDesc& materialDesc);

	public:
		void PushGraphicsData();
		void UnBindGraphicsData();
		unsigned int GetID() { return this->id; }

protected:
		yunuGI::MaterialDesc materialDesc;

		yunuGI::Color color;
		std::wstring vs;
		std::wstring ps;
		std::array<std::wstring, MAX_TEXTURE> textures;
		std::array<unsigned int, MAX_TEXTURE> useTextures;
		std::array<int, MAX_INT> temp_int;
		int lightIndex;

		static unsigned int id;
	};
}