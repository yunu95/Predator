#pragma once

#include <memory>
#include <string>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "Material.h"

#include "Mesh.h"

class yunuGI::IMaterial;

namespace nail
{
	class IRenderable
	{
	public:
		IRenderable();
#pragma region Setter
		virtual void SetWorldTM(const DirectX::SimpleMath::Matrix& wtm)
		{
			this->wtm = wtm;
		};
		virtual void SetMesh(Mesh* mesh)
		{
			this->mesh = mesh;
		}
		virtual void SetMaterial(unsigned int index, yunuGI::IMaterial* material)
		{
			// 새로운 Material이라면
			if (index + 1 > this->materialVec.size())
			{
				this->materialVec.emplace_back(reinterpret_cast<Material*>(material));
			}
			else
			{
				this->materialVec[index] = reinterpret_cast<Material*>(material);
			}
		}
		virtual void SetActive(bool isActive)
		{
			this->isActive = isActive;
		}
#pragma endregion

#pragma region Getter
		DirectX::SimpleMath::Matrix& GetWorldTM() { return this->wtm; }
		Mesh* GetMesh()
		{
			return this->mesh;
		}
		Material* GetMaterial(unsigned int index)
		{
			return materialVec[index];
		}
		bool IsActive()
		{
			return isActive;
		}
		bool IsStatic() { return isStatic; }

		float GetID() { return this->renderableID; }
#pragma endregion

	protected:
		DirectX::SimpleMath::Matrix wtm;
		Mesh* mesh{ nullptr };
		std::vector<Material*> materialVec;
		bool isActive = true;
		bool isStatic;

		float renderableID;
		static float objectID;
	};

}