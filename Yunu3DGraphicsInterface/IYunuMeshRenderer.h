#pragma once

#include "IYunuGIRenderable.h"

namespace yunuGI
{
	class IMaterial;
	class IMesh;

	class IMeshRenderer : public IRenderable
	{
	public:
		virtual void SetMaterial(unsigned int index, IMaterial* material, bool isOrigin = false) = 0;
		virtual IMaterial* GetMaterial(unsigned int index = 0, bool isInstance = true) = 0;
		virtual int GetMaterialCount() = 0;
		virtual void SetMesh(IMesh* mesh) = 0;
		virtual IMesh* GetMesh() const= 0;
	};
}
