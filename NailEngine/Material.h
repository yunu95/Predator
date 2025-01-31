#pragma once
#include "IMaterial.h"

#include "Resource.h"

#include <array>

#include "Struct.h"

class VertexShader;
class PixelShader;
class Texture;
class yunuGI::IShader;
class yunuGI::ITexture;

class Material : public yunuGI::IMaterial, public Resource
{
public:
	Material();
	~Material();
	Material(const Material& rhs);
	void operator=(const Material& rhs);
public:
	virtual void SetVertexShader(const yunuGI::IShader* shader) override;
	virtual void SetPixelShader(const yunuGI::IShader* shader) override;
	virtual void SetTexture(yunuGI::Texture_Type textureType, const yunuGI::ITexture* texture) override;
	virtual void SetColor(const yunuGI::Color& color) override;
	virtual void SetInt(int index, int val) override;
	virtual void SetFloat(int index, float val) override;
	virtual const yunuGI::IShader* GetPixelShader() const override;
	virtual const yunuGI::IShader* GetVertexShader() const override;
	virtual yunuGI::ITexture* GetTexture(yunuGI::Texture_Type textureType) override;
	virtual yunuGI::IMaterial* GetMaterial() override;
	virtual yunuGI::Color& GetColor() override;
public:
	void PushGraphicsData();
	void UnBindGraphicsData();

private:
	yunuGI::Color color{1.f,1.f,1.f,1.f};

	std::shared_ptr<VertexShader> vs;
	std::shared_ptr<PixelShader> ps;

	std::array<std::shared_ptr<Texture>, MAX_TEXTURE> textures;
	std::array<unsigned int, MAX_TEXTURE> useTextures;
	std::array<int, MAX_INT> temp_int;
	std::array<float, MAX_FLOAT> temp_float;
	int lightIndex;

	MaterialBuffer materialBuffer;
};

