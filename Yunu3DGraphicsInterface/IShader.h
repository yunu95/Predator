#pragma once
#include "IResource.h"

namespace yunuGI
{
	enum class ShaderType
	{
		Forward,
		Deferred,
		Light,
		Particle,
		Decal
	};

	enum class Rasterizer
	{
		Wire,
		Solid
	};

	struct ShaderInfo
	{
		ShaderType shaderType;
		Rasterizer rasterizer;
	};

	class IShader : virtual public IResource
	{
	public:
		virtual const ShaderInfo& GetShaderInfo() const = 0;
		virtual void Bind()const  = 0;
		virtual void UnBind()const = 0;

	private:

	protected:
		ShaderInfo shaderInfo;
	};
}