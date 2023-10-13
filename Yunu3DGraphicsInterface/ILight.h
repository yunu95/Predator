#pragma once
#include "IYunuGIRenderable.h"

#include "YunuGIMatrix4x4.h"
#include "YunuVector3.h"
#include "YunuVector4.h"
#include "YunuGIColor.h"

#define MAX_LIGHT 50

namespace yunuGI
{
	enum class LightType
	{
		Directional,
		Point,
		Spot
	};

	struct LightColor
	{
		Vector4 diffuse;
		Vector4 ambient;
		Vector4 specular;
	};

	struct LightInfo
	{
		unsigned int lightType;
		LightColor color;
		Vector4 position;
		Vector4 direction;
		float range;
		float angle;
		float padding;
	};

	struct LightParams
	{
		unsigned int lightCount;
		LightInfo lights[MAX_LIGHT];
		Vector3 padding;
	};

	class ILight : virtual public IRenderable
	{
	public:
		virtual void SetLightDiffuseColor(yunuGI::Color& color) = 0;
	};
}