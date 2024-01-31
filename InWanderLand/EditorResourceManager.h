/// 2024. 01. 30 김상준
/// 에디터에서 사용할 Resource 를 관리하는 Manager

#pragma once

#include "Singleton.h"

#include "imgui.h"

#include <unordered_map>
#include <string>

namespace application
{
	namespace editor
	{
		class ResourceManager;

		// Texture 관리를 위한 클래스
		// DX11 의 경우, ShaderResourceView* 형태로 사용됩니다.
		class Texture2D
		{
			friend class ResourceManager;

		public:
			virtual ~Texture2D();

			Texture2D() = default;
			Texture2D(Texture2D&& texture) noexcept;
			Texture2D& operator=(Texture2D&& texture) noexcept;
			Texture2D(const Texture2D&) = delete;
			Texture2D& operator=(const Texture2D&) = delete;

			void* GetID() { return textureAddress; }
			float GetWidth() { return width; }
			float GetHeight() { return height; }

		private:
			void** GetTextureAddressPointer() { return &textureAddress; }
			void* textureAddress = nullptr;
			float width = 0.f;
			float height = 0.f;
		};

		class ResourceManager
			: public Singleton<ResourceManager>
		{
			friend class Singleton<ResourceManager>;

		public:
			void Initialize(void* device);
			Texture2D* GetTexture2D(std::string filename);
			bool LoadTextureFromFile(std::string filename);

		private:
			ResourceManager() = default;

			void* d3dDevice = nullptr;

			// Editor\Resources 폴더로부터 상대적 경로를 Key 값으로 합니다.
			// 해당 Key 값을 통해 LoadTextureFromFile 함수를 호출합니다.
			std::unordered_map<std::string, std::unique_ptr<Texture2D>> textureMap = std::unordered_map<std::string, std::unique_ptr<Texture2D>>();
		};
	}
}

